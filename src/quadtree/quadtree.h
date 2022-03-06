//
// Created by @sanonichan on 05/03/2022.
//
#ifndef PLATOFORMGAME_QUADTREE_H
#define PLATOFORMGAME_QUADTREE_H

#include <vector>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../entities/entity/entity.hpp"
using namespace std;

class Quadtree;
class Entity;

class Quadtree : public Entity {
public:
    Quadtree(float x, float y, float width, float height, int level, int maxLevel);

    ~Quadtree();

    void					AddObject(Entity *object);
    vector<Entity*>				GetObjectsAt(float x, float y);
    void					Clear();

    void					SetFont(const sf::Font &font);
    void					Draw(sf::RenderWindow  *canvas);

private:
    float					x;
    float					y;
    float					width;
    float					height;
    int					level;
    int					maxLevel;
    vector<Entity*>				objects;

    Quadtree *				parent;
    Quadtree *				NW;
    Quadtree *				NE;
    Quadtree *				SW;
    Quadtree *				SE;

    sf::RectangleShape			shape;
    sf::Text				text;

    bool					contains(Quadtree *child, Entity *object);
};

#endif //PLATOFORMGAME_QUADTREE_H
