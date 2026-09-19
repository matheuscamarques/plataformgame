#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include "vector"
#include "../core/System.h"
#include "../support/World/World.h"
#include "../entities/player/player.h"
#include "../support/Camera/Camera.h"
#include "../support/Debug/DebugOverlay.h"
#include "../support/DeathSystem.h"
#include "../support/DropSystem.h"
#include "../support/EnemySystem.h"
#include "../support/ExplosionSystem.h"
#include "../support/Input/InputMap.h"
#include "../support/ParticleSystem.h"
#include "../support/ThrowSystem.h"

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
    support::DropSystem *drops_ = nullptr; // observa; dono é o scheduler
    support::ParticleSystem *particles_ = nullptr; // observa; dono é o scheduler
    bool running = false;
    void render();
    void tick();

    // Tamanho da viewport. Futuro: AssetManager é dono de font;
    // Game só pede por chave. Não criar AssetManager no B5.
    float viewW_ = 0.0f;
    float viewH_ = 0.0f;
    sf::Font font;
};