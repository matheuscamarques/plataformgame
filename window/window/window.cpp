#include "./window.h"

Window::Window(int width, int height,const char *title, Game *game)
{

    sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML works!");
    game->setWindow(&window);
    game->start();
    game->run();
}
