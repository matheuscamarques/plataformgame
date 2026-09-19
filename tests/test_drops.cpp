#include <cassert>
#include <cstdio>
#include "entities/player/player.h"
#include "support/DropSystem.h"
#include "support/GameContext.h"

// Drops: spawn, expiração sem player, coleta, magnetismo, teto do pool.
int main() {
    using namespace support;

    { // SpawnsActiveOrb
        DropSystem ds;
        auto *o = ds.spawnXP({100.f, 100.f});
        assert(o != nullptr && ds.activeCount() == 1u);
    }
    { // OrbExpiresAfterLifetime (sem player: só envelhece)
        DropSystem ds;
        ds.spawnXP({100.f, 100.f});

        GameContext ctx{}; // sem player
        for (int i = 0; i < 200; ++i) ds.tick(0.1f, ctx); // 20s > 12s
        assert(ds.activeCount() == 0u);
    }
    { // CollectIncrementsCounter (orb no centro do player)
        DropSystem ds;
        Player mock; // 50x50 em (0,0) → centro (25,25)
        ds.spawnXP({25.f, 25.f});

        GameContext ctx{};
        ctx.player = &mock;

        ds.tick(0.016f, ctx);
        assert(ds.totalCollected() == 1);
        assert(ds.activeCount() == 0u);
    }
    { // MagnetPullsOrb (50px: dentro do raio 64, fora da coleta 12)
        DropSystem ds;
        Player mock;
        auto *o = ds.spawnXP({75.f, 25.f});
        assert(o != nullptr);

        GameContext ctx{};
        ctx.player = &mock;

        const float x0 = o->pos.x;
        ds.tick(0.1f, ctx);
        assert(o->magnetized);
        assert(o->pos.x < x0); // voou em direção ao player
    }
    { // PoolCapsAtMax (128, descarta excedente sem crash)
        DropSystem ds;
        for (int i = 0; i < 500; ++i) ds.spawnXP({0.f, 0.f});
        assert(ds.activeCount() <= 128u);
    }

    std::printf("drops test OK\n");
    return 0;
}
