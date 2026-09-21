#include <cstdio>
#include <memory>
#include <vector>
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "defines.h"
int main() {
    std::vector<std::unique_ptr<Entity>> world;
    for (int j = 0; j < 30; j++)
        world.push_back(std::make_unique<Entity>(COLIDE, j * 50, 100, 50, 50));
    world.push_back(std::make_unique<Entity>(COLIDE, 500, 50, 50, 50)); // parede real a frente
    Player p;
    p.setX(0.0f); p.setY(50.0f);
    float maxX = 0.0f;
    for (int t = 0; t < 120; t++) {
        p.setVx(9.8f); p.setVy(9.8f); p.Entity::tick();
        for (int k = (int)world.size() - 1; k >= 0; k--)
            if (p.isColide(*world[k])) p.collide(*world[k]);
        if (p.getX() > maxX) maxX = p.getX();
    }
    std::printf("maxX=%.1f (parede em x=500, player 30 de largura para em ~470)\n", maxX);
    if (maxX > 471.0f) { std::printf("FALHOU: atravessou parede real\n"); return 1; }
    std::printf("OK: parede real bloqueia\n");
    return 0;
}
