#include "StratumManager.h"

#include <cmath>

#include "defines.h"
#include "entities/player/player.h"
#include "support/GameContext.h"
#include "world/Stratum.h"

namespace support {

void StratumManager::tick(float /*dt*/, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p) return;
    const int ty = static_cast<int>(std::floor(p->getCenterY() / BLOCK_SIZE));
    const int s = stratumAt(ty);
    if (s > deepest_) {
        for (int i = deepest_ + 1; i <= s; ++i) unlocked_ |= (1u << i);
        deepest_ = s;
    }
}

sf::Vector2f StratumManager::respawnPoint(float x) const {
    return {x, static_cast<float>(checkpointTy(deepest_)) * BLOCK_SIZE};
}

} // namespace support
