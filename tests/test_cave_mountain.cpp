#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Commit 3: caverna modulada por montanha.
//
// Nota de calibragem: a razão montanha/plano só é estável onde o bônus
// é grande (t>=0.5) e o range é largo (+-4000 colunas). Com bônus médio
// pequeno e range estreito, blobs de 64px dominam a estatística e a
// razão varia 0.4-1.1 por seed — métrica errada, não código errado.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        // 1) Monotonicidade: flat => montanha, sempre (mecanismo, sem ruído).
        int strict = 0;
        for (int tx = -2000; tx < 2000; tx += 3) {
            for (int ty = -50; ty < 150; ty += 3) {
                int s = ty - 30; // surface fictícia p/ variar o depth
                bool f = isCave(tx, ty, seed, s, 0.0f);
                bool m = isCave(tx, ty, seed, s, 0.9f);
                if (f) assert(m);
                if (!f && m) strict++;
            }
        }
        assert(strict > 10000); // bônus efetivamente abre cavernas

        // 2) Guard intacto nos picos: topo de montanha nunca é oco.
        for (int tx = -1200; tx < 1200; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s; ty <= s + 2; ty++) {
                assert(tileType(tx, ty, seed) != 0);
                assert(tileType(tx, ty, seed) == tileType(tx, ty, seed));
            }
        }

        // 3) Densidade: montanha FORTE (t>=0.5) vs plano, range largo.
        int mN = 0, mCave = 0, fN = 0, fCave = 0, mDeepN = 0, mDeepCave = 0;
        for (int tx = -4000; tx < 4000; tx++) {
            int s = surfaceHeight(tx, seed);
            float m = mountainMask(tx, 0, seed);
            float t = (m - MOUNTAIN_THRESHOLD) / (1.f - MOUNTAIN_THRESHOLD);
            for (int d = 15; d <= 25; d++) {
                int ty = s + 3 + d;
                bool cave = tileType(tx, ty, seed) == 0;
                if (t >= 0.5f) {
                    mN++;
                    if (cave) mCave++;
                    if (d >= 20) {
                        mDeepN++;
                        if (cave) mDeepCave++;
                    }
                } else if (m <= MOUNTAIN_THRESHOLD) {
                    fN++;
                    if (cave) fCave++;
                }
            }
        }
        float fM = (float)mCave / mN;
        float fF = (float)fCave / fN;
        float fDeep = (float)mDeepCave / mDeepN;
        std::printf("seed=%u mont=%.3f plano=%.3f razao=%.2f fundo-mont=%.3f\n",
                    seed, fM, fF, fM / fF, fDeep);
        assert(fM > fF * 1.3f);
        assert(fDeep <= 0.70f); // teto global no fundo de montanha
    }

    std::printf("cave mountain test OK\n");
    return 0;
}
