#include "game.h"

#include "entities/Player/Player.h"
#include "world/World.h"

// Métodos pequenos; o resto mora em App/Renderer/Input/Bootstrapper.

void Game::setWindow(sf::RenderWindow *window)
{
     viewW_ = static_cast<float>(window->getSize().x);
     viewH_ = static_cast<float>(window->getSize().y);

    //window->setView(*this->view);
    this->window = window;
}


support::World* Game::getWorld() {
    return this->world.get();
}

void Game::setWorld(std::unique_ptr<support::World> world) {
    this->world = std::move(world);
}

Player* Game::getPlayer() {
    return this->player.get();
}
