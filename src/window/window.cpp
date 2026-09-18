#include "./window.h"

Window::Window(int width, int height,const char *title, Game *game)
{
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title);
    window->setVerticalSyncEnabled(true);

    game->setWindow(window.get());
    game->start();
    game->run();
}
