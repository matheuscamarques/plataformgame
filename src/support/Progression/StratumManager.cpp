#include "StratumManager.h"

#include <cmath>

#include "defines.h"
#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "support/GameContext.h"
#include "world/Stratum.h"

namespace support {

void StratumManager::tick(float /*dt*/, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p) return;
    const int ty = static_cast<int>(std::floor(p->getCenterY() / core::kBlockSize));
    const int s = stratumAt(ty);
    current_ = s;
    if (s > deepest_) {
        for (int i = deepest_ + 1; i <= s; ++i) unlocked_ |= (1u << i);
        deepest_ = s;
        // SFX checkpoint (estrato novo; sem ctx.audio em teste = mudo).
        if (ctx.audio) ctx.audio->play(game::keyOf(game::Sfx::Checkpoint));
    }
}

sf::Vector2f StratumManager::respawnPoint(float x) const {
    return {x, static_cast<float>(checkpointTy(deepest_)) * core::kBlockSize};
}

} // namespace support
