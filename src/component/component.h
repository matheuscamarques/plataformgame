/**
 * @file src/component/component.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara retângulo SFML nomeado base para colisão e desenho.
 * @details Define classe Component com herança de RectangleShape e FloatRect mais acesso a posição, tamanho e nome, incluída por Entity e sistemas de colisão.
 */

//
// Created by sanonichan on 01/03/2022.
//

#ifndef PLATOFORMGAME_COMPONENT_H
#define PLATOFORMGAME_COMPONENT_H
#include <SFML/Graphics.hpp>

#include "core/Vec.h"
#include "core/VecSfml.h"

class Component : public sf::RectangleShape , public sf::FloatRect {
public:
    Component(const int name,float x, float y, float w, float h);
    Component(const int name, core::Vec2f pos, core::Vec2f size);
    Component();
    void setX(float x);
    void setY(float y);
    void setW(float w);
    void setH(float h);
    float getX() const;
    float getY() const;
    float getW() const;
    float getH() const;

    float getRight() const;
    float getBottom() const;
    core::Vec2f getTopLeft() const;
    core::Vec2f getCenter() const;

    int getName() const;

private:
    int name;
    float x;
    float y;
    float w;
    float h;
};


#endif //PLATOFORMGAME_COMPONENT_H
