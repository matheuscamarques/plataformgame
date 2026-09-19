#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"
#include "support/World/Block.h"

// Subsurface layering: topo | terra x DIRT_DEPTH | pedra. Caverna e água vencem.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int caveSamples = 0;
        for (int tx = -800; tx < 800; tx++) {
            ColumnData col = computeColumn(tx, seed);
            int s = col.surface;
            bool ocean = col.ocean;

            // 1. Topo == tile do bioma (ou neve).
            {
                Tile top = tileType(tx, s, seed);
                assert(top == tileType(tx, s, seed, col)); // sobrecarga idêntica
                Biome b = pickBiome(col.temperature, col.humidity,
                                    ocean, isCoastal(s));
                Tile expected = snowcap(s) ? Tile::Snow : biomeTopTile(b);
                assert(top == expected);
            }

            // 2/3/5. Zona de terra e pedra (terra firme, sem caverna no meio).
            for (int d = 1; d <= DIRT_DEPTH + 1; d++) {
                int ty = s + d;
                Tile t = tileType(tx, ty, seed);
                assert(t == tileType(tx, ty, seed, col)); // sobrecarga idêntica
                bool cave = isCave(tx, ty, seed, s, col.mountain);
                if (cave) {
                    assert(!isSolid(t)); // 7. caverna vence terra e pedra
                    if (d < 10) caveSamples++;
                    continue;
                }
                if (d <= DIRT_DEPTH) {
                    if (ocean) assert(t == Tile::Sand);  // 5. sob oceano: areia
                    else assert(t == Tile::Dirt);        // 2. terra
                } else {
                    assert(t == Tile::Stone);            // 3. pedra
                }
                assert(tileType(tx, ty, seed) == t); // 6. determinismo
            }

            // 4. Zero terra/pedra acima da superfície.
            for (int ty = s - 12; ty < s; ty++) {
                Tile t = tileType(tx, ty, seed);
                assert(t != Tile::Dirt && t != Tile::Stone);
            }
        }
        assert(caveSamples > 50); // teste 7 com amostra real, não vácuo
        std::printf("seed=%u camadas OK (cavernas amostradas=%d)\n", seed, caveSamples);
    }

    std::printf("layers test OK\n");
    return 0;
}
