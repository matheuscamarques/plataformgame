#include "SpawnSystem.h"

#include <cmath>

#include "../core/Random.h"
#include "../defines.h"
#include "../entities/player/player.h"
#include "EnemySystem.h"
#include "GameContext.h"
#include "World/Stratum.h"
#include "World/World.h"

namespace support {

int SpawnSystem::budgetForStratum(int s) {
    // Densidade cai com a profundidade (outros inimigos entram depois).
    switch (s) {
        case 0: return 3;
        case 1: return 4;
        case 2: return 4;
        case 3: return 3;
        case 4: return 3;
        case 5: return 2;
        default: return 2;
    }
}

void SpawnSystem::tick(float dt, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;

    // Economia primeiro: longe some, todo tick (barato, sem timer).
    const float px = p->getCenterX(), py = p->getCenterY();
    ctx.enemies->despawnFar(px, py, kDespawnRadius);

    timer_ += dt;
    if (timer_ < kInterval) return;
    timer_ = 0.f;

    if (ctx.enemies->count() >= kGlobalCap) return;
    const int ty = static_cast<int>(std::floor(py / BLOCK_SIZE));
    if (static_cast<std::size_t>(ctx.enemies->count()) >=
        static_cast<std::size_t>(budgetForStratum(stratumAt(ty))))
        return;

    // Posição: anel 600-1000px ao lado, depois chão para baixo.
    // Sem mundo (teste): spawna no ar na altura do player.
    const float side = core::randRange(0.f, 1.f) < 0.5f ? -1.f : 1.f;
    float sx = px + side * core::randRange(kSpawnMin, kSpawnMax);
    float sy = py + core::randRange(-100.f, 100.f);
    if (ctx.world) {
        int tx = static_cast<int>(std::floor(sx / BLOCK_SIZE));
        int tyy = static_cast<int>(std::floor(sy / BLOCK_SIZE));
        const int top = tyy;
        // Desce até achar topo sólido (pés no chão, não dentro da rocha).
        while (tyy * BLOCK_SIZE < top * BLOCK_SIZE + kGroundScan) {
            if (ctx.world->isSolid(tx, tyy + 1) && !ctx.world->isSolid(tx, tyy))
                break;
            ++tyy;
        }
        if (tyy * BLOCK_SIZE >= top * BLOCK_SIZE + kGroundScan) return; // sem chão
        sy = static_cast<float>(tyy) * BLOCK_SIZE;
    }
    ctx.enemies->spawn("slime", sx, sy);
}

} // namespace support
