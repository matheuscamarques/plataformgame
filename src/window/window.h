#pragma once

#include <SFML/Graphics.hpp>
#include "../game/game.h"
#include <SFML/OpenGL.hpp>
#include <memory>
class Window
{
public:
    Window(int, int,const char *, Game *);
private:
    std::unique_ptr<sf::RenderWindow> window;
};