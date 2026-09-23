/**
 * @file src/game/Bootstrapper.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Monta mundo, jogador, janela e sistemas no início.
 * @details Implementa Game main que cria World, Player, scheduler com combate, inimigos, efeitos e progressão, chamado uma vez pelo entrypoint via Window.
 */

#include "game.h"

#include <stdexcept>

#include "core/Config.h"
#include "core/Log.h"
#include "entities/Player/Player.h"
#include "world/Generation.h"
#include "world/World.h"
#include "../window/window.h"
#include "support/Combat/ContactDamageSystem.h"
#include "support/Combat/DeathSystem.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/Combat/MeleeSystem.h"
#include "support/Effects/ParticleSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Combat/BodySystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Enemies/SpawnSystem.h"
#include "support/Progression/DropSystem.h"
#include "support/Progression/StratumManager.h"

// Bootstrapper: monta o Game (mundo, player, scheduler, janela).

void Game::main()
{
    auto game = std::make_unique<Game>();
    //game->view = new sf::View(sf::FloatRect(0.f, 0.f, 1000.f, 600.f));
    auto world = std::make_unique<support::World>(core::kWorldSeed);
    game->player = std::make_unique<Player>();
    // Spawn no flanco de montanha mais próximo (terra garantida):
    // cair do céu no meio do nada não mostra o jogo.
    int spawnTx = support::findSpawnTileX(0, core::kWorldSeed);
    game->player->setX(spawnTx * core::kBlockSize);
    game->setWorld(std::move(world));

    if (!game->font.loadFromFile("./arial.ttf"))
    {
        throw std::runtime_error("Could not load font");
    }
    LOG_INFO("Game", "boot ok, seed=" << core::kWorldSeed << " spawnTx=" << spawnTx);

    // Sistemas via scheduler; 2 slimes perto do spawn (determinístico).
    game->stratum_ = &game->scheduler_.add<support::StratumManager>();
    game->run_.setStratumManager(game->stratum_);
    game->enemies_ = &game->scheduler_.add<support::EnemySystem>();
    game->scheduler_.add<support::BodySystem>();
    game->particles_ = &game->scheduler_.add<support::ParticleSystem>();
    game->throws_ = &game->scheduler_.add<support::ThrowSystem>();
    game->explodes_ = &game->scheduler_.add<support::ExplosionSystem>();
    game->deaths_ = &game->scheduler_.add<support::DeathSystem>();
    game->melee_ = &game->scheduler_.add<support::MeleeSystem>();
    game->scheduler_.add<support::ContactDamageSystem>();
    game->drops_ = &game->scheduler_.add<support::DropSystem>();
    game->scheduler_.add<support::SpawnSystem>(); // spawn contínuo (sem wiring)
    game->throws_->setExplosionSystem(game->explodes_);
    game->throws_->setParticleSystem(game->particles_);
    game->explodes_->setParticleSystem(game->particles_);
    game->explodes_->setDropSystem(game->drops_); // drops de bloco (fase 2)
    game->deaths_->setDropSystem(game->drops_);
    game->deaths_->setParticleSystem(game->particles_);
    game->melee_->setParticleSystem(game->particles_);
    // game->enemies_->spawn("slime", (spawnTx - 6) * core::kBlockSize, 0.0f);
    // game->enemies_->spawn("slime", (spawnTx + 6) * core::kBlockSize, 0.0f);

    // add border font

    Window window(800, 800, "Game", game.get());
}
