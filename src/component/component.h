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
    void setX(float x);
    void setY(float y);
    void setW(float w);
    void setH(float h);
    float getX();
    float getY();
    float getW();
    float getH();

private:
    float x;
    float y;
    float w;
    float h;
};


#endif //PLATOFORMGAME_COMPONENT_H
