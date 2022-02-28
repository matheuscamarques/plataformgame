#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "../level/Level/level.h"
#include "vector"
class Game
{
public:
    Game();
    static void main();
    void setWindow(sf::RenderWindow *window);
    void start();
    void run();
    Level* getLevel();
    void setLevel(Level *level);
    

private:
    sf::RenderWindow *window;
    Level * level;
    bool running = false;
    void render();
    void tick();

    
};