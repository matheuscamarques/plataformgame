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

            // Topo: areia na costa (inclui todo oceano), 4 fora dela.
            int top = tileType(tx, s, seed);
            if (isCoastal(s)) {
                assert(top == 6);
                sandTops++;
            } else {
                assert(top == 4);
            }
            if (ocean) {
                oceanTops++;
                assert(top == 6); // oceano é sempre costeiro (s <= SEA+3 aqui)
            }

            // Nenhum sólido acima do nível do mar em coluna de oceano.
            if (ocean) {
                for (int ty = s - 30; ty < SEA_LEVEL; ty++) {
                    assert(tileType(tx, ty, seed) == 0);
                }
                // Banda d'água: vazia de sólido (vira entidade água).
                for (int ty = SEA_LEVEL; ty < s; ty++) {
                    assert(tileType(tx, ty, seed) == 0);
                }
            }

            // Terra segue igual: topo sólido, ar acima (ou plataforma rara).
            if (!ocean) {
                assert(top == 4 || top == 6);
                for (int ty = s - 10; ty < s; ty++) {
                    int t = tileType(tx, ty, seed);
                    assert(t == 0 || t == 2);
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
