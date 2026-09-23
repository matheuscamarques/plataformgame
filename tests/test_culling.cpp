/**
 * @file tests/test_culling.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava culling igual filtro força-bruta por rect.
 * @details Cobre World e forEachEntityInRect, roda com make test que compila em build/tests/test_culling.
 */

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>

#include "entities/Entity.hpp"
#include "world/World.h"

// Culling de render: forEachEntityInRect == filtro força-bruta da lista
// global, visitando só chunks do rect (pinned modified não encarece).
int main() {
    using namespace support;

    World world(1337u);
    world.update(0, 26);
    // Sujidade: modifica faixa larga para pinnar chunks no LRU.
    for (int leg = 0; leg < 12; leg++) {
        int baseX = leg * 30;
        world.update(baseX, 26);
        for (int i = 0; i < 10; i++) world.breakTile(baseX + i, 35);
    }
    world.update(0, 26);

    const auto &all = world.getPlatforms();
    std::printf("culling: loaded=%zu active=%zu\n",
                world.loadedChunkCount(), all.size());

    const float rects[][4] = {
        {-1600.f, 400.f, 920.f, 1320.f},  // viewport superfície
        {-5000.f, -500.f, 5000.f, 600.f}, // largo, coords negativas
        {0.f, 30000.f, 800.f, 30800.f},   // vazio (sem chunks)
        {-1600.f, 400.f, -1500.f, 500.f}, // 1 chunk parcial
    };
    for (auto &r : rects) {
        std::vector<Entity *> expect;
        for (Entity *e : all) {
            if (e->getX() + e->getW() < r[0] || e->getX() > r[2] ||
                e->getY() + e->getH() < r[1] || e->getY() > r[3])
                continue;
            expect.push_back(e);
        }
        std::vector<Entity *> got;
        world.forEachEntityInRect(r[0], r[1], r[2], r[3], [&](Entity *e) {
            got.push_back(e);
        });
        // got == expect: sem faltar nem sobrar.
        std::sort(expect.begin(), expect.end());
        std::sort(got.begin(), got.end());
        assert(expect == got);
    }

    // Janela pequena visita fração mínima do total pinned.
    std::size_t small = 0;
    world.forEachEntityInRect(-60.f, 440.f, 860.f, 1360.f,
                              [&](Entity *) { small++; });
    std::printf("culling: janela 920x920 -> %zu de %zu\n", small, all.size());
    assert(small < all.size());

    std::printf("culling test OK\n");
    return 0;
}
