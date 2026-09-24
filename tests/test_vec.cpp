/**
 * @file tests/test_vec.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava core::Vec2 (Fase 1 da fundação).
 * @details Cobre operadores, length/normalized/dot e round-trip com SFML via VecSfml, roda com make test que compila em build/tests/test_vec.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "core/Vec.h"
#include "core/VecSfml.h"

int main() {
    using core::Vec2f;
    using core::Vec2i;

    { // Ops (soma, sub, escalar, composto, igualdade)
        const Vec2f a{1.f, 2.f}, b{3.f, 4.f};
        assert(a + b == Vec2f(4.f, 6.f));
        assert(b - a == Vec2f(2.f, 2.f));
        assert(a * 2.f == Vec2f(2.f, 4.f));
        assert(b / 2.f == Vec2f(1.5f, 2.f));
        Vec2f c{0.f, 0.f};
        c += a;
        c -= Vec2f(1.f, 1.f);
        assert(c == Vec2f(0.f, 1.f));
        assert(a != b);
        const Vec2i i{3, 4};
        assert(i.lengthSq() == 25);
    }
    { // LengthNormalizedDot (3-4-5, zero seguro)
        const Vec2f v{3.f, 4.f};
        assert(v.lengthSq() == 25.f && v.length() == 5.f);
        const auto n = v.normalized();
        assert(std::fabs(n.x - 0.6f) < 1e-5f && std::fabs(n.y - 0.8f) < 1e-5f);
        assert(v.dot(Vec2f(1.f, 0.f)) == 3.f);
        const auto z = Vec2f(0.f, 0.f).normalized();
        assert(z == Vec2f(0.f, 0.f));
    }
    { // SfmlRoundTrip (fronteira sem perda)
        const Vec2f v{1.5f, -2.25f};
        const Vec2f back = core::fromSf(core::toSf(v));
        assert(back == v);
        const Vec2i w = core::fromSf(sf::Vector2i(7, -3));
        assert(w == Vec2i(7, -3));
        assert(core::toSf(w) == sf::Vector2i(7, -3));
    }

    std::printf("vec test OK\n");
    return 0;
}
