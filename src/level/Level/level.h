#ifndef PLATOFORMGAME_LEVEL_H
#define PLATOFORMGAME_LEVEL_H
#include <memory>
#include <vector>

#include "../../entities/entity/entity.hpp"
#include "../../quadtree/quadtree.h"
#include "../../defines.h"

#define  M 50
#define  N 1000

class Level {
    private:
        std::vector<std::unique_ptr<Entity>> enemies;
        std::vector<std::unique_ptr<Entity>> platforms;
        // views não-owning para dentro de platforms (água fica fora)
        std::vector<Entity*> colidesPlatforms;
        int m = M;
        int n = N;

        void generateLevel();
    public:
        explicit Level(std::unique_ptr<Quadtree> qt);
        ~Level();
        std::unique_ptr<Quadtree> quadtree;
        void addEnemy(std::unique_ptr<Entity> enemy);
        void addPlatform(std::unique_ptr<Entity> platform);
        std::vector<std::unique_ptr<Entity>> & getEnemies();
        std::vector<std::unique_ptr<Entity>> & getPlatforms();
        std::vector<Entity*> & getColidePlatforms();

       int getM();
       int getN();

    std::vector<std::vector<int>> map;
};


#endif //PLATOFORMGAME_LEVEL_H
