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
#include "assets/PlayerSprite.h"
#include "game/MusicBank.h"
#include "game/SoundBank.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/Combat/SpriteFrame.h"
#include "support/Enemies/DwarfAI.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/GameContext.h"
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
    // Luz 1x: ciclo 10 min + lightmap do tamanho da view (precisa de GL).
    dayNight_.setCycleDuration(600.f); // 10 min = 1 dia
    lighting_.init(static_cast<unsigned>(viewW_), static_cast<unsigned>(viewH_));
    lighting_.setSurfaceSampler([this](float worldX) {
        return getWorld()->surfaceYAt(worldX);
    });
    lighting_.setDayNight(&dayNight_);
    lighting_.setSunFadeDepth(300.f);
    lighting_.setPlayerRadius(90.f);
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

        // M: cicla material do set inteiro (debug visual).
        if (input_.pressed(support::Action::CycleMaterial)) {
            input_.consume(support::Action::CycleMaterial);
            p->cycleMaterial();
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
                             &screenshots_, &debugFeed_, &audio_};

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

    // SFX de transição do player (poll fim-do-tick; cobre TODAS as fontes
    // de dano/movimento num lugar só, sem ctx nos métodos do Player).
    if (!frozen) {
        if (!wasGrounded && p->jumping)
            audio_.play(game::keyOf(game::Sfx::PlayerLand));
        if (phaseBefore == 0 && static_cast<int>(p->meleePhase) == 1)
            audio_.play(game::keyOf(game::Sfx::MeleeSwing));
        if (p->hp < hpBefore) {
            if (p->hp <= 0)
                audio_.play(game::keyOf(game::Sfx::PlayerDeath));
            else
                audio_.play(game::keyOf(game::Sfx::PlayerHurt));
        }
    }
}
