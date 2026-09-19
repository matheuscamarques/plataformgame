#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Lava só no fundo (ty>=6200), bedrock em 12000. Faixas amostradas,
// nunca sweep completo (o fundo agora tem 12k tiles).
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        for (int tx = -2000; tx < 2000; tx += 7) {
            int s = surfaceHeight(tx, seed);

            // 1. Bedrock absoluto em ty >= WORLD_BOTTOM.
            assert(tileType(tx, WORLD_BOTTOM, seed) == Tile::Bedrock);
            assert(tileType(tx, WORLD_BOTTOM + 10, seed) == Tile::Bedrock);

            // 2. Nenhum bedrock acima do fundo (faixa rasa + spots fundos).
            for (int ty = s; ty < s + 60; ty++) {
                assert(tileType(tx, ty, seed) != Tile::Bedrock);
            }
            for (int ty : {1000, 3000, 6000, 9000, 11500}) {
                assert(tileType(tx, ty, seed) != Tile::Bedrock);
            }

            // 3. Nenhuma lava acima de LAVA_DEPTH_START.
            for (int ty = s - 5; ty < s + 120; ty++) {
                assert(tileType(tx, ty, seed) != Tile::Lava);
            }
            for (int ty : {1000, 2000, 3000, 4000, 5000, 6000}) {
                assert(tileType(tx, ty, seed) != Tile::Lava);
            }

            // 4. Lava só onde isCave seria true (faixa funda amostrada).
            for (int ty = LAVA_DEPTH_START; ty < LAVA_DEPTH_START + 200; ty += 3) {
                if (tileType(tx, ty, seed) == Tile::Lava) {
                    float m = mountainMask(tx, 0, seed);
                    assert(isCave(tx, ty, seed, s, m));
                }
            }

            // 5. Determinismo.
            for (int ty : {6199, 6200, 6300, WORLD_BOTTOM - 1}) {
                assert(tileType(tx, ty, seed) == tileType(tx, ty, seed));
            }
        }

        // 6. Fração de lava na faixa funda: nem 0%, nem mar de fogo.
        int lavaN = 0, total = 0;
        for (int tx = -500; tx < 500; tx += 2) {
            for (int ty = LAVA_DEPTH_START; ty < LAVA_DEPTH_START + 100; ty++) {
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
