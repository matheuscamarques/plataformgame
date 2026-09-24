/**
 * @file tests/test_physics_raycast.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava Physics2D.raycast DDA (Fase 2).
 * @details Cobre acerto vertical com normal, vazio no céu, origem dentro de sólido e raio zero, roda com make test que compila em build/tests/test_physics_raycast.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "physics/Physics2D.h"
#include "world/World.h"

int main() {
    support::World world(1337u);
    physics::Physics2D ph(world);
    world.update(2, 8);

    // Acha o chão em x=100 com sonda vertical.
    auto down = ph.raycast({100.f, 0.f}, {100.f, 20000.f});
    assert(down.hit);
    const float groundTop = down.point.y;
    assert(down.normal.x == 0.f && down.normal.y == -1.f);
    assert(down.distance > 100.f);

    { // VerticalHitsGround (ponto no topo, distância coerente)
        auto h = ph.raycast({100.f, groundTop - 500.f},
                            {100.f, groundTop + 500.f});
        assert(h.hit);
        assert(std::fabs(h.point.y - groundTop) < 2.f);
        assert(h.normal.y == -1.f);
        assert(std::fabs(h.distance - 500.f) < 2.f);
    }
    { // SkyIsEmpty (raio horizontal no céu não acerta)
        auto h = ph.raycast({0.f, 10.f}, {5000.f, 10.f});
        assert(!h.hit);
        assert(std::fabs(h.distance - 5000.f) < 1.f);
    }
    { // InsideSolidHitsAtZero (origem dentro de sólido)
        auto h = ph.raycast({100.f, groundTop + 100.f},
                            {100.f, groundTop + 500.f});
        assert(h.hit && h.distance == 0.f);
    }
    { // ZeroLengthMisses (raio degenerado nunca acerta)
        auto h = ph.raycast({100.f, 100.f}, {100.f, 100.f});
        assert(!h.hit && h.distance == 0.f);
    }

    std::printf("physics_raycast test OK (ground=%.1f)\n", groundTop);
    return 0;
}
