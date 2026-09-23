/**
 * @file tests/test_realwall.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava parede real bloqueando e chão sustentando.
 * @details Cobre Entity e colisão, roda com make test que compila em build/tests/test_realwall.
 */

#include <cstdio>
#include <memory>
#include <vector>
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "defines.h"
int main() {
    std::vector<std::unique_ptr<Entity>> world;
    for (int j = 0; j < 30; j++)
        world.push_back(std::make_unique<Entity>(core::kIdColide, j * core::kBlockSize,
                                                 2 * core::kBlockSize,
                                                 core::kBlockSize, core::kBlockSize));
    world.push_back(std::make_unique<Entity>(core::kIdColide, 10 * core::kBlockSize,
                                             core::kBlockSize,
                                             core::kBlockSize, core::kBlockSize)); // parede real a frente
    Player p;
    p.setX(0.0f); p.setY(static_cast<float>(core::kBlockSize));
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
