#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include "vector"
#include "../support/World/World.h"
#include "../entities/player/player.h"
#include "../support/Camera/Camera.h"
#include "../support/Input/InputMap.h"

class Game : public Component
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
    sf::RenderWindow *window;
    std::unique_ptr<support::World> world;
    std::unique_ptr<Player> player;
    support::Camera camera;
    support::InputMap input_;
    //sf::View *view;
    bool running = false;
    void render();
    void tick();
    int totalCandidatesSeen = 0;

    sf::Font font;
};