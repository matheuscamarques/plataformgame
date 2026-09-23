/**
 * @file tests/test_peaks.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava picos nevados com spawn no flanco.
 * @details Cobre snowcap e surfaceHeight, roda com make test que compila em build/tests/test_peaks.
 */

#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "world/Generation.h"

// Picos nevados + spawn no flanco de montanha.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int snowCols = 0, n = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, seed);
            n++;
            if (!snowcap(s)) continue;
            snowCols++;
            // neve só em altitude que exige uplift (só montanha chega lá)
            assert(s <= 14);
            // pico é branco no mundo
            assert(tileType(tx, s, seed) == Tile::Snow);
            assert(tileType(tx, s, seed) == Tile::Snow); // determinístico
        }
        float f = (float)snowCols / n;
        std::printf("seed=%u neve=%.4f\n", seed, f);
        assert(f > 0.005f && f < 0.05f); // raro: pico, não campo

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
