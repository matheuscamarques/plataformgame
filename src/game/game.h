#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "vector"
#include "../level/Level/level.h"
#include "../camera/Camera.h"

class Game : public Component
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
    //sf::View *view;
    bool running = false;
    void render();
    void tick();
    int totalQuadtreeSee = 0;

    sf::Font *font;
};