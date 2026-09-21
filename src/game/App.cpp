#include "game.h"

#include <cmath>
#include <vector>

#include "core/Config.h"
#include "core/Material.h"
#include "core/Time.h"
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "world/World.h"
#include "assets/PlayerSprite.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/Combat/SpriteFrame.h"
#include "support/Enemies/DwarfAI.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

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
    // Preserva os 30 TPS do loop original (Time default é 1/60).
    core::Time::setFixedStep(1.0f / 30.0f);
    // Sprites 1x: janela aberta = contexto GL vivo (nunca em teste).
    if (!spritesBuilt_) {
        sprites_ = sprites::build();
        spritesBuilt_ = true;
        player->meleeTex = &sprites_.playerPunch; // default = soco
    }
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
    const bool frozen = run_.isPaused() || run_.isDead();
    if (!frozen) {
        p->tick();

        // S6: J (Action::Light) arremessa dinamite.
        // Cooldown cobre o edge por frame: pressed fica alto em todos os
        // ticks do frame, o 2º tick já encontra cooldown rodando.
        // (throwCooldown é tickado no Player::tick, junto dos outros.)
        if (input_.pressed(support::Action::Light)) p->tryThrow(*throws_);

        // M: cicla material do set inteiro (debug visual).
        if (input_.pressed(support::Action::CycleMaterial)) {
            input_.consume(support::Action::CycleMaterial);
            int m = static_cast<int>(p->loadout.weapon);
            m = (m + 1) % static_cast<int>(core::MaterialId::COUNT);
            p->loadout.weapon = static_cast<core::MaterialId>(m);
            p->loadout.helm = p->loadout.weapon;
            p->loadout.chest = p->loadout.weapon;
            p->loadout.legs = p->loadout.weapon;
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
                             throws_, explodes_, drops_, &targets};

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
    if (!run_.isPaused() && !run_.isDead()) scheduler_.tick(1.0f / 30.0f, ctx);
}
