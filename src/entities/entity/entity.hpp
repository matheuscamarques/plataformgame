#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <map>
#include "../../component/component.h"

class Entity : public Component
{
    public:
        Entity(const char* name,float x, float y, float w, float h);

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

        const char* getName();
    private:
        float x;
        float y;
        float w;
        float vx;
        float vy;
        float h;
        float gravity;
        const char* name = "";
        std::map<std::string,Component> bounds;
}; 