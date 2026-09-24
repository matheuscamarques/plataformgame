/**
 * @file tests/test_physics_2d.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava Physics2D.moveBody (Fase 2 da fundação).
 * @details Cobre queda até pousar, anti-túnel em alta velocidade, repouso grounded e delegação isSolidTile, roda com make test que compila em build/tests/test_physics_2d.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "physics/Physics2D.h"
#include "world/World.h"

namespace {

// Gera terreno em torno de x e acha o topo do chão com sonda.
float findGround(support::World &w, physics::Physics2D &ph, float x) {
    w.update(static_cast<int>(std::floor(x / core::kBlockSize)), 8);
    physics::Body probe;
    probe.position = {x, 0.f};
    probe.halfExtents = {5.f, 5.f};
    for (int i = 0; i < 2000; ++i) {
        auto r = ph.moveBody(probe, 1.f / 30.f, 600.f);
        if (r.hitGround) break;
    }
    assert(probe.grounded);
    return probe.position.y + probe.halfExtents.y; // base = topo do sólido
}

} // namespace

int main() {
    support::World world(1337u);
    physics::Physics2D ph(world);
    const float groundTop = findGround(world, ph, 100.f);
    assert(groundTop > 100.f); // caiu de verdade, não spawnou no chão

    { // FallLands (queda integra, pousa, flags)
        physics::Body b;
        b.position = {100.f, groundTop - 300.f};
        b.halfExtents = {5.f, 5.f};
        bool landed = false;
        for (int i = 0; i < 600; ++i) {
            auto r = ph.moveBody(b, 1.f / 30.f, 600.f);
            if (r.hitGround) {
                landed = true;
                assert(b.grounded && b.velocity.y == 0.f);
                break;
            }
        }
        assert(landed);
        assert(std::fabs((b.position.y + b.halfExtents.y) - groundTop) < 6.f);
    }
    { // NoTunnel (vy 6000 num passo só não atravessa o chão)
        physics::Body b;
        b.position = {100.f, groundTop - 120.f};
        b.halfExtents = {5.f, 5.f};
        b.velocity = {0.f, 6000.f}; // 200px no passo (4 tiles)
        auto r = ph.moveBody(b, 1.f / 30.f, 0.f);
        assert(r.hitGround && b.grounded);
        assert(b.position.y + b.halfExtents.y <= groundTop + 6.f);
    }
    { // RestStaysGrounded (parado sobre sólido continua grounded)
        physics::Body b;
        b.position = {100.f, groundTop - 5.f};
        b.halfExtents = {5.f, 5.f};
        auto r = ph.moveBody(b, 1.f / 30.f, 600.f);
        assert(b.grounded && !r.hitGround); // já estava: sem novo impacto
        assert(b.velocity.y == 0.f);
    }
    { // StaticIgnoresStep (corpo estático não integra)
        physics::Body b;
        b.position = {100.f, groundTop - 300.f};
        b.isStatic = true;
        ph.moveBody(b, 1.f / 30.f, 600.f);
        assert(b.position.y == groundTop - 300.f && !b.grounded);
    }
    { // DelegatesTileQuery (backend == World)
        for (int ty = 0; ty < 12; ++ty)
            assert(ph.isSolidTile(2, ty) == world.isSolid(2, ty));
    }

    std::printf("physics_2d test OK (ground=%.1f)\n", groundTop);
    return 0;
}
