#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include "vector"
#include "../support/World/World.h"
#include "../entities/player/player.h"
#include "../camera/Camera.h"

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
    Camera camera{0.0f, 0.0f};
    //sf::View *view;
    bool running = false;
    void render();
    void tick();
    int totalCandidatesSeen = 0;

    sf::Font font;
};