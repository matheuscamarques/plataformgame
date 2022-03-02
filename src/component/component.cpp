//
// Created by sanonichan on 01/03/2022.
//

#include "component.h"

Component::Component() : sf::RectangleShape(), sf::FloatRect()
{
}

Component::Component(float x, float y, float w, float h) :
sf::RectangleShape(sf::Vector2f(w, h)),
sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h)) {
    setPosition(x, y);
    left = x;
    top  = y;
    
}

Component::Component(sf::Vector2f pos, sf::Vector2f size) :
sf::RectangleShape(size) ,
sf::FloatRect(pos, size)
{
    setPosition(pos);
    left = x;
    top  = y;
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
    return this->w;
}

float Component::getW() {
    return this->w;
}

float Component::getH() {
   return this->h;
}


