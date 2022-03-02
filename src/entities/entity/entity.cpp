
#include "./entity.hpp"

Entity::Entity(float x, float y, float w, float h) : Component(sf::Vector2f(x, y), sf::Vector2f(w, h))
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->vx = 0;
    this->vy = 0;
    this->gravity = 0.5;
    this->bounds = std::map<std::string, Component>();

    this->bounds["bottom"] = Component(x + (w / 2) - ((w / 2) / 2), y + (h / 2), w / 2, h / 2);
    this->bounds["bottom"].setFillColor(sf::Color::Transparent);
    this->bounds["bottom"].setOutlineColor(sf::Color::Green);
    this->bounds["bottom"].setOutlineThickness(1);

    this->bounds["top"] = Component(x + (w / 2) - ((w / 2) / 2), y, w / 2, h / 2);
    this->bounds["top"].setFillColor(sf::Color::Transparent);
    this->bounds["top"].setOutlineColor(sf::Color::Green);
    this->bounds["top"].setOutlineThickness(1);

    this->bounds["left"] = Component(x, y + 5, 5, h - 15);
    this->bounds["left"].setFillColor(sf::Color::Transparent);
    this->bounds["left"].setOutlineColor(sf::Color::Green);
    this->bounds["left"].setOutlineThickness(1);

    this->bounds["right"] = Component(x + w - 5, y + 5, 5, h - 15);
    this->bounds["right"].setFillColor(sf::Color::Transparent);
    this->bounds["right"].setOutlineColor(sf::Color::Green);
    this->bounds["right"].setOutlineThickness(1);

    this->setPosition(x, y);
}

void Entity::tick()
{
    setX(this->x + this->vx);
    setY(this->y + this->vy);
    this->setPosition(this->x, this->y);
}

void Entity::draw(sf::RenderWindow *window)
{
    window->draw(*this);
    for (auto &b : this->bounds) window->draw(b.second);
    
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

void Entity::setX(float x)
{
    this->x = x;
}
void Entity::setY(float y)
{
    this->y = y;
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
    this->bounds["bottom"].left = this->x + (this->w / 2) - ((this->w / 2) / 2);
    this->bounds["bottom"].top = this->y + (this->h / 2);
    return this->bounds["bottom"];
}
Component Entity::getBoundsTop()
{
    this->bounds["top"].left = this->x + (this->w / 2) - ((this->w / 2) / 2);
    this->bounds["top"].top = this->y;
    return this->bounds["top"];
}

Component Entity::getBoundsLeft()
{
    this->bounds["left"].left = this->x;
    this->bounds["left"].top = this->y + 5;
    return this->bounds["left"];
}

Component Entity::getBoundsRight()
{
    this->bounds["right"].left = this->x + this->w - 5;
    this->bounds["right"].top = this->y + 5;
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
    this->vy = vy;
}
