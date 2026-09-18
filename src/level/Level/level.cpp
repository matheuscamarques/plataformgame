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
            // Terreno coerente: superfície suave via noise + tipos por hash.
            // Funciona para qualquer coluna j (inclusive negativa, Fase 4).
            float relief = valueNoise2D(j * 0.02f, 3.7f, seed); // [0,1] suave
            int surface = m / 2 - 4 + static_cast<int>(relief * 9.0f);

            if (i > surface)
            {
                // Maciço: sempre sólido; tipo cosmético varia por hash.
                float pick = rand01(j, i, seed ^ 0x9E3779B9u);
                if (pick < 0.15f)      this->map[i][j] = 1;
                else if (pick < 0.35f) this->map[i][j] = 2;
                else if (pick < 0.55f) this->map[i][j] = 3;
                else if (pick < 0.75f) this->map[i][j] = 4;
                else                   this->map[i][j] = 5;
            }
            else if (i == surface)
            {
                // Topo do terreno: sempre sólido (chão contínuo e caminhável).
                this->map[i][j] = 4;
            }
            else
            {
                // Acima do terreno: vazio, com plataformas flutuantes esparsas.
                float plat = rand01(j, i, seed ^ 0x51F37EDu);
                if (plat < 0.035f) this->map[i][j] = 2;
                else               this->map[i][j] = 0;
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
