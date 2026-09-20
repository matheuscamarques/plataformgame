#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "world/Generation.h"

// Lagos pintam ar já carvado: seco fora do limiar, água onde molhado.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        // 1) Seco fora do limiar: wet==0 onde o noise está baixo.
        {
            int dry = 0, n = 0;
            for (int ty = 0; ty < 200; ty += 5) {
                for (int tx = -500; tx < 500; tx += 5) {
                    n++;
                    if (lakeWet(tx, ty, seed) == 0.0f) dry++;
                }
            }
            float fDry = (float)dry / n;
            std::printf("seed=%u seco=%.3f\n", seed, fDry);
            assert(fDry > 0.50f); // maioria seca: lago é exceção
            assert(lakeWet(0, 0, seed) == lakeWet(0, 0, seed)); // determinístico
        }

        // 2) Caverna rasa molhada vira água (não ar).
        {
            int found = 0;
            for (int tx = -2000; tx < 2000 && found < 50; tx++) {
                int s = surfaceHeight(tx, seed);
                for (int ty = s + 3; ty < s + 20; ty++) {
                    // caverna rasa (acima da lava) e molhada
                    if (isLavaDepth(ty)) continue;
                    float m = mountainMask(tx, 0, seed);
                    if (!isCave(tx, ty, seed, s, m)) continue;
                    if (lakeWet(tx, ty, seed) <= 0.0f) continue;
                    assert(tileType(tx, ty, seed) == Tile::Water);
                    found++;
                    break;
                }
            }
            std::printf("seed=%u lagos-rasos=%d\n", seed, found);
            assert(found > 0);
        }

        // 3) Lava vence onde a poça manda, mesmo molhado; fora da poça, não.
        {
            int lavaChecked = 0, airChecked = 0;
            for (int tx = -2000; tx < 2000 && (lavaChecked < 20 || airChecked < 20); tx++) {
                int s = surfaceHeight(tx, seed);
                for (int ty = LAVA_DEPTH_START + 6; ty < LAVA_DEPTH_START + 400; ty++) {
                    float m = mountainMask(tx, 0, seed);
                    if (!isCave(tx, ty, seed, s, m)) continue;
                    float w = lakeWet(tx, ty, seed);
                    if (w > LAVA_WET_THRESHOLD && lavaChecked < 20) {
                        assert(tileType(tx, ty, seed) == Tile::Lava);
                        lavaChecked++;
                        break;
                    }
                    if (w > 0.0f && w <= LAVA_WET_THRESHOLD && airChecked < 20) {
                        assert(tileType(tx, ty, seed) != Tile::Lava);
                        airChecked++;
                        break;
                    }
                }
            }
            assert(lavaChecked > 0 && airChecked > 0);
        }
    }

    std::printf("lakes test OK\n");
    return 0;
}
