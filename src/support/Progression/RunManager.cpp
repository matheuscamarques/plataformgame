#include "RunManager.h"

#include <cmath>

#include "defines.h"
#include "entities/Player/Player.h"
#include "support/Progression/DropSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Input/InputMap.h"
#include "StratumManager.h"
#include "support/Effects/ThrowSystem.h"
#include "world/Stratum.h"
#include "world/World.h"

namespace support {

void RunManager::tick(float /*dt*/, GameContext &ctx) {
    if (ctx.input && ctx.input->pressed(Action::Pause) && !dead_)
        paused_ = !paused_;
    if (ctx.input && ctx.input->pressed(Action::Restart))
        restart(ctx);
    if (!dead_ && ctx.player && ctx.player->hp <= 0) dead_ = true;
}

void RunManager::restart(GameContext &ctx) {
    Player *p = ctx.player;
    if (!p) return;

    float x = p->getX();
    float y = stratum_ ? stratum_->respawnPoint(x).y : 0.f;
    if (ctx.world) {
        int tx = static_cast<int>(std::floor(p->getCenterX() / core::kBlockSize));
        int ty = static_cast<int>(std::floor(y / core::kBlockSize));
        int guard = 0;
        // 2 tiles livres (corpo 50px): cabeça e pés fora da rocha.
        while (guard++ < 400 &&
               (ctx.world->isSolid(tx, ty) || ctx.world->isSolid(tx, ty - 1)))
            ty--;
        y = static_cast<float>(ty) * core::kBlockSize;
    }
    p->respawn(x, y);

    if (ctx.enemies) {
        ctx.enemies->clear();
        ctx.enemies->spawn("slime", x - 300.f, y);
        ctx.enemies->spawn("slime", x + 300.f, y);
    }
    if (ctx.throws) ctx.throws->clear();
    if (ctx.drops) ctx.drops->clear();
    // Partículas: fora do ctx, somem sozinhas (lifetime curto).

    dead_ = false;
    paused_ = false;
}

} // namespace support
