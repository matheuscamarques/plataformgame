#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Ilhas: runs horizontais 4-7, nunca sobre oceano, nunca no maciço.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int islands = 0, maxRun = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, seed);
            // varre o céu da coluna: todo type-2 acima da superfície
            // pertence a um run horizontal >= 4 no mesmo ty.
            for (int ty = s - 14; ty < s; ty++) {
                int t = tileType(tx, ty, seed);
                assert(t == 0 || t == 2);
                assert(tileType(tx, ty, seed) == t); // determinístico
                if (t != 2) continue;
                if (tx > -2000 && tileType(tx - 1, ty, seed) == 2) continue; // meio do run
                int run = 0;
                while (tileType(tx + run, ty, seed) == 2) run++;
                assert(run >= 4); // sem tile isolado voando
                if (run > maxRun) maxRun = run;
                islands++;
            }
        }
        std::printf("seed=%u ilhas=%d maxRun=%d\n", seed, islands, maxRun);
        assert(maxRun <= 16); // 4..7 por ilha; fusão vizinha rara e limitada
        assert(islands > 20); // ~45% de 100 grupos
    }

    // nenhuma ilha sobre coluna oceânica
    for (uint32_t seed : {1337u, 999u, 42u}) {
        for (int tx = -2000; tx < 2000; tx++) {
            if (!isOceanColumn(tx, seed)) continue;
            int s = surfaceHeight(tx, seed);
            for (int ty = s - 14; ty < s; ty++) {
                assert(tileType(tx, ty, seed) == 0);
            }
        }
    }

    std::printf("islands test OK\n");
    return 0;
}
