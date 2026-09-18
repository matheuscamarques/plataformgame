//
// Created by sanonichan on 28/02/2022.
//

#include "level.h"
#include<string.h>

Level::Level(std::unique_ptr<Quadtree> qt) : quadtree(std::move(qt)), map(m, std::vector<int>(n, 0)) {
    this->generateLevel();
}

Level::~Level() = default;

void Level::addEnemy(std::unique_ptr<Entity> enemy)
{
    this->enemies.push_back(std::move(enemy));
}
void Level::addPlatform(std::unique_ptr<Entity> platform)
{
    this->platforms.push_back(std::move(platform));
}

std::vector<std::unique_ptr<Entity>> &Level::getEnemies()
{
    return this->enemies;
}
std::vector<std::unique_ptr<Entity>> &Level::getPlatforms()
{
    return this->platforms;
}

void Level::generateLevel()
{
    // Geração determinística por tile: mesma (coluna, linha, seed) = mesmo mapa.
    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->n; j++)
        {
            // get random value 0.000 a 1.000
            if (i > m / 2 + m / 3)
            {
                // valor determinístico do tile (coluna j, linha i)
                float random = rand01(j, i, seed);
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
                // valor determinístico do tile (coluna j, linha i)
                float random = rand01(j, i, seed);
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
                // valor determinístico do tile (coluna j, linha i)
                float random = rand01(j, i, seed);
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
                // valor determinístico do tile (coluna j, linha i)
                float random = rand01(j, i, seed);
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
                // valor determinístico do tile (coluna j, linha i)
                float random = rand01(j, i, seed);
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
                // valor determinístico do tile (coluna j, linha i)
                float random = rand01(j, i, seed);
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
            auto platform = std::make_unique<Entity>(COLIDE, j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

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
            this->platforms.push_back(std::move(platform));
        }
    }

    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->n; j++)
        {
            if (i > m / 2)
            {
                // search in plataforms if there is a platform
                int isValid = 0;
                for (auto &slot : this->platforms)
                {
                    Entity *platform = slot.get();
                    if ((int)platform->getX() == j * BLOCK_SIZE && (int)platform->getY() == i * BLOCK_SIZE)
                    {
                        isValid = 1;
                        break;
                    }
                }

                if (isValid == 0)
                {
                    auto platform = std::make_unique<Entity>(WATER, j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    platform->setFillColor(sf::Color(0, 255, 255));
                    this->platforms.push_back(std::move(platform));
                }
            }
        }
    }

    // Água só entra em platforms; colisão usa filtro abaixo.
    colidesPlatforms.clear();
    for (auto &slot : platforms)
    {
        if (slot->getName() != WATER)
        {
            colidesPlatforms.push_back(slot.get());
        }
    }
}

std::vector<Entity*> & Level::getColidePlatforms(){
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
