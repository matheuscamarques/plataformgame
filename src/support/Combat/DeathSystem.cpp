#include "DeathSystem.h"

#include "support/Progression/DropSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Effects/ParticleSystem.h"

namespace support {

void DeathSystem::tick(float /*dt*/, GameContext &ctx) {
    if (!ctx.enemies) return;
    // Seguro: EnemySystem (150), BodySystem (250) e ExplosionSystem (320)
    // já rodaram; nada depois de 330 itera slimes no tick.
    ctx.enemies->removeDead([&](sf::Vector2f pos) {
        if (particles_) particles_->spawnTileBreak(pos, 0, 0, 0);
        if (drops_)     drops_->spawnXP(pos, 1);
    }, &ctx);
}

} // namespace support
