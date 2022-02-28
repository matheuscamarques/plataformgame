#ifndef PLATOFORMGAME_LEVEL_H
#define PLATOFORMGAME_LEVEL_H
#include <vector>

#include "../../entities/entity/entity.hpp"
#define  M  9;
#define  N 22;

class Level {
    private:
        std::vector<Entity> *enemies;
        std::vector<Entity> *platforms;
        Entity *player = nullptr;
        int m = M;
        int n = N;

        void generateLevel();
    public:
        Level();
        void addEnemy(Entity enemy);
        void addPlatform(Entity platform);
        void addPlayer(Entity *player);
        std::vector<Entity> * getEnemies();
        std::vector<Entity> * getPlatforms();
        Entity* getPlayer();
       int getM();
       int getN();
    int map[9][22]  = {
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

};


#endif //PLATOFORMGAME_LEVEL_H
