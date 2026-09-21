
#include "Entity.hpp"
#include "defines.h"


Entity::Entity(int name,float x, float y, float w, float h) : Component(name,sf::Vector2f(x, y), sf::Vector2f(w, h)){
    this->name = name;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    // Sincroniza o FloatRect base: plataformas estáticas nunca recebem
    // tick(), então left/top precisam nascer corretos para intersects().
    this->left = x;
    this->top = y;
    this->vx = 0;
    this->vy = 0;
    this->gravity = 0.0;
    this->bounds = std::map<std::string, Component>();

    this->bounds["bottom"] = Component(core::kIdBound,x + (w / 3) - ((w / 3) / 2), y + (h / 2), w / 3, h / 2);
    this->bounds["bottom"].setFillColor(sf::Color::Transparent);
    this->bounds["bottom"].setOutlineColor(sf::Color::Green);
    this->bounds["bottom"].setOutlineThickness(1);

    this->bounds["top"] = Component(core::kIdBound,x + (w / 2) - ((w / 2) / 2), y, w / 3, h / 2);
    this->bounds["top"].setFillColor(sf::Color::Transparent);
    this->bounds["top"].setOutlineColor(sf::Color::Green);
    this->bounds["top"].setOutlineThickness(1);

    // Sensores laterais cobrem só a banda média [0.3h, 0.7h]: andar sobre
    // o chão afunda o player ~vy por tick, e sensor alto demais encostava
    // no tile vizinho do mesmo nível (parede invisível nas emendas).
    this->bounds["left"] = Component(core::kIdBound,x,y+h*0.3f,w*0.2f,h*0.4f);
    this->bounds["left"].setFillColor(sf::Color::Transparent);
    this->bounds["left"].setOutlineColor(sf::Color::Green);
    this->bounds["left"].setOutlineThickness(1);

    this->bounds["right"] = Component(core::kIdBound,x,y+h*0.3f,w*0.20f,h*0.4f);
    this->bounds["right"].setFillColor(sf::Color::Transparent);
    this->bounds["right"].setOutlineColor(sf::Color::Green);
    this->bounds["right"].setOutlineThickness(1);

    this->setPosition(x, y);
}



void Entity::tick()
{
    setX(getX() + getVx());
    setY(getY() + getVy());
    this->left = getX();
    this->top  = getY();
    this->setPosition(getX(), getY());
}

void Entity::draw(sf::RenderWindow *window)
{
    window->draw(*this);
    // window->draw(this->getBoundsBottom());
    // window->draw(this->getBoundsTop());
    //window->draw(this->getBoundsLeft());
    //window->draw(this->getBoundsRight());
}

float Entity::getX()
{
    return this->x;
}
float Entity::getY()
{
    return this->y;
}
float Entity::getW()
{
    return this->w;
}
float Entity::getH()
{
    return this->h;
}
float Entity::getGravity()
{
    return this->gravity;
}

const int Entity::getName()
{
    return name;
}

void Entity::setX(float x)
{
    this->x = x;
    this->left = x;
    setPosition(x, getY());
}
void Entity::setY(float y)
{
    this->y = y;
    this->top = y;
    setPosition(getX(), y);
}
void Entity::setW(float w)
{
    this->w = w;
}
void Entity::setH(float h)
{
    this->h = h;
}
void Entity::setGravity(float g)
{
    this->gravity = g;
}

Component Entity::getBoundsBottom()
{
    this->bounds["bottom"].setPosition(this->x + (this->w / 2) - ((this->w / 2) / 3), this->y + (this->h / 2));
    this->bounds["bottom"].top = this->y + (this->h / 2);
    this->bounds["bottom"].left = this->x + (this->w / 2) - ((this->w / 2) / 3);
    return this->bounds["bottom"];
}
Component Entity::getBoundsTop()
{
    this->bounds["top"].setPosition(this->x + (this->w / 2) - ((this->w / 2) / 3), this->y);
    this->bounds["top"].top = this->y;
    this->bounds["top"].left = this->x + (this->w / 2) - ((this->w / 2) / 3);
    return this->bounds["top"];
}

Component Entity::getBoundsLeft()
{
    this->bounds["left"].setPosition(x,y+h*0.3f);
    this->bounds["left"].top = this->y + h*0.3f;
    this->bounds["left"].left = this->x;
    return this->bounds["left"];
}

Component Entity::getBoundsRight()
{
    this->bounds["right"].setPosition(x+w-w*0.2f,y+h*0.3f);
    this->bounds["right"].top = y+h*0.3f;
    this->bounds["right"].left = x+w-w*0.2f;
    return this->bounds["right"];
}


float Entity::getVx()
{
    return this->vx;
}

float Entity::getVy()
{
    return this->vy;
}

void Entity::setVx(float xx)
{
    this->vx = xx;
}

void Entity::setVy(float vy)
{
    this->vy = vy ;
}

std::map<std::string, Component> Entity::getBounds() {
    return this->bounds;
}

bool Entity::isColide(Entity entity) {
    if (getBoundsTop().intersects(entity)) {
       return true;
    }
    if (getBoundsBottom().intersects(entity)) {
       return true;
    }
    if (getBoundsRight().intersects(entity)) {
        return true;
    }
    if (getBoundsLeft().intersects(entity)) {
        return true;
    }
    return false;
}

float Entity::getCenterX() {
    return this->x + (this->w / 2);
}

float Entity::getCenterY(){
    return this->y + (this->h / 2);
}