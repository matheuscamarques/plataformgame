#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <map>

 class Entity : public sf::RectangleShape
{
    public:
        Entity(float x, float y, float w, float h);

        void update();
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
        
        // sf::FloatRect getBoundsBottom();
        // sf::FloatRect getBoundsTop();
        // sf::FloatRect getBoundsLeft();
        // sf::FloatRect getBoundsRight();

    private:
        float x;
        float y;
        float w;
        float vx;
        float vy;
        float h;
        float gravity;
        std::map<std::string,sf::FloatRect> bounds;

    private:



}; 