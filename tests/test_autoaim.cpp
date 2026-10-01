/**
 * @file tests/test_autoaim.cpp
 * @brief Trava o aim-bot: vivo mais próximo no cone do facing.
 * @details Slime 40x30 (centro = spawn+{20,15}). Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include "support/Enemies/EnemySystem.h"

int main() {
    using namespace support;
    const core::Vec2f from{0.f, 0.f};

    { // AheadAboveBeatsBehind (voador à frente ganha do de trás)
        EnemySystem e;
        e.spawn("slime", 80.f, -95.f);  // centro (100,-80)
        e.spawn("slime", -70.f, -15.f); // centro (-50,0)
        Enemy* t = e.nearestAhead(from, 1);
        assert(t != nullptr && t->body.getX() == 80.f);
    }
    { // BehindOnlyIsNull (nada à frente: mira manual)
        EnemySystem e;
        e.spawn("slime", -70.f, -15.f);
        assert(e.nearestAhead(from, 1) == nullptr);
    }
    { // DeadSkipped (morto não é alvo: pega o próximo)
        EnemySystem e;
        e.spawn("slime", 40.f, -15.f);  // centro (60,0)
        e.spawn("slime", 100.f, -15.f); // centro (120,0)
        e.forEach([](Enemy& s) {
            if (s.body.getX() == 40.f) (void)s.resources.takeDamage(9999);
        });
        Enemy* t = e.nearestAhead(from, 1);
        assert(t != nullptr && t->body.getX() == 100.f);
    }
    { // DestroyPendingSkipped (marcado p/ varredura não é alvo)
        EnemySystem e;
        e.spawn("slime", 40.f, -15.f);
        e.forEach([&](Enemy& s) { e.markForDestroy(s); });
        assert(e.nearestAhead(from, 1) == nullptr);
    }
    { // OutOfRange (além de 300: null; range custom vale)
        EnemySystem e;
        e.spawn("slime", 380.f, -15.f); // centro (400,0)
        assert(e.nearestAhead(from, 1) == nullptr);
        assert(e.nearestAhead(from, 1, 8.f, 500.f) != nullptr);
        assert(e.nearestAhead(from, 1, 8.f, 50.f) == nullptr);
    }
    { // OverheadWithinSlop (dx=0 sobre a cabeça: entra)
        EnemySystem e;
        e.spawn("slime", -20.f, -115.f); // centro (0,-100)
        assert(e.nearestAhead(from, 1) != nullptr);
    }
    { // SlopBoundary (dx=-8: fora; -7.9: dentro)
        EnemySystem e;
        e.spawn("slime", -28.f, -15.f); // centro x=-8
        assert(e.nearestAhead(from, 1) == nullptr);
        EnemySystem e2;
        e2.spawn("slime", -27.9f, -15.f);
        assert(e2.nearestAhead(from, 1) != nullptr);
    }
    { // FacingLeft (cone espelha: -x entra, +x não)
        EnemySystem e;
        e.spawn("slime", -120.f, -95.f); // centro (-100,-80)
        e.spawn("slime", 80.f, -15.f);   // centro (100,0)
        Enemy* t = e.nearestAhead(from, -1);
        assert(t != nullptr && t->body.getX() == -120.f);
        EnemySystem e2;
        e2.spawn("slime", 80.f, -15.f);
        assert(e2.nearestAhead(from, -1) == nullptr);
    }

    std::printf("autoaim test OK\n");
    return 0;
}
