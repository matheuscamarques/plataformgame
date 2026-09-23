/**
 * @file tests/test_trenches.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava fossa e monte com máscara estável em 128.
 * @details Cobre Generation e oceano, roda com make test que compila em build/tests/test_trenches.
 */

#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "world/Generation.h"

// Fossa + monte submarino.
//
// NOTA DE CALIBRAGEM: máscara em /1024 deixava a fossa a 2000+ tiles
// em 2/3 seeds; em /384 a taxa efetiva variava 0-3% por anti-correlação
// acidental com o oceano. Em /128 a taxa efetiva é 1.4-1.8% estável.
// Frequência da máscara CASA com a escala do oceano (lagoas ~50 tiles).
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int trenchCols = 0, n = 0, effTrench = 0, seaM = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, seed);
            bool ocean = isOceanColumn(tx, seed);
            n++;

            float tr = trenchMask(tx, 0, seed);
            if (tr > TRENCH_THRESHOLD) {
                trenchCols++;
                // Só afirma em oceano: fora d'água a fossa é ignorada.
                if (ocean) {
                    effTrench++;
                    assert(s > SEA_LEVEL); // continua oceano
                    assert(s <= 29 + TRENCH_DEPTH_MAX);
                }
            }

            // Monte submarino nunca emerge.
            float p = peakMask(tx, 0, seed);
            if (ocean && p > SEAMOUNT_THRESHOLD) {
                seaM++;
                assert(s >= SEA_LEVEL + 2);
            }

            // Determinismo das máscaras.
            assert(trenchMask(tx, 0, seed) == tr);
            assert(peakMask(tx, 0, seed) == p);
        }
        float fTrench = (float)trenchCols / n;
        std::printf("seed=%u fossas=%.4f efetivas=%d semounts=%d\n",
                    seed, fTrench, effTrench, seaM);
        assert(fTrench > 0.005f && fTrench < 0.15f); // faixas, não tapete
        assert(effTrench > 0); // fossa de verdade em algum oceano
    }

    // Presença em range largo: monte submarino é raro por coluna e o
    // ±2000 pode não conter nenhum (1337: mais próximo em 2633).
    for (uint32_t seed : {1337u, 999u, 42u}) {
        int sm = 0;
        for (int tx = -4000; tx < 4000; tx++) {
            if (!isOceanColumn(tx, seed)) continue;
            if (peakMask(tx, 0, seed) > SEAMOUNT_THRESHOLD) sm++;
        }
        assert(sm > 0);
    }

    std::printf("trenches test OK\n");
    return 0;
}
