/**
 * @file src/component/component.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa retângulo nomeado base com posição e tamanho.
 * @details Define construtores e getters e setters de x, y, w, h e bordas de Component herdando RectangleShape e FloatRect, usado por Entity e tiles.
 */

//
// Created by sanonichan on 01/03/2022.
//

#include "component.h"
#include "core/VecSfml.h"

Component::Component() : sf::RectangleShape(), sf::FloatRect()
{
}

Component::Component(int name,float x, float y, float w, float h) :
sf::RectangleShape(core::toSf(core::Vec2f(w, h))),
sf::FloatRect(core::toSf(core::Vec2f(x, y)), core::toSf(core::Vec2f(w, h))) {
    setPosition(x, y);
    left = x;
    top  = y;
    this->name = name;
    
}

Component::Component(int name, core::Vec2f pos, core::Vec2f size) :
sf::RectangleShape(core::toSf(size)) ,
sf::FloatRect(core::toSf(pos), core::toSf(size))
{
    setPosition(core::toSf(pos));
    left = pos.x;
    top  = pos.y;
    this->name = name;
}

void Component::setX(float x) {
    this->x = x;
}

void Component::setY(float y) {
    this->y = y;
}

void Component::setH(float h) {
    this->h = h;
}

void Component::setW(float w) {
    this->w = w;
}

float Component::getX() {
    return this->x;
}

float Component::getY() {
    return this->y;
}

float Component::getW() {
    return this->w;
}

float Component::getH() {
   return this->h;
}

float Component::getRight() const {
    return left + w;
}

float Component::getBottom() const {
    return top + h;
}

core::Vec2f Component::getTopLeft() const {
    return {left, top};
}
core::Vec2f Component::getCenter() const {
    return {left + w/2, top + h/2};
}

int Component::getName() {
    return this->name;
}



