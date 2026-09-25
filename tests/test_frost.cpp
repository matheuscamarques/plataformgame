/**
 * @file tests/test_frost.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava frost status (Fase 2 elementais).
 * @details Cobre buildup, ativação, expiração, attackSpeedMult e swing lento, roda com make test que compila em build/tests/test_frost.
 */

#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"

int main() {
    { // BuildupActivates (acumula até o limiar, ativa 6s)
        Player p;
        const float th = p.statusThreshold();
        assert(p.frostTimer == 0.f);
        p.addFrost(th * 0.5f);
        assert(p.frostTimer == 0.f && p.frostBuildup == th * 0.5f);
        p.addFrost(th * 0.5f);
        assert(p.frostTimer == Player::kFrostDur && p.frostBuildup == 0.f);
    }
    { // NoReaccumulateWhileActive (ativo ignora buildup novo)
        Player p;
        p.addFrost(p.statusThreshold());
        assert(p.frostTimer > 0.f);
        p.addFrost(999.f);
        assert(p.frostBuildup == 0.f);
    }
    { // ExpiresInTick (6s a 30Hz; margem p/ erro float de 1/30)
        Player p;
        p.addFrost(p.statusThreshold());
        for (int i = 0; i < 200; ++i) p.tick();
        assert(p.frostTimer <= 0.f);
    }
    { // SlowMultWhileActive (0.7 ativo, 1.0 fora)
        Player p;
        assert(p.computeModifiers().attackSpeedMult == 1.f);
        p.addFrost(p.statusThreshold());
        assert(p.computeModifiers().attackSpeedMult == Player::kFrostSlow);
    }
    { // SwingRunsSlow (windup dura mais ticks sob frost)
        auto ticksToActive = [](bool frosted) {
            Player p;
            if (frosted) p.addFrost(p.statusThreshold());
            assert(p.startSwing());
            int n = 0;
            while (p.meleePhase == MeleePhase::Windup && n < 1000) {
                p.updateMelee(1.f / 30.f);
                ++n;
            }
            return n;
        };
        const int slow = ticksToActive(true);
        const int fast = ticksToActive(false);
        assert(fast > 0 && slow > fast); // 0.7x desce mais devagar
    }

    std::printf("frost test OK\n");
    return 0;
}
