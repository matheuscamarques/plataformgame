#include "./window.h"

Window::Window(int width, int height,const char *title, Game *game)
{
    auto window = new sf::RenderWindow(sf::VideoMode(width, height), title);
    window->setVerticalSyncEnabled(true);

    game->setWindow(window);
    game->start();
    game->run();
}
