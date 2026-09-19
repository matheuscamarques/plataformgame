#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"
#include "support/World/Block.h"

int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        // Guard direto: nunca caverna em/até surfaceY + 2,
        // EXCETO boca de verme (única exceção, testada em test_worms).
        for (int tx = -300; tx < 300; tx += 7) {
            int s = surfaceHeight(tx, seed);
            float m = mountainMask(tx, 0, seed);
            assert(!isCave(tx, s, seed, s, m) || wormMouth(tx, s, seed));
            assert(!isCave(tx, s + 1, seed, s, m) || wormMouth(tx, s + 1, seed));
            assert(!isCave(tx, s + 2, seed, s, m) || wormMouth(tx, s + 2, seed));
            assert(isCave(tx, s, seed, s, m) == isCave(tx, s, seed, s, m)); // determinístico
        }

        // Invariante: nenhum buraco em wy <= surface + 2, exceto boca
        // (ar seco ou água se a boca está molhada).
        for (int tx = -400; tx < 400; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s - 4; ty <= s + 2; ty++) {
                Tile t = tileType(tx, ty, seed);
                if (ty >= s) {
                    bool mouthWater = t == Tile::Water && wormMouth(tx, ty, seed);
                    assert(isSolid(t) || mouthWater ||
                           (t == Tile::Air && wormMouth(tx, ty, seed)));
                }
                assert(tileType(tx, ty, seed) == t); // determinístico
            }
        }

        // Densidade cresce com a profundidade (só colunas planas:
        // montanha tem teto próprio no test_cave_mountain).
        // Range largo (+-2000): features de 64px correlacionam tiles
        // vizinhos; range estreito mede um blob só, não a densidade.
        int shallow = 0, deep = 0, nShallow = 0, nDeep = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            if (mountainMask(tx, 0, seed) > MOUNTAIN_THRESHOLD) continue;
            int s = surfaceHeight(tx, seed);
            for (int d = 0; d <= 10; d++) {
                nShallow++;
                // buraco = não-sólido (ar ou caverna inundada)
                if (!isSolid(tileType(tx, s + 3 + d, seed))) shallow++;
            }
            for (int d = 15; d <= 25; d++) {
                nDeep++;
                if (!isSolid(tileType(tx, s + 3 + d, seed))) deep++;
            }
        }
        float fShallow = (float)shallow / nShallow;
        float fDeep = (float)deep / nDeep;
        std::printf("seed=%u raso=%.3f fundo=%.3f\n", seed, fShallow, fDeep);
        assert(fShallow < fDeep);
        assert(fDeep < 0.60f); // fundo denso, não oco
    }

    std::printf("cave test OK\n");
    return 0;
}
