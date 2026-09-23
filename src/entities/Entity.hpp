/**
 * @file src/entities/Entity.hpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara entidade jogável com física e caixas de colisão.
 * @details Define classe Entity herdando Component com tick, draw, velocidade, gravidade e mapa de bounds, incluída por Player, App e Renderer.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <map>
#include "component/component.h"

class Entity : public Component
{
    public:
        Entity(int name,float x, float y, float w, float h);

        void tick();
        void draw(sf::RenderWindow *window);
        
        //bool  isColide(Entity &entity);
        float getX();
        float getY();
        float getW();
        float getH();
        float getVx();
        float getVy();
        float getGravity();

        void setX(float x);
        void setY(float y);
        void setVx(float xx);
        void setVy(float vy);
        void setW(float w);
        void setH(float h);
        void setGravity(float g);

        Component getBoundsBottom();
        Component getBoundsTop();
        Component getBoundsLeft();
        Component getBoundsRight();
        std::map<std::string,Component> getBounds();

        const int getName();

        bool isColide(Entity entity);
        float getCenterX();
        float getCenterY();
    float x;
    float y;
    int facing = 1; // -1 esquerda, +1 direita (BodySystem/render usam)
private:
    float w;
        float vx;
        float vy;
        float h;
        float gravity;
        int name = -1;
        std::map<std::string,Component> bounds;
}; 