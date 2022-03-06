#ifndef PLATOFORMGAME_LEVEL_H
#define PLATOFORMGAME_LEVEL_H
#include <vector>

#include "../../entities/entity/entity.hpp"
#include "../../entities/player/player.h"
#include "../../quadtree/quadtree.h"


#define  M 10;
#define  N 100;

class Level {
    private:
        std::vector<Entity*> *enemies;
        std::vector<Entity*> *platforms;
        std::vector<Entity*> *colidesPlatforms;
        Player *player = nullptr;
        int m = M;
        int n = N;

        void generateLevel();
    public:
        explicit Level(Quadtree *quadtree);
        Quadtree *quadtree;
        void addEnemy(Entity *enemy);
        void addPlatform(Entity *platform);
        std::vector<Entity*> * getEnemies();
        std::vector<Entity*> * getPlatforms();
        std::vector<Entity*> * getColidePlatforms();
        Player* getPlayer();
        //Camera* getCamera();
       // void setCamera(Camera* camera);

       int getM();
       int getN();

    int map[10][100]  = {}; // quadtree

    void setPlayer(Player *pPlayer);
};


#endif //PLATOFORMGAME_LEVEL_H
