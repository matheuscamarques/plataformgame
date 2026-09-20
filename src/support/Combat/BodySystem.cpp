#include "BodySystem.h"

#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "entities/Player/Player.h"

namespace support {

void BodySystem::tick(float /*dt*/, GameContext &ctx) {
    if (ctx.player) {
        ctx.player->body.rebuild(
            {ctx.player->getX(), ctx.player->getY()},
            ctx.player->facing);
    }
    if (ctx.enemies) {
        ctx.enemies->forEach([](Enemy &s) {
            s.bodyParts.rebuild({s.body.getX(), s.body.getY()},
                                s.body.facing);
        });
    }
}

} // namespace support
