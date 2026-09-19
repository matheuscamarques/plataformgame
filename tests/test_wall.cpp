// Harness: player andando sobre chao longo, processando vizinho ANTES do chao
// (pior ordem possivel do hash) — reproduz a parede invisivel nas emendas.
#include <cstdio>
#include <memory>
#include <vector>
#include "entities/entity/entity.hpp"
#include "entities/player/player.h"
#include "defines.h"

int main() {
    std::vector<std::unique_ptr<Entity>> floor;
    for (int j = 0; j < 30; j++)
        floor.push_back(std::make_unique<Entity>(COLIDE, j * 50, 100, 50, 50));
    Player p;
    p.setX(0.0f);
    p.setY(50.0f); // em pe

    float maxX = 0.0f;
    for (int t = 0; t < 120; t++) {
        p.setVx(9.8f);
        p.setVy(9.8f);
        p.Entity::tick();
        // ordem reversa: vizinho da frente primeiro (como o hash pode devolver)
        for (int k = (int)floor.size() - 1; k >= 0; k--) {
            if (p.isColide(*floor[k])) p.collide(*floor[k]);
        }
        if (p.getX() > maxX) maxX = p.getX();
        if (p.getY() > 200.0f) break; // caiu do mundo
    }
    std::printf("x_final=%.1f y_final=%.1f maxX=%.1f\n", p.getX(), p.getY(), maxX);
    if (maxX < 500.0f) {
        std::printf("FALHOU: emperrou na emenda (parede invisivel)\n");
        return 1;
    }
    if (p.getY() < 40.0f || p.getY() > 60.0f) {
        std::printf("FALHOU: saiu do chao y=%.1f\n", p.getY());
        return 1;
    }
    std::printf("OK: atravessou as emendas andando\n");
    return 0;
}
