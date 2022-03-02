//
// Created by sanonichan on 01/03/2022.
//

#ifndef PLATOFORMGAME_COMPONENT_H
#define PLATOFORMGAME_COMPONENT_H
#include <SFML/Graphics.hpp>

class Component : public sf::RectangleShape , public sf::FloatRect {
public:
    Component(float x, float y, float w, float h);
    Component(sf::Vector2f pos, sf::Vector2f size);
    Component();
};


#endif //PLATOFORMGAME_COMPONENT_H
