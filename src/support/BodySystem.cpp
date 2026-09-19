#include "BodySystem.h"

#include "EnemySystem.h"
#include "GameContext.h"
#include "../entities/player/player.h"

namespace support {

void BodySystem::tick(float /*dt*/, GameContext &ctx) {
    if (ctx.player) {
        ctx.player->body.rebuild(
            {ctx.player->getX(), ctx.player->getY()},
            ctx.player->facing);
    }
    if (ctx.enemies) {
        ctx.enemies->forEach([](Slime &s) {
            s.bodyParts.rebuild({s.body.getX(), s.body.getY()},
                                s.body.facing);
        });
    }
}

} // namespace support
