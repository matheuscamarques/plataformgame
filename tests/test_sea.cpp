#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Commit mar: oceano preenche sem mexer na altura.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int oceanCols = 0, n = 0, sandTops = 0, oceanTops = 0;

        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, seed);
            bool ocean = isOceanColumn(tx, seed);
            n++;
            if (ocean) oceanCols++;

            // Consistência: isOceanColumn == surface > SEA_LEVEL.
            assert(ocean == (s > SEA_LEVEL));

            // Topo: areia na costa (inclui todo oceano), bioma fora dela.
            Tile top = tileType(tx, s, seed);
            if (isCoastal(s)) {
                // areia, exceto boca (ar em terra, água em oceano)
                bool mouthOk = wormMouth(tx, s, seed) &&
                               top == (ocean ? Tile::Water : Tile::Air);
                assert(top == Tile::Sand || mouthOk);
                if (top == Tile::Sand) sandTops++;
            } else {
                Biome b = pickBiome(temperature(tx, s, seed), humidity(tx, s, seed),
                                    ocean, false);
                Tile expected = wormMouth(tx, s, seed)
                              ? (ocean ? Tile::Water : Tile::Air)
                              : snowcap(s)            ? Tile::Snow
                                                      : biomeTopTile(b);
                assert(top == expected);
            }
            if (ocean) {
                // oceano é sempre costeiro (s <= SEA+3 aqui): areia ou boca d'água
                if (top == Tile::Sand) oceanTops++;
                else assert(top == Tile::Water && wormMouth(tx, s, seed));
            }

            // Céu acima do mar: ar. Banda d'água: Tile::Water (vira entidade).
            if (ocean) {
                for (int ty = s - 30; ty < SEA_LEVEL; ty++) {
                    assert(tileType(tx, ty, seed) == Tile::Air);
                }
                for (int ty = SEA_LEVEL; ty < s; ty++) {
                    assert(tileType(tx, ty, seed) == Tile::Water);
                }
            }

            // Terra segue igual: topo sólido do bioma (ou neve 8 nos picos),
            // ar acima (ou ilha).
            if (!ocean) {
                Biome b = pickBiome(temperature(tx, s, seed), humidity(tx, s, seed),
                                    false, isCoastal(s));
                Tile expected = wormMouth(tx, s, seed)
                              ? (ocean ? Tile::Water : Tile::Air)
                              : snowcap(s)            ? Tile::Snow
                                                      : biomeTopTile(b);
                assert(top == expected);
                for (int ty = s - 10; ty < s; ty++) {
                    Tile t = tileType(tx, ty, seed);
                    assert(t == Tile::Air || t == Tile::IslandPlatform);
                }
            }
        }

        float fOcean = (float)oceanCols / n;
        std::printf("seed=%u oceano=%.3f areia=%d\n", seed, fOcean, sandTops);
        assert(fOcean > 0.10f && fOcean < 0.30f); // calibrado: L=26 => 14-21%
        assert(oceanTops > 0);
        assert(sandTops >= oceanTops); // praia existe fora do oceano também
    }

    std::printf("sea test OK\n");
    return 0;
}
