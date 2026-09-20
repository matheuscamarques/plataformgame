#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "world/Generation.h"

// Ilhas: runs horizontais 4-7, nunca sobre oceano, nunca no maciço.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int islands = 0, maxRun = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, seed);
            bool ocean = isOceanColumn(tx, seed);
            // varre o céu da coluna: ilha em run >= 4, resto ar
            // (ou água na banda do oceano).
            for (int ty = s - 14; ty < s; ty++) {
                Tile t = tileType(tx, ty, seed);
                bool waterOk = ocean && ty >= SEA_LEVEL && t == Tile::Water;
                assert(t == Tile::Air || t == Tile::IslandPlatform || waterOk);
                assert(tileType(tx, ty, seed) == t); // determinístico
                if (t != Tile::IslandPlatform) continue;
                if (tx > -2000 && tileType(tx - 1, ty, seed) == Tile::IslandPlatform) continue; // meio do run
                int run = 0;
                while (tileType(tx + run, ty, seed) == Tile::IslandPlatform) run++;
                assert(run >= 4); // sem tile isolado voando
                if (run > maxRun) maxRun = run;
                islands++;
            }
        }
        std::printf("seed=%u ilhas=%d maxRun=%d\n", seed, islands, maxRun);
        assert(maxRun <= 16); // 4..7 por ilha; fusão vizinha rara e limitada
        assert(islands > 20); // ~45% de 100 grupos
    }

    // nenhuma ilha sobre coluna oceânica (só ar acima do mar, água na banda)
    for (uint32_t seed : {1337u, 999u, 42u}) {
        for (int tx = -2000; tx < 2000; tx++) {
            if (!isOceanColumn(tx, seed)) continue;
            int s = surfaceHeight(tx, seed);
            for (int ty = s - 14; ty < s; ty++) {
                Tile t = tileType(tx, ty, seed);
                assert(t == Tile::Air || (ty >= SEA_LEVEL && t == Tile::Water));
            }
        }
    }

    std::printf("islands test OK\n");
    return 0;
}
