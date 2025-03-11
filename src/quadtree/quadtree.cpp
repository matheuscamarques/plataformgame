#include "./quadtree.h"
#include <iostream>
#include <sstream>
#include "../defines.h"
using namespace std;

Quadtree::Quadtree(float _x, float _y, float _width, float _height, int _level, int _maxLevel) :
        Entity(QUADTREE,_x, _y, _width, _height),
        level	(_level),
        maxLevel(_maxLevel)
{
    shape.setPosition(this->getX(), this->getY());
    shape.setSize(sf::Vector2f(this->getW(), this->getH()));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(sf::Color(64, 128, 255));
    text.setPosition(this->getX(), this->getY() + level * 16);
    text.setCharacterSize(12);

    if (level == maxLevel) {
        return;
    }

    NW = new Quadtree(this->getX(), this->getY(), getW() / 2.0f, getH() / 2.0f, level+1, maxLevel);
    NE = new Quadtree(this->getX() + getW() / 2.0f, this->getY(), getW() / 2.0f, getH() / 2.0f, level+1, maxLevel);
    SW = new Quadtree(this->getX(), this->getY() + getH() / 2.0f, getW() / 2.0f, getH() / 2.0f, level+1, maxLevel);
    SE = new Quadtree(this->getX() + getW() / 2.0f, this->getY() + getH() / 2.0f, getW() / 2.0f, getH() / 2.0f, level+1, maxLevel);
}

Quadtree::~Quadtree()
{
    if (level == maxLevel)
        return;

    delete NW;
    delete NE;
    delete SW;
    delete SE;
}

void Quadtree::AddObject(Entity *object) {
    if (level == maxLevel) {
        objects.push_back(object);
        return;
    }
    if (contains(NW, object)) {
        NW->AddObject(object); return;
    } else if (contains(NE, object)) {
        NE->AddObject(object); return;
    } else if (contains(SW, object)) {
        SW->AddObject(object); return;
    } else if (contains(SE, object)) {
        SE->AddObject(object); return;
    }
    if (contains(this, object)) {
        objects.push_back(object);
    }
}

vector<Entity*> Quadtree::GetObjectsAt(float _x, float _y) {
    if (level == maxLevel) {
        return objects;
    }

    vector<Entity*> returnObjects, childReturnObjects;
    if (!objects.empty()) {
        returnObjects = objects;
    }
    if (_x > getX() + getW() / 2.0f && _x < getX() + getW()) {
        if (_y > getY() + getH() / 2.0f && _y < getY() + getH()) {
            childReturnObjects = SE->GetObjectsAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        } else if (_y > getY() && _y <= getY() + getH() / 2.0f) {
            childReturnObjects = NE->GetObjectsAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        }
    } else if (_x > getX() && _x <= getX() + getW() / 2.0f) {
        if (_y > getY() + getH() / 2.0f && _y < getY() + getH()) {
            childReturnObjects = SW->GetObjectsAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        } else if (_y > getY() && _y <= getY() + getH() / 2.0f) {
            childReturnObjects = NW->GetObjectsAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        }
    }
    return returnObjects;
}

void Quadtree::Clear() {
    if (level == maxLevel) {
        objects.clear();
        return;
    } else {
        NW->Clear();
        NE->Clear();
        SW->Clear();
        SE->Clear();
    }
    if (!objects.empty()) {
        objects.clear();
    }
}

void Quadtree::SetFont(const sf::Font &font) {
    text.setFont(font);
    if (level != maxLevel) {
        NW->SetFont(font);
        NE->SetFont(font);
        SW->SetFont(font);
        SE->SetFont(font);
    }
}

void Quadtree::Draw(sf::RenderWindow *canvas) {
    stringstream ss;
    ss << objects.size();
    string numObjectsStr = ss.str();
    text.setString(numObjectsStr);
    canvas->draw(shape);
    canvas->draw(text);
    if (level != maxLevel) {
        NW->Draw(canvas);
        NE->Draw(canvas);
        SW->Draw(canvas);
        SE->Draw(canvas);
    }
}

bool Quadtree::contains(Quadtree *child, Entity *object) {
    return (
        child->getX() <= object->getX() &&
        child->getY() <= object->getY() &&
        child->getX() + child->getW() >= object->getX() + object->getW() &&
        child->getY() + child->getH() >= object->getY() + object->getH()
    );
}