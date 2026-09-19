#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Subsurface layering: topo | terra x DIRT_DEPTH | pedra. Caverna e água vencem.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int caveSamples = 0;
        for (int tx = -800; tx < 800; tx++) {
            int s = surfaceHeight(tx, seed);
            bool ocean = isOceanColumn(tx, seed);

            // 1. Topo == tile do bioma (ou neve).
            {
                int top = tileType(tx, s, seed);
                Biome b = pickBiome(temperature(tx, s, seed), humidity(tx, s, seed),
                                    ocean, isCoastal(s));
                int expected = snowcap(tx, seed, s) ? 8 : biomeTopTile(b);
                assert(top == expected);
            }

            // 2/3/5. Zona de terra e pedra (terra firme, sem caverna no meio).
            for (int d = 1; d <= DIRT_DEPTH + 1; d++) {
                int ty = s + d;
                int t = tileType(tx, ty, seed);
                bool cave = isCave(tx, ty, seed, s, mountainMask(tx, 0, seed));
                if (cave) {
                    assert(t == 0); // 7. caverna vence terra e pedra
                    if (d < 10) caveSamples++;
                    continue;
                }
                if (d <= DIRT_DEPTH) {
                    if (ocean) assert(t == 6);  // 5. sob oceano: areia, nunca terra
                    else assert(t == 10);       // 2. terra
                } else {
                    assert(t == 11);            // 3. pedra
                }
                assert(tileType(tx, ty, seed) == t); // 6. determinismo
            }

            // 4. Zero terra/pedra acima da superfície.
            for (int ty = s - 12; ty < s; ty++) {
                int t = tileType(tx, ty, seed);
                assert(t != 10 && t != 11);
            }
        }
        assert(caveSamples > 50); // teste 7 com amostra real, não vácuo
        std::printf("seed=%u camadas OK (cavernas amostradas=%d)\n", seed, caveSamples);
    }

    std::printf("layers test OK\n");
    return 0;
}
