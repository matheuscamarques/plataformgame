/**
 * @file tests/test_throw.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava spawn, gravidade, fuse e teto do pool.
 * @details Cobre ThrowSystem e explosão, roda com make test que compila em build/tests/test_throw.
 */

#include <cassert>
#include <cstdio>
#include "support/Effects/ThrowSystem.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/GameContext.h"

// Throw: spawn, gravidade, fuse, teto do pool.
int main() {
    using namespace support;

    { // SpawnsActiveThrowable
        ThrowSystem ts;
        auto *t = ts.throwItem({0.f, 0.f}, {100.f, -200.f});
        assert(t != nullptr && ts.activeCount() == 1u);
    }
    { // GravityPullsDownward (sem mundo: sem colisão, só gravidade)
        ThrowSystem ts;
        auto *t = ts.throwItem({0.f, 0.f}, {0.f, 0.f});
        GameContext ctx{};
        ts.tick(0.1f, ctx);
        assert(t->vel.y > 0.f);
    }
    { // FuseExpires (com ExplosionSystem, sem alvos: remove igual)
        ThrowSystem ts;
        ExplosionSystem es;
        ts.setExplosionSystem(&es);
        auto *t = ts.throwItem({0.f, 0.f}, {0.f, 0.f});
        t->fuse = 0.1f;
        GameContext ctx{};
        std::vector<ExplosionTarget> empty;
        ctx.explosionTargets = &empty;
        ts.tick(0.2f, ctx);
        assert(ts.activeCount() == 0u);
    }
    { // PoolCapsAtMax (64, descarta excedente sem crash)
        ThrowSystem ts;
        for (int i = 0; i < 1000; ++i)
            ts.throwItem({0.f, 0.f}, {0.f, 0.f});
        assert(ts.activeCount() <= 64u);
    }

    std::printf("throw test OK\n");
    return 0;
}
