#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"
#include "support/World/Block.h"

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
            for (int ty = s - 50; ty < s + 150 && ty < WORLD_BOTTOM; ty += 2) {
                // Invariante só vale onde o guard passa (ty >= s+3):
                // na faixa do guard, uniform pode ser true e o tile sólido.
                if (ty < s + 3) continue;
                float depth = (float)(ty - s) / CAVE_DEPTH_RANGE;
                if (depth > 1.f) depth = 1.f;
                bool uniform = caveNoise(tx, ty, seed) > CAVE_BASE - depth * CAVE_DEPTH_FALLOFF;
                Tile tile = tileType(tx, ty, seed);
                // caverna-uniforme vira vazio (ar em cima, água inundada embaixo)
                if (uniform) assert(!isSolid(tile));
                if (!uniform && !isSolid(tile)) strict++;
            }
        }
        // Validado em 3 seeds: strict fica em 376-908 (bedrock corta a
        // cauda funda da amostra). Folga 2x; se CAVE_MOUNTAIN_BONUS mudar,
        // revalida aqui.
        assert(strict > 200); // bônus abre cavernas de verdade
        std::printf("seed=%u monotonicidade OK (strict=%d)\n", seed, strict);

        // 2) Guard intacto nos picos: topo de montanha nunca é oco
        // (exceto boca, seca ou molhada).
        for (int tx = -1200; tx < 1200; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s; ty <= s + 2; ty++) {
                Tile t = tileType(tx, ty, seed);
                bool mouthWater = t == Tile::Water && wormMouth(tx, ty, seed);
                assert(isSolid(t) || mouthWater ||
                       (t == Tile::Air && wormMouth(tx, ty, seed)));
                assert(tileType(tx, ty, seed) == t);
            }
        }

        // 3) Teto GLOBAL no fundo (todas as colunas): rocha continua
        // sendo a maioria. Medido 0.34-0.36; 0.55 tem margem folgada.
        // Vazios contam ar + água inundada (ambos são "não-rocha").
        int n = 0, caves = 0;
        for (int tx = -4000; tx < 4000; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int d = 25; d <= 100; d++) {
                n++;
                if (!isSolid(tileType(tx, s + 3 + d, seed))) caves++;
            }
        }
        float fDeep = (float)caves / n;
        std::printf("seed=%u fundo-global=%.3f\n", seed, fDeep);
        assert(fDeep <= 0.55f);
    }

    std::printf("cave mountain test OK\n");
    return 0;
}
