/**
 * @file tests/test_sweep.cpp
 * @brief Teste headless da Fase E: geometria do SweepArc (cone+anel).
 * @details Puro, sem GL. Roda com make test em build/tests/test_sweep.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/Combat/SweepArc.h"

static support::SweepArc eastArc() {
    support::SweepArc a;
    a.origin = {0.f, 0.f};
    a.centerAngle = 0.f; // E
    a.halfWidth = support::kSweepHalfWidth;
    a.rInner = 10.f;
    a.rOuter = 50.f;
    a.empty = false;
    return a;
}

int main() {
    using support::sweepHitsCircle;
    using support::SweepArc;
    constexpr float kPi = 3.14159265f;

    { // ConeE (dentro acerta; fora/atrás/dentro-morto/long e vazio não)
        const SweepArc a = eastArc();
        assert(sweepHitsCircle(a, {30.f, 0.f}, 0.f));
        assert(!sweepHitsCircle(a, {30.f, 25.f}, 0.f)); // 39.8° > 35°
        assert(!sweepHitsCircle(a, {-30.f, 0.f}, 0.f)); // atrás
        assert(!sweepHitsCircle(a, {5.f, 0.f}, 0.f));   // zona morta
        assert(!sweepHitsCircle(a, {60.f, 0.f}, 0.f));  // além da ponta
        SweepArc e;
        assert(!sweepHitsCircle(e, {30.f, 0.f}, 0.f)); // vazio nunca
    }
    { // MargemCobreTamanho (graze de corpo grande conta)
        const SweepArc a = eastArc();
        // 30° fora do cone de 22°, mas raio 8 a dist 30: margem 15.3°.
        assert(!sweepHitsCircle(a, {25.9808f, 15.f}, 0.f));
        assert(sweepHitsCircle(a, {25.9808f, 15.f}, 8.f));
        // Ponta encostando: dist 55 = R+5 com raio 5.
        assert(sweepHitsCircle(a, {55.f, 0.f}, 5.f));
        assert(!sweepHitsCircle(a, {55.f, 0.f}, 4.f));
    }
    { // DiagonalNE (cone aponta p/ cima-direita em screen-space)
        SweepArc a = eastArc();
        a.centerAngle = -kPi / 4.f;
        assert(sweepHitsCircle(a, {20.f, -20.f}, 0.f));
        assert(!sweepHitsCircle(a, {20.f, 20.f}, 0.f));
    }
    { // WrapPi (centro em 171.9°: ponto em -171.9° está a 16°)
        SweepArc a = eastArc();
        a.centerAngle = 3.f;
        const float r = 30.f;
        assert(sweepHitsCircle(a,
                               {r * std::cos(-3.f), r * std::sin(-3.f)},
                               0.f));
        assert(!sweepHitsCircle(a, {30.f, 0.f}, 0.f));
    }
    { // EmCimaDoOmbro (dist 0: acerta, sem divisão por zero)
        const SweepArc a = eastArc();
        assert(sweepHitsCircle(a, {0.f, 0.f}, 0.f));
    }
    { // ZonaMortaComCorpo (círculo todo dentro: fora; encostando: dentro)
        const SweepArc a = eastArc();
        assert(!sweepHitsCircle(a, {5.f, 0.f}, 2.f)); // 5+2 < 10
        assert(sweepHitsCircle(a, {5.f, 0.f}, 6.f));  // 5+6 > 10
    }

    std::printf("sweep test OK\n");
    return 0;
}
