#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Effects/ThrowSystem.h"

// S6: tryThrow com cooldown, inventário, facing e pool cheio.
int main() {
    using namespace support;

    { // ThrowsWhenReady (inventário cai, cooldown arma, spawn ativo)
        Player p;
        ThrowSystem ts;
        assert(p.dynamiteCount == 999 && p.throwCooldown.ready());
        assert(p.tryThrow(ts));
        assert(p.dynamiteCount == 998);
        assert(!p.throwCooldown.ready());
        assert(ts.activeCount() == 1u);
    }
    { // CooldownBlocksDoubleThrow (2º imediato falha sem gastar)
        Player p;
        ThrowSystem ts;
        assert(p.tryThrow(ts));
        assert(!p.tryThrow(ts));
        assert(p.dynamiteCount == 998 && ts.activeCount() == 1u);
    }
    { // ReadyAfterHalfSecond (14 ticks de 1/30 bloqueiam; folga libera)
        Player p;
        ThrowSystem ts;
        assert(p.tryThrow(ts));
        for (int i = 0; i < 14; ++i) p.throwCooldown.tick(1.f / 30.f);
        assert(!p.throwCooldown.ready());
        p.throwCooldown.tick(1.f); // folga: evita resíduo float no limite exato
        assert(p.throwCooldown.ready() && p.tryThrow(ts));
        assert(p.dynamiteCount == 997);
    }
    { // EmptyInventoryThrowsNothing
        Player p;
        ThrowSystem ts;
        p.dynamiteCount = 0;
        assert(!p.tryThrow(ts));
        assert(ts.activeCount() == 0u);
    }
    { // FullPoolKeepsInventory (pool cheio: sem spawn, sem gasto)
        Player p;
        ThrowSystem ts;
        for (int i = 0; i < 64; ++i) ts.throwItem({0.f, 0.f}, {0.f, 0.f});
        assert(ts.activeCount() == 64u);
        assert(!p.tryThrow(ts));
        assert(p.dynamiteCount == 999 && p.throwCooldown.ready());
    }
    { // FacingDirectsThrow (esquerda = vx negativo)
        Player p;
        ThrowSystem ts;
        p.facing = -1;
        assert(p.tryThrow(ts));
        bool leftward = false;
        ts.forEachActive([&](const Throwable &t) { leftward = t.vel.x < 0.f; });
        assert(leftward);
    }

    std::printf("player throw test OK\n");
    return 0;
}
