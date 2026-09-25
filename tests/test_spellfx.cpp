/**
 * @file tests/test_spellfx.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless do SpellFX (lógica sem GL).
 * @details Cobre burst de cura (spawns + expira + clear) e heat bounds; draw fica p/ prova GL, roda com make test que compila em build/tests/test_spellfx.
 */

#include <cassert>
#include <cstdio>

#include "support/Effects/SpellFX.h"

int main() {
    { // HealBurstSpawnsAndExpires (10 faíscas, somem em 1s)
        support::SpellFX fx;
        assert(fx.activeSparks() == 0u);
        fx.burstHeal({100.f, 100.f});
        assert(fx.activeSparks() == 10u);
        for (int i = 0; i < 30; ++i) fx.tick(1.f / 30.f);
        assert(fx.activeSparks() == 0u); // 0.7s > vida máxima
    }
    { // ClearResets (restart limpa faíscas e calor)
        support::SpellFX fx;
        fx.burstHeal({0.f, 0.f});
        fx.tick(1.f / 30.f);
        fx.clear();
        assert(fx.activeSparks() == 0u);
        assert(fx.heatAt(8, 15) == 0.f);
    }
    { // HeatBoundsSafe (fora da área = 0, sem crash)
        support::SpellFX fx;
        assert(fx.heatAt(-1, 0) == 0.f);
        assert(fx.heatAt(0, 16) == 0.f);
        assert(fx.heatAt(16, 0) == 0.f);
        assert(fx.heatAt(8, 8) == 0.f); // sem chama acesa
    }

    std::printf("spellfx test OK\n");
    return 0;
}
