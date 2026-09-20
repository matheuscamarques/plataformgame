// Harness: replica Game::tick sem janela/teclado e verifica se o player pousa.
#include <cmath>
#include <cstdio>
#include <vector>
#include "world/World.h"
#include "entities/player/player.h"
#include "defines.h"

int main() {
    support::World world(1337u);
    Player p;
    p.setX(100.0f);
    p.setY(0.0f);

    float lastY = 0.0f;
    int stable = 0;
    int landedTick = -1;
    for (int t = 0; t < 600; t++) {
        p.tick(); // gravidade (teclado headless = tudo solto)
        int ptx = static_cast<int>(std::floor(p.getX() / BLOCK_SIZE));
        int pty = static_cast<int>(std::floor(p.getY() / BLOCK_SIZE));
        world.update(ptx, pty);
        std::vector<Entity*> candidatos;
        world.query(p.getX() - BLOCK_SIZE, p.getY() - BLOCK_SIZE,
                    p.getW() + BLOCK_SIZE * 2, p.getH() + BLOCK_SIZE * 2,
                    candidatos);
        if (t == 590) std::printf("candidatos@t590=%zu y=%.1f\n", candidatos.size(), p.getY());
        for (Entity *e : candidatos) {
            if (p.isColide(*e)) p.collide(*e);
        }
        if (std::fabs(p.getY() - lastY) < 0.001f) {
            if (++stable == 50) { landedTick = t; break; }
        } else {
            stable = 0;
        }
        lastY = p.getY();
        if (p.getY() > 20000.0f) break; // atravessou tudo
    }
    std::printf("y_final=%.1f landedTick=%d\n", p.getY(), landedTick);
    if (landedTick < 0 || p.getY() < 500.0f || p.getY() > 20000.0f) {
        std::printf("FALHOU: player nao pousou (atravessou ou nao caiu)\n");
        return 1;
    }
    std::printf("OK: player pousou e estabilizou\n");
    return 0;
}
