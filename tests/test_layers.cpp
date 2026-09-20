#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "world/Generation.h"
#include "world/Block.h"

// Subsurface layering: topo | terra x DIRT_DEPTH | pedra. Caverna e água vencem.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int caveSamples = 0;
        for (int tx = -800; tx < 800; tx++) {
            ColumnData col = computeColumn(tx, seed);
            int s = col.surface;
            bool ocean = col.ocean;

            // 1. Topo == tile do bioma (ou neve, ou boca).
            {
                Tile top = tileType(tx, s, seed);
                assert(top == tileType(tx, s, seed, col)); // sobrecarga idêntica
                Biome b = pickBiome(col.temperature, col.humidity,
                                    ocean, isCoastal(s));
                Tile expected = wormMouth(tx, s, seed)
                              ? (ocean ? Tile::Water : Tile::Air)
                              : snowcap(s)            ? Tile::Snow
                                                      : biomeTopTile(b);
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
                    // 3. pedra ou minério (pickOre só troca Stone por veio)
                    assert(isSolid(t) && t != Tile::Dirt && t != Tile::Sand);
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
