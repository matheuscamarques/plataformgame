#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        // Guard direto: nunca caverna em/até surfaceY + 2.
        for (int tx = -300; tx < 300; tx += 7) {
            int s = surfaceHeight(tx, seed);
            assert(!isCave(tx, s, seed, s));
            assert(!isCave(tx, s + 1, seed, s));
            assert(!isCave(tx, s + 2, seed, s));
            assert(isCave(tx, s, seed, s) == isCave(tx, s, seed, s)); // determinístico
        }

        // Invariante que não pode falhar: nenhum buraco em wy <= surface + 2.
        for (int tx = -400; tx < 400; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s - 4; ty <= s + 2; ty++) {
                int t = tileType(tx, ty, seed);
                if (ty >= s) assert(t != 0);
                assert(tileType(tx, ty, seed) == t); // determinístico
            }
        }

        // Densidade cresce com a profundidade.
        int shallow = 0, deep = 0, nShallow = 0, nDeep = 0;
        for (int tx = -400; tx < 400; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int d = 0; d <= 10; d++) {
                nShallow++;
                if (tileType(tx, s + 3 + d, seed) == 0) shallow++;
            }
            for (int d = 15; d <= 25; d++) {
                nDeep++;
                if (tileType(tx, s + 3 + d, seed) == 0) deep++;
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
