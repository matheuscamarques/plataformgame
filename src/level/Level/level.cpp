//
// Created by sanonichan on 28/02/2022.
//

#include "level.h"
#define BLOCK_SIZE 40

Level::Level()
{
    this->platforms = new std::vector<Entity>();
    this->enemies = new std::vector<Entity>();
    this->generateLevel();
}
void Level::addEnemy(Entity enemy)
{
    this->enemies->push_back(enemy);
}
void Level::addPlatform(Entity platform)
{
    this->platforms->push_back(platform);
}
void Level::addPlayer(Entity *player)
{
    this->player = player;
}
std::vector<Entity> *Level::getEnemies()
{
    return this->enemies;
}
std::vector<Entity> *Level::getPlatforms()
{
    return this->platforms;
}
Entity *Level::getPlayer()
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

            if (i > m / 2 )
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
        for (int i = 0; i < this->m; i++)
        {
            for (int j = 0; j < this->n; j++)
            {
                auto platform = Entity(j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

                if (this->map[i][j] == 1)
                {
                    platform.setFillColor(sf::Color(60, 60, 60));
                }
                else if (this->map[i][j] == 2)
                {
                    platform.setFillColor(sf::Color(146, 90, 43));
                }
                else if (this->map[i][j] == 3)
                {
                    platform.setFillColor(sf::Color(120, 60, 0));
                }
                else if (this->map[i][j] == 4)
                {
                    platform.setFillColor(sf::Color(159, 89, 30));
                }
                else if (this->map[i][j] == 5)
                {
                    platform.setFillColor(sf::Color(150, 75, 0));
                }
                if (this->map[i][j] != 0)
                {
                    this->platforms->push_back(platform);
                }
            }
        }

        for (int i = 0; i < this->m; i++)
        {
            for (int j = 0; j < this->n; j++)
            {
                if (i > m / 2)
                {
                    // search in plataforms if there is a platform
                    std::vector<Entity> *list = this->getPlatforms();
                    auto p = list->begin();
                    int isValid = 0;
                    while (p != this->getPlatforms()->end())
                    {
                        if ((int)p->getX() == j * BLOCK_SIZE && (int)p->getY() == i * BLOCK_SIZE)
                        {
                            isValid = 1;
                        }
                        p++;
                    }

                    if (isValid == 0)
                    {
                        auto platform = Entity(j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                        platform.setFillColor(sf::Color(0, 255, 255));
                        this->platforms->push_back(platform);
                    }
                }

                
            }
        }

        
    }
}

int Level::getM()
{
    return this->m;
}

int Level::getN()
{
    return this->n;
}
