#include "game.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Sleep.hpp>

#include "core/Config.h"
#include "core/Material.h"
#include "core/Time.h"
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "world/World.h"
#include "world/Chunk.h"
#include "world/ChunkKey.h"
#include "world/ChunkLoader.h"
#include "world/LightPropagator.h"
#include "assets/PlayerSprite.h"
#include "game/MusicBank.h"
#include "game/SoundBank.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/Combat/SpriteFrame.h"
#include "support/Enemies/DwarfAI.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/GameContext.h"
#include "support/Progression/DropSystem.h"
#include "support/Progression/StratumManager.h"

// App: ciclo de vida (ctor/start/run/tick). Render em Renderer.cpp,
// eventos em Input.cpp, boot em Bootstrapper.cpp.

Game::Game()
{
}


void Game::start()
{
    if (running)
    {
        return;
    }
    running = true;
    //m_thread.launch();
}

void Game::run()
{
    // Preview de audição: PREVIEW_MUSIC=1 toca as 6 tracks em sequência
    // e sai (ferramenta de debug, não faz parte do jogo).
    if (std::getenv("PREVIEW_MUSIC")) {
        const std::vector<std::pair<const char*, core::MusicTrack>> tracks = {
            {"surface",      game::surfaceTheme()},
            {"shallowCaves", game::shallowCavesTheme()},
            {"fungalWoods",  game::fungalWoodsTheme()},
            {"oldMines",     game::oldMinesTheme()},
            {"moltenHalls",  game::moltenHallsTheme()},
            {"core",         game::coreTheme()},
        };
        for (const auto& [name, t] : tracks) {
            sf::SoundBuffer buf;
            core::synthesizeTrack(t, buf);
            sf::Sound s;
            s.setBuffer(buf);
            s.play();
            std::cout << "Playing " << name << " ("
                      << buf.getDuration().asSeconds() << "s)...\n";
            while (s.getStatus() == sf::Sound::Playing)
                sf::sleep(sf::milliseconds(50));
        }
        return;
    }
    // Preserva os 30 TPS do loop original (Time default é 1/60).
    core::Time::setFixedStep(1.0f / 30.0f);
    // Sprites 1x: janela aberta = contexto GL vivo (nunca em teste).
    if (!spritesBuilt_) {
        sprites_ = sprites::build();
        spritesBuilt_ = true;
        player->meleeTex = &sprites_.playerPunch; // default = soco
    }
    // Música 1x: síntese em RAM (~250ms, ~5MB p/ 6 tracks). Zero arquivo.
    if (!musicBuilt_) {
        music_.registerTrack(0, game::surfaceTheme());
        music_.registerTrack(1, game::shallowCavesTheme());
        music_.registerTrack(2, game::fungalWoodsTheme());
        music_.registerTrack(4, game::oldMinesTheme());
        music_.registerTrack(6, game::moltenHallsTheme());
        music_.registerTrack(9, game::coreTheme());
        // Estratos 3,5,7,8,10: sem track ainda → mantém a atual.
        musicBuilt_ = true;
    }
    // SFX 1x: 26 sons sintetizados (~350KB). Zero arquivo.
    if (!sfxBuilt_) {
        game::buildSoundBank(audio_);
        sfxBuilt_ = true;
    }
    // Luz 1x: ciclo 10 min + radial do player (GL). Sol por tile vem
    // de LightPropagator (grids no generate/break, textura no render).
    dayNight_.setCycleDuration(600.f); // 10 min = 1 dia
    lighting_.init();
    lighting_.setDayNight(&dayNight_);
    // Raio VISUAL (centro à borda): 180 renderiza 360px de largura.
    // Aura difusa que ilumina o caminho (~3.6 tiles por lado).
    lighting_.setPlayerRadius(180.f);
    // Overlay sutil (item 16): o grid carrega a cena, o ADD dá legibilidade.
    // Master 0.4 — sem ele o centro satura e o "ovo" volta.
    lighting_.setMasterIntensity(0.4f);
    // Bloom 1x (item 21): buffers do tamanho da view; GL já tem contexto.
    bloom_.init(static_cast<unsigned>(viewW_), static_cast<unsigned>(viewH_));
    float lastStat = 0.0f;
    int frames = 0;
    int updates = 0;

    while (running && window->isOpen())
    {
        // Ordem: beginFrame ANTES do pollEvent, senão o edge morre
        // antes do primeiro tick ler (e há N ticks por frame).
        pollEvents();
        core::Time::beginFrame();
        int ticks = core::Time::consumeTicks();
        for (int i = 0; i < ticks; i++)
        {
            tick();
            updates++;
        }
        render();
        frames++;

        if (core::Time::elapsed() - lastStat >= 1.0f)
        {
            lastStat = core::Time::elapsed();
            updates = 0;
            frames = 0;
        }

    }
}


void Game::tick() {
    tickCount_++;
    Player *p = player.get();
    p->moveRight = input_.held(support::Action::Right);
    p->moveUp    = input_.held(support::Action::Up);
    p->moveDown  = input_.held(support::Action::Down);
    p->moveLeft  = input_.held(support::Action::Left);
    p->runFast   = input_.held(support::Action::RunFast);
    if (p->moveLeft && !p->moveRight) p->facing = -1;
    if (p->moveRight && !p->moveLeft) p->facing = 1;

    // Run gate: morto/pausado congela movimento, mundo e scheduler.
    // RunManager roda sempre (precisa ver o R).
    // Grid aberto NÃO congela (decisão: sem cursor de teclado, sem
    // freeze — o jogo segue; mouse vem depois).
    // Snapshots p/ SFX de transição (sistemas não veem antes/depois).
    const bool wasGrounded = p->jumping;
    const int hpBefore = p->hp;
    const int phaseBefore = static_cast<int>(p->meleePhase);
    const bool frozen = run_.isPaused() || run_.isDead();
    if (!frozen) {
        // SFX pulo: Up com pé no chão (antes do tick consumir o estado).
        // Na água, silêncio: collide() mantém jumping=true e o som
        // dispararia a 30×/s (metralhadora). Nado com som é fase futura.
        if (p->moveUp && p->jumping && !p->inWater)
            audio_.play(game::keyOf(game::Sfx::PlayerJump));
        p->tick();

        // S6: J (Action::Light) arremessa dinamite.
        // Cooldown cobre o edge por frame: pressed fica alto em todos os
        // ticks do frame, o 2º tick já encontra cooldown rodando.
        // (throwCooldown é tickado no Player::tick, junto dos outros.)
        if (input_.pressed(support::Action::Light) && p->tryThrow(*throws_)) {
            // SFX arremesso + pavio (tryThrow true = saiu da mão).
            audio_.play(game::keyOf(game::Sfx::ThrowDyn));
            audio_.play(game::keyOf(game::Sfx::DynFuse));
        }

        // 1-5: slot ativo da hotbar (fase 4a; sem consumo — edge por frame).
        // Fora quando o grid está aberto (navegação é do grid).
        if (!inventoryUI_.isOpen())
            activeHotbarSlot_ =
                hotbar_.handleInput(input_, activeHotbarSlot_);

        // E: abre/fecha o grid (fase 4b). Consome o edge.
        if (input_.pressed(support::Action::ToggleInventory)) {
            input_.consume(support::Action::ToggleInventory);
            inventoryUI_.toggle();
        }
        // Esc com grid aberto fecha o grid em vez de pausar por baixo.
        // Roda antes do run_.tick (abaixo): o RunManager não vê o edge.
        if (inventoryUI_.isOpen() && input_.pressed(support::Action::Pause)) {
            input_.consume(support::Action::Pause);
            inventoryUI_.close();
        }
        // Com o menu aberto, a UI dirige o input (máquina de estados).
        // Injeta as dependências a cada tick e consome os edges p/ o
        // fixed-step não repetir e o RunManager não ver o R (restart)
        // por baixo do TabRight. Sem freeze: o jogo segue rodando.
        if (inventoryUI_.isOpen()) {
            inventoryUI_.setInventory(&p->inventory);
            inventoryUI_.setEquipment(&p->equipment);
            inventoryUI_.setPlayer(p);
            inventoryUI_.setDrops(drops_);
            inventoryUI_.handleInput(input_);
            using A = support::Action;
            if (input_.pressed(A::TabRight)) input_.consume(A::Restart);
            if (input_.pressed(A::TabLeft)) input_.consume(A::TabLeft);
            if (input_.pressed(A::TabRight)) input_.consume(A::TabRight);
            if (input_.pressed(A::SubTabLeft)) input_.consume(A::SubTabLeft);
            if (input_.pressed(A::SubTabRight))
                input_.consume(A::SubTabRight);
            if (input_.pressed(A::FirstSlot)) input_.consume(A::FirstSlot);
            if (input_.pressed(A::LastSlot)) input_.consume(A::LastSlot);
            if (input_.pressed(A::ArrangeAll)) input_.consume(A::ArrangeAll);
            if (input_.pressed(A::UseItem)) input_.consume(A::UseItem);
            if (input_.pressed(A::Interact)) input_.consume(A::Interact);
        }

        // Mundo infinito: carrega/descarrega chunks em torno do tile do player.
        int playerTileX = static_cast<int>(std::floor(p->getX() / core::kBlockSize));
        int playerTileY = static_cast<int>(std::floor(p->getY() / core::kBlockSize));
        getWorld()->update(playerTileX, playerTileY);

        // Consulta o hash ao redor do player (1 tile de margem).
        // Inclui água de propósito: Player::collide usa WATER para natação.
        std::vector<Entity*> candidatos;
        getWorld()->query(
            p->getX() - core::kBlockSize,
            p->getY() - core::kBlockSize,
            p->getW() + core::kBlockSize * 2,
            p->getH() + core::kBlockSize * 2,
            candidatos);

        for (Entity *e : candidatos) {
            if (p->isColide(*e)) {
                p->collide(*e);
            }
        }
    }

    // Sistemas (inimigos etc.): scheduler com prioridade declarada.
    // BodySystem reconstrói hitboxes pós-movimento (priority 250).
    // Alvos de explosão: montados aqui (Player + Slimes), lidos pelo
    // ExplosionSystem durante o tick do ThrowSystem.
    std::vector<support::ExplosionTarget> targets;
    targets.push_back({sf::Vector2f(p->getCenterX(), p->getCenterY()),
                       &p->body, nullptr, true, p, nullptr});
    enemies_->forEach([&](support::Enemy &s) {
        if (s.resources.isDead()) return;
        targets.push_back({sf::Vector2f(s.body.getCenterX(), s.body.getCenterY()),
                           &s.bodyParts, &s.resources, false,
                           &s.body, &s.knockbackLock});
    });
    support::GameContext ctx{getWorld(), p, &input_, enemies_,
                             throws_, explodes_, drops_, &targets,
                             &screenshots_, &debugFeed_, &audio_, &camera};

    // Sprite atual primeiro: BodySystem (scheduler) deriva hitboxes dele.
    p->currentFrameId = run_.isDead()
        ? support::SpriteFrameId::PlayerDeath
        : game::resolvePlayerSprite(p->jumping, p->getVx(),
                                    p->hurtIframes.running(),
                                    p->inMeleeSwing(),
                                    p->swingAim,
                                    p->throwAnimT > 0.f,
                                    p->walkFrame);
    enemies_->forEach([&](support::Enemy &s) {
        if (auto *d = dynamic_cast<support::DwarfAI *>(s.ai.get())) {
            switch (d->state()) {
                case support::DwarfState::ThrowWindup:
                case support::DwarfState::ThrowRelease:
                    s.currentFrameId = support::SpriteFrameId::DwarfThrow;
                    break;
                case support::DwarfState::Melee:
                    s.currentFrameId = support::SpriteFrameId::DwarfMelee;
                    break;
                default:
                    if (std::fabs(s.body.getVx()) > 0.5f) {
                        s.currentFrameId = ((tickCount_ / 10) % 2 == 0)
                            ? support::SpriteFrameId::DwarfWalkA
                            : support::SpriteFrameId::DwarfWalkB;
                    } else {
                        s.currentFrameId = support::SpriteFrameId::DwarfIdle;
                    }
                    break;
            }
        } else {
            s.currentFrameId = (std::fabs(s.body.getVx()) > 4.5f)
                ? support::SpriteFrameId::SlimeSquash
                : support::SpriteFrameId::SlimeIdle;
        }
    });

    run_.tick(1.0f / 30.0f, ctx);

    // Números de dano congelam no pause (nada flutua/expira parado).
    if (!run_.isPaused()) debugFeed_.tick(1.0f / 30.0f);
    if (!run_.isPaused() && !run_.isDead()) scheduler_.tick(1.0f / 30.0f, ctx);
    // Blast do boom: tick fora do scheduler (visual puro, sem gameplay).
    if (throws_ && !run_.isPaused()) throws_->tickBlasts(1.0f / 30.0f);

    // SFX: libera canais terminados (mesmo pausado: sons <1s terminam).
    audio_.tick();

    // Música: crossfade + troca por estrato (posição real, sobe e desce).
    // Congelado (pause/morte) = música pausada (retoma onde parou).
    if (run_.isPaused() || run_.isDead()) {
        music_.pause();
    } else {
        music_.resume();
        music_.tick(1.0f / 30.0f);
        dayNight_.tick(1.0f / 30.0f); // relógio anda com o jogo (pausa congela)
        if (stratum_) {
            const int s = stratum_->current();
            if (s != lastMusicStratum_) {
                music_.playStratum(s);
                lastMusicStratum_ = s;
            }
        }
    }

    // Luz do player: fonte com raycast ao trocar de tile (~3-4x/s) OU
    // quando o chunk foi relightado (explosão zera o grid; parado, a
    // fonte precisa voltar — senão o subsolo apaga de vez). Render
    // consome o dirty depois do tick: sem loop (re-add → dirty →
    // textura → limpo → pula até mover/relightar de novo).
    if (!frozen) {
        const int ptx = static_cast<int>(player->getX() / core::kBlockSize);
        const int pty = static_cast<int>(player->getY() / core::kBlockSize);
        const support::ChunkCoord cc = support::chunkCoordFromWorld(
            ptx, pty, support::Chunk::W);
        support::Chunk *pc = getWorld()->findChunk(cc.x, cc.y);
        if (pc && (ptx != lastPlayerLightTileX_ ||
                   pty != lastPlayerLightTileY_ || pc->lightDirty)) {
            lastPlayerLightTileX_ = ptx;
            lastPlayerLightTileY_ = pty;
            // Cross-chunk: cast único + push outward (costura sem corte).
            // Nível 13: ~13 tiles com decaimento 1/tile.
            support::LightPropagator::addBlockSourceAt(
                [this](int cx, int cy) { return getWorld()->findChunk(cx, cy); },
                player->getX(), player->getY(), 13);
        }
    }

    // SFX de transição do player (poll fim-do-tick; cobre TODAS as fontes
    // de dano/movimento num lugar só, sem ctx nos métodos do Player).
    // Screen shake (item 23) pega carona aqui: 0.3 no dano, 0.8 na morte.
    camera.tickTrauma(1.f / 30.f);
    if (!frozen) {
        if (!wasGrounded && p->jumping)
            audio_.play(game::keyOf(game::Sfx::PlayerLand));
        if (phaseBefore == 0 && static_cast<int>(p->meleePhase) == 1)
            audio_.play(game::keyOf(game::Sfx::MeleeSwing));
        if (p->hp < hpBefore) {
            if (p->hp <= 0) {
                audio_.play(game::keyOf(game::Sfx::PlayerDeath));
                camera.addTrauma(0.8f);
            } else {
                audio_.play(game::keyOf(game::Sfx::PlayerHurt));
                camera.addTrauma(0.3f);
            }
        }
    }
}
