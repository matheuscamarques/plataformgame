#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Commit 3: caverna modulada por montanha.
//
// NOTA DE CALIBRAGEM (lição da autocorrelação, 4ª vez): densidade por
// CLASSE (só colunas fortes) é loteria — colunas fortes são ~1% e
// clusterizadas, e blobs de caverna têm 64px. ±4000 colunas medem ~100
// colunas correlacionadas, não 8000 amostras. Por isso este teste
// afirma MECANISMO (monotonicidade, sem variância) e usa agregado
// GLOBAL (todas as colunas, estável 0.34-0.36) para o teto.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        // 1) Monotonicidade: caverna-uniforme => caverna-modulada.
        // Sem variância: vale ponto a ponto ou o código está errado.
        int strict = 0;
        for (int tx = -2000; tx < 2000; tx += 2) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s - 50; ty < s + 150; ty += 2) {
                // Invariante só vale onde o guard passa (ty >= s+3):
                // na faixa do guard, uniform pode ser true e o tile sólido.
                if (ty < s + 3) continue;
                float depth = (float)(ty - s) / 40.0f;
                if (depth > 1.f) depth = 1.f;
                bool uniform = caveNoise(tx, ty, seed) > CAVE_BASE - depth * CAVE_DEPTH_FALLOFF;
                int tile = tileType(tx, ty, seed);
                if (uniform) assert(tile == 0);
                if (!uniform && tile == 0) strict++;
            }
        }
        // Validado em 3 seeds: strict fica em 1900-3300. Folga 3x;
        // se CAVE_MOUNTAIN_BONUS mudar, revalida aqui.
        assert(strict > 1000); // bônus abre cavernas de verdade
        std::printf("seed=%u monotonicidade OK (strict=%d)\n", seed, strict);

        // 2) Guard intacto nos picos: topo de montanha nunca é oco.
        for (int tx = -1200; tx < 1200; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s; ty <= s + 2; ty++) {
                assert(tileType(tx, ty, seed) != 0);
                assert(tileType(tx, ty, seed) == tileType(tx, ty, seed));
            }
        }

        // 3) Teto GLOBAL no fundo (todas as colunas): rocha continua
        // sendo a maioria. Medido 0.34-0.36; 0.55 tem margem folgada.
        int n = 0, caves = 0;
        for (int tx = -4000; tx < 4000; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int d = 25; d <= 100; d++) {
                n++;
                if (tileType(tx, s + 3 + d, seed) == 0) caves++;
            }
        }
        float fDeep = (float)caves / n;
        std::printf("seed=%u fundo-global=%.3f\n", seed, fDeep);
        assert(fDeep <= 0.55f);
    }

    std::printf("cave mountain test OK\n");
    return 0;
}
