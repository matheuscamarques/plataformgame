/**
 * @file src/game/game.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa acessores pequenos e ciclo de vida do Game.
 * @details Define destrutor, setWindow, get e set de World e Player e toggle de chunks assíncronos, complementa App, Renderer, Input e Bootstrapper via game.h.
 */

#include "game.h"

#include "entities/Player/Player.h"
#include "world/ChunkLoader.h"
#include "world/World.h"

// Métodos pequenos; o resto mora em App/Renderer/Input/Bootstrapper.

Game::~Game() = default; // chunkLoader_ precisa do tipo completo aqui

void Game::setWindow(sf::RenderWindow *window)
{
     viewW_ = static_cast<float>(window->getSize().x);
     viewH_ = static_cast<float>(window->getSize().y);

    //window->setView(*this->view);
    this->window = window;
    screenshots_.setWindow(window);
    // Backend persistente: texturas dos handles vivem aqui. Recria com
    // a janela e invalida handles antigos (ids do backend morto).
    enemyBackend_ = std::make_unique<render::Render2D>(*window);
    backendHandles_.clear();
}


support::World* Game::getWorld() {
    return this->world.get();
}

void Game::setWorld(std::unique_ptr<support::World> world) {
    this->world = std::move(world);
}

void Game::setAsyncChunks(bool on) {
    if (!on) {
        if (world) world->setChunkLoader(nullptr);
        chunkLoader_.reset();
        return;
    }
    if (!world) return;
    support::World* w = world.get();
    chunkLoader_ = std::make_unique<support::ChunkLoader>(
        [w](int cx, int cy) { return w->buildBareChunk(cx, cy); });
    chunkLoader_->start();
    world->setChunkLoader(chunkLoader_.get());
}

Player* Game::getPlayer() {
    return this->player.get();
}
