#include "./window.h"

Window::Window(int width, int height,const char *title, Game *game)
{
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title);
    window->setVerticalSyncEnabled(true);
    // Sem repeat do SO: segurar E/F/Esc gerava KeyPressed a cada ~30ms e
    // cada repeat re-armava o edge após o consume() (menu abria e fechava
    // sozinho). Edge aqui = 1 por aperto físico; held() segue por nível.
    window->setKeyRepeatEnabled(false);

    game->setWindow(window.get());
    game->start();
    game->run();
}
