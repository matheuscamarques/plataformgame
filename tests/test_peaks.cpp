#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Picos nevados + spawn no flanco de montanha.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int snowCols = 0, n = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, seed);
            n++;
            if (!snowcap(tx, seed, s)) continue;
            snowCols++;
            // neve só em pico alto e forte
            float m = mountainMask(tx, 0, seed);
            float t = (m - MOUNTAIN_THRESHOLD) / (1.f - MOUNTAIN_THRESHOLD);
            assert(t > 0.65f && s <= 14);
            // pico é branco no mundo
            assert(tileType(tx, s, seed) == 8);
            assert(tileType(tx, s, seed) == 8); // determinístico
        }
        float f = (float)snowCols / n;
        std::printf("seed=%u neve=%.4f\n", seed, f);
        assert(f > 0.005f && f < 0.06f); // raro: pico, não campo

        // spawn: flanco (uplift 8..20), perto, terra, determinístico
        int sx = findSpawnTileX(0, seed);
        int s = surfaceHeight(sx, seed);
        int up = 29 - s;
        std::printf("seed=%u spawnTx=%d uplift=%d\n", seed, sx, up);
        assert(sx >= -2000 && sx <= 2000);
        assert(up >= 8 && up <= 20);
        assert(!isOceanColumn(sx, seed)); // terra garantida
        assert(findSpawnTileX(0, seed) == sx);
    }

    std::printf("peaks test OK\n");
    return 0;
}
