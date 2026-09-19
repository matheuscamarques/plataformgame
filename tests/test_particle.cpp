#include <cassert>
#include <cstdio>
#include "support/ParticleSystem.h"
#include "support/GameContext.h"

// Partículas: spawn, expiração, pool sob stress, faísca curta.
int main() {
    using namespace support;

    { // SpawnTileBreakCreatesDebris
        ParticleSystem ps;
        ps.spawnTileBreak({100.f, 100.f}, 0, 0, 0);
        assert(ps.activeDebris() > 0);
    }
    { // OreMaskPopcountDrivesOreParticles: 4 bits -> 8 de minério + poeira
        ParticleSystem ps;
        uint8_t mask = 0b00001111;
        ps.spawnTileBreak({0.f, 0.f}, 0, 2, mask);
        assert(ps.activeDebris() >= 12u);
    }
    { // ParticlesExpireAfterLifetime (world null ok: sem colisão não consulta)
        ParticleSystem ps;
        ps.spawnTileBreak({0.f, 0.f}, 0, 0, 0);
        GameContext ctx{};
        for (int i = 0; i < 100; ++i) ps.tick(0.1f, ctx); // 10s
        assert(ps.activeDebris() == 0u);
    }
    { // PoolDoesNotLeakUnderStress (512 debris / 4096 dust, sem crash)
        ParticleSystem ps;
        for (int i = 0; i < 2000; ++i)
            ps.spawnTileBreak({0.f, 0.f}, 0, 0, 0xFF);
        assert(ps.activeDebris() <= 512u);
        assert(ps.activeDust() <= 4096u);
    }
    { // HitSparkShortLived
        ParticleSystem ps;
        ps.spawnHitSpark({50.f, 50.f});
        assert(ps.activeDust() > 0);
    }

    std::printf("particle test OK\n");
    return 0;
}
