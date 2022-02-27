#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class Game
{
public:
    Game();
    static void main();
    void setWindow(sf::RenderWindow *window);
    void start();
    void run();

private:
    sf::RenderWindow *window;
    bool running = false;
    //sf::Thread m_thread;
    void render();
    void tick();
};