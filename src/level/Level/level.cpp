//
// Created by sanonichan on 28/02/2022.
//

#include "level.h"
#include<string.h>

Level::Level(Quadtree *quadtree) : map(m, std::vector<int>(n, 0)) {
    this->quadtree = quadtree;
    this->platforms = new std::vector<Entity*>();
    this->colidesPlatforms = new std::vector<Entity*>();
    this->enemies = new std::vector<Entity*>();

    this->generateLevel();
}

Level::~Level() {
    for (Entity *e : *platforms) delete e;
    for (Entity *e : *enemies) delete e;
    // colidesPlatforms compartilha ponteiros com platforms: só limpa, não deleta
    delete platforms;
    delete colidesPlatforms;
    delete enemies;
    delete player;
    delete quadtree;
}
void Level::addEnemy(Entity *enemy)
{
    this->enemies->push_back(enemy);
}
void Level::addPlatform(Entity *platform)
{
    this->platforms->push_back(platform);
}

std::vector<Entity*> *Level::getEnemies()
{
    return this->enemies;
}
std::vector<Entity*> *Level::getPlatforms()
{
    return this->platforms;
}
Player *Level::getPlayer()
{
    return this->player;
}

void Level::generateLevel()
{
    // percorre map set int values
    srand(time(NULL));
    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->n; j++)
        {
            // get random value 0.000 a 1.000
            if (i > m / 2 + m / 3)
            {
                // set seed
                float random = (float)rand() / (float)RAND_MAX;
                // if random value is less than 0.2
                if (random < 0.2)
                {
                    // set value to 1
                    this->map[i][j] = 1;
                }
                else if (random > 0.2 && random < 0.4)
                {
                    this->map[i][j] = 5;
                }
                else if (random > 0.4 && random < 0.6)
                {
                    this->map[i][j] = 3;
                }
                else if (random > 0.6 && random < 0.8)
                {
                    this->map[i][j] = 3;
                }
                else if (random > 0.8 && random < 1.0)
                {
                    this->map[i][j] = 5;
                }
                else
                {
                    this->map[i][j] = 0;
                }
                continue;
            }

            if (i > m / 2 + m / 4)
            {
                // set seed
                float random = (float)rand() / (float)RAND_MAX;
                // if random value is less than 0.2
                if (random < 0.2)
                {
                    // set value to 1
                    this->map[i][j] = 0;
                }
                else if (random > 0.2 && random < 0.4)
                {
                    this->map[i][j] = 2;
                }
                else if (random > 0.4 && random < 0.6)
                {
                    this->map[i][j] = 3;
                }
                else if (random > 0.6 && random < 0.8)
                {
                    this->map[i][j] = 4;
                }
                else if (random > 0.8 && random < 1.0)
                {
                    this->map[i][j] = 5;
                }
                else
                {
                    this->map[i][j] = 0;
                }
                continue;
            }

            if (i > m / 2 + m / 5)
            {
                // set seed
                float random = (float)rand() / (float)RAND_MAX;
                // if random value is less than 0.2
                if (random < 0.2)
                {
                    // set value to 1
                    this->map[i][j] = 0;
                }
                else if (random > 0.2 && random < 0.4)
                {
                    this->map[i][j] = 2;
                }
                else if (random > 0.4 && random < 0.6)
                {
                    this->map[i][j] = 0;
                }
                else if (random > 0.6 && random < 0.8)
                {
                    this->map[i][j] = 4;
                }
                else if (random > 0.8 && random < 1.0)
                {
                    this->map[i][j] = 5;
                }
                else
                {
                    this->map[i][j] = 0;
                }
                continue;
            }

            if (i > m / 2 + m / 6)
            {
                // set seed
                float random = (float)rand() / (float)RAND_MAX;
                // if random value is less than 0.2
                if (random < 0.2)
                {
                    // set value to 1
                    this->map[i][j] = 0;
                }
                else if (random > 0.2 && random < 0.4)
                {
                    this->map[i][j] = 2;
                }
                else if (random > 0.4 && random < 0.6)
                {
                    this->map[i][j] = 0;
                }
                else if (random > 0.6 && random < 0.8)
                {
                    this->map[i][j] = 4;
                }
                else if (random > 0.8 && random < 1.0)
                {
                    this->map[i][j] = 0;
                }
                else
                {
                    this->map[i][j] = 0;
                }
                continue;
            }

            if (i > m / 2 + m / 7)
            {
                // set seed
                float random = (float)rand() / (float)RAND_MAX;
                // if random value is less than 0.2
                if (random < 0.2)
                {
                    // set value to 1
                    this->map[i][j] = 0;
                }
                else if (random > 0.2 && random < 0.4)
                {
                    this->map[i][j] = 2;
                }
                else if (random > 0.4 && random < 0.6)
                {
                    this->map[i][j] = 0;
                }
                else if (random > 0.6 && random < 0.8)
                {
                    this->map[i][j] = 0;
                }
                else if (random > 0.8 && random < 1.0)
                {
                    this->map[i][j] = 0;
                }
                else
                {
                    this->map[i][j] = 0;
                }
                continue;
            }

            if (i > m / 2)
            {
                // set seed
                float random = (float)rand() / (float)RAND_MAX;
                // if random value is less than 0.2
                if (random < 0.2)
                {
                    // set value to 1
                    this->map[i][j] = 4;
                }
                else if (random > 0.2 && random < 0.4)
                {
                    this->map[i][j] = 2;
                }
                else if (random > 0.4 && random < 0.6)
                {
                    this->map[i][j] = 0;
                }
                else if (random > 0.6 && random < 0.8)
                {
                    this->map[i][j] = 0;
                }
                else if (random > 0.8 && random < 1.0)
                {
                    this->map[i][j] = 4;
                }
                else
                {
                    this->map[i][j] = 0;
                }
                continue;
            }
        }
    }

    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->n; j++)
        {
            if (this->map[i][j] == 0)
            {
                continue;
            }
            auto platform = new Entity(COLIDE, j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

            if (this->map[i][j] == 1)
            {
                platform->setFillColor(sf::Color(60, 60, 60));
            }
            else if (this->map[i][j] == 2)
            {
                platform->setFillColor(sf::Color(146, 90, 43));
            }
            else if (this->map[i][j] == 3)
            {
                platform->setFillColor(sf::Color(120, 60, 0));
            }
            else if (this->map[i][j] == 4)
            {
                platform->setFillColor(sf::Color(159, 89, 30));
            }
            else if (this->map[i][j] == 5)
            {
                platform->setFillColor(sf::Color(150, 75, 0));
            }
            this->platforms->push_back(platform);
        }
    }

    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->n; j++)
        {
            if (i > m / 2)
            {
                // search in plataforms if there is a platform
                std::vector<Entity*> *list = this->getPlatforms();
                auto p = list->begin();
                int isValid = 0;
                while (p != this->getPlatforms()->end())
                {
                    Entity * platform = *p;
                    if ((int)platform->getX() == j * BLOCK_SIZE && (int)platform->getY() == i * BLOCK_SIZE)
                    {
                        isValid = 1;
                    }
                    p++;
                }

                if (isValid == 0)
                {
                    auto platform = new Entity(WATER, j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    platform->setFillColor(sf::Color(0, 255, 255));
                    this->platforms->push_back(platform);
                }
            }
        }
    }

    // Água só entra em platforms; colisão usa filtro abaixo.
    colidesPlatforms->clear();
    for (Entity *e : *platforms)
    {
        if (e->getName() != WATER)
        {
            colidesPlatforms->push_back(e);
        }
    }
}

std::vector<Entity*> *  Level::getColidePlatforms(){
    return colidesPlatforms;
}

int Level::getM()
{
    return this->m;
}

int Level::getN()
{
    return this->n;
}

void Level::setPlayer(Player *pPlayer)
{
    this->player = pPlayer;
}
