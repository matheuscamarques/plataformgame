/**
 * @file src/window/window.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara wrapper fino da janela SFML do jogo.
 * @details Define classe Window com construtor de largura, altura e título mais RenderWindow própria, incluída pelo main e ligada ao Game via setWindow.
 */

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