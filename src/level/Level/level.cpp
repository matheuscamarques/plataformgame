//
// Created by sanonichan on 28/02/2022.
//

#include "level.h"

Level::Level() {
    this->platforms = new std::vector<Entity>();
    this->enemies   = new std::vector<Entity>();
    this->generateLevel();
}
void Level::addEnemy(Entity enemy){
    this->enemies->push_back(enemy);
}
void Level::addPlatform(Entity platform){
    this->platforms->push_back(platform);
}
void Level::addPlayer(Entity *player){
    this->player = player;
}
std::vector<Entity>* Level::getEnemies(){
    return this->enemies;
}
std::vector<Entity>* Level::getPlatforms(){
    return  this->platforms;
}
Entity* Level::getPlayer(){
    return this->player;
}
void Level::generateLevel(){
    // percorre map set int values
    srand(time(NULL));
    for(int i=0;i<this->m;i++){
        for(int j=0;j<this->n;j++){
            // get random value 0.000 a 1.000
            if(i > m/2 + m/3){
                // set seed

                float random = (float) rand() / (float)RAND_MAX;
                // if random value is less than 0.2
                if (random < 0.2) {
                    // set value to 1
                    this->map[i][j] = 1;
                } else if (random > 0.2 && random < 0.4) {
                    this->map[i][j] = 2;
                } else if (random > 0.4 && random < 0.6) {
                    this->map[i][j] = 3;
                } else if (random > 0.6 && random < 0.8) {
                    this->map[i][j] = 4;
                } else if (random > 0.8 && random < 1.0) {
                    this->map[i][j] = 5;
                }
            }
        }
        for(int i=0;i<this->m;i++) {
            for (int j = 0; j < this->n; j++) {
                auto platform = Entity(j * 50 , i * 50 , 50,50);

                if(this->map[i][j] == 1){
                    platform.setFillColor(sf::Color::Red);
                }
                else if(this->map[i][j] == 2){
                    platform.setFillColor(sf::Color::Green);
                }
                else if(this->map[i][j] == 3){
                    platform.setFillColor(sf::Color::Blue);
                }
                else if(this->map[i][j] == 4){
                    platform.setFillColor(sf::Color::Yellow);
                }
                else if(this->map[i][j] == 5){
                    platform.setFillColor(sf::Color::Magenta);
                }
                this->addPlatform(platform);
            }
        }
    }
}

int Level::getM(){
    return this->m;
}

int Level::getN(){
    return this->n;
}

