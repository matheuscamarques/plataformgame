#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include "vector"
#include "core/System.h"
#include "../support/Camera/Camera.h"
#include "../support/Debug/DebugOverlay.h"
#include "../support/Input/InputMap.h"
#include "support/Progression/RunManager.h"
#include "assets/Sprites/SpriteSet.h"

// Só ponteiros no header: definição completa mora no .cpp de cada
// sistema (game.cpp inclui). Valor continua incluído (precisa do tipo).
class Player;

namespace support {
class ContactDamageSystem;
class DeathSystem;
class DropSystem;
class EnemySystem;
class ExplosionSystem;
class MeleeSystem;
class SpawnSystem;
class StratumManager;
class ParticleSystem;
class ThrowSystem;
class World;
} // namespace support

class Game
{
public:
    Game();
    static void main();
    void setWindow(sf::RenderWindow *window);
    void start();
    void run();
    support::World* getWorld();
    void setWorld(std::unique_ptr<support::World> world);
    Player* getPlayer();
    

private:
    sf::RenderWindow *window; // emprestado: dono é Window
    std::unique_ptr<support::World> world; // dono
    std::unique_ptr<Player> player; // dono
    support::Camera camera; // valor
    support::InputMap input_; // valor
    support::DebugOverlay overlay_; // valor
    core::SystemScheduler scheduler_; // dono dos Systems
    support::EnemySystem *enemies_ = nullptr; // observa; dono é o scheduler
    support::ThrowSystem *throws_ = nullptr; // observa; dono é o scheduler
    support::ExplosionSystem *explodes_ = nullptr; // observa; dono é o scheduler
    support::DeathSystem *deaths_ = nullptr; // observa; dono é o scheduler
    support::MeleeSystem *melee_ = nullptr; // observa; dono é o scheduler
    support::StratumManager *stratum_ = nullptr; // observa; dono é o scheduler
    support::RunManager run_; // valor: gate do tick (morte/pause)
    support::DropSystem *drops_ = nullptr; // observa; dono é o scheduler
    support::ParticleSystem *particles_ = nullptr; // observa; dono é o scheduler
    sprites::SpriteSet sprites_; // dono: build 1x no run (precisa de GL)
    bool spritesBuilt_ = false;
    bool charView_ = false; // F3: ASCII por char, sem textura
    int tickCount_ = 0; // p/ animação walk do anão
    bool running = false;
    void render();
    void tick();
    void drawPlayerSprite();
    void drawPlayerEquipment();
    void drawPlayerWeapon();
    void drawEnemiesSprites();

    // Tamanho da viewport. Futuro: AssetManager é dono de font;
    // Game só pede por chave. Não criar AssetManager no B5.
    float viewW_ = 0.0f;
    float viewH_ = 0.0f;
    sf::Font font;
};