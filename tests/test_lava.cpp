#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Lava no fundo, bedrock no abismo. Dano fica pra Fase C.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        for (int tx = -2000; tx < 2000; tx += 7) {
            int s = surfaceHeight(tx, seed);

            // 1. Bedrock absoluto em ty >= WORLD_BOTTOM.
            assert(tileType(tx, WORLD_BOTTOM, seed) == Tile::Bedrock);
            assert(tileType(tx, WORLD_BOTTOM + 10, seed) == Tile::Bedrock);

            // 2. Nenhum bedrock acima do fundo.
            for (int ty = s; ty < WORLD_BOTTOM; ty++) {
                assert(tileType(tx, ty, seed) != Tile::Bedrock);
            }

            // 3. Nenhuma lava acima de LAVA_LEVEL.
            for (int ty = s - 5; ty <= LAVA_LEVEL; ty++) {
                assert(tileType(tx, ty, seed) != Tile::Lava);
            }

            // 4. Lava só onde isCave seria true.
            for (int ty = LAVA_LEVEL + 1; ty < WORLD_BOTTOM; ty++) {
                if (tileType(tx, ty, seed) == Tile::Lava) {
                    float m = mountainMask(tx, 0, seed);
                    assert(isCave(tx, ty, seed, s, m));
                }
            }

            // 5. Determinismo.
            for (int ty : {LAVA_LEVEL + 1, LAVA_LEVEL + 10, WORLD_BOTTOM - 1}) {
                assert(tileType(tx, ty, seed) == tileType(tx, ty, seed));
            }
        }

        // 6. Fração de lava no fundo: nem 0%, nem mar de fogo.
        int lavaN = 0, total = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            for (int ty = LAVA_LEVEL + 5; ty < WORLD_BOTTOM; ty++) {
                total++;
                if (tileType(tx, ty, seed) == Tile::Lava) lavaN++;
            }
        }
        float f = (float)lavaN / total;
        std::printf("seed=%u lava frac=%.3f\n", seed, f);
        assert(f > 0.05f && f < 0.60f);
    }

    std::printf("lava test OK\n");
    return 0;
}
