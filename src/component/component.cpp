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
}

Component::Component(sf::Vector2f pos, sf::Vector2f size) :
sf::RectangleShape(size) ,
sf::FloatRect(pos, size)
{
    setPosition(pos);
}