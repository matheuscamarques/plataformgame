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
