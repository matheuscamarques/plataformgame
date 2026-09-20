#include "BodySystem.h"

#include "assets/SpriteFrameRegistry.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "entities/Player/Player.h"

namespace support {

void BodySystem::tick(float /*dt*/, GameContext &ctx) {
    if (ctx.player) {
        Player &p = *ctx.player;
        const auto f = assets::frameData(p.currentFrameId);
        if (f.rows) {
            p.body.rebuildFromSprite(
                {p.getX(), p.getY()}, {p.getW(), p.getH()},
                f.rows, f.w, f.h, f.pal, f.palCount,
                p.facing);
        } else {
            p.body.rebuild({p.getX(), p.getY()}, p.facing);
        }
    }
    if (ctx.enemies) {
        ctx.enemies->forEach([](Enemy &s) {
            const auto f = assets::frameData(s.currentFrameId);
            if (f.rows) {
                s.bodyParts.rebuildFromSprite(
                    {s.body.getX(), s.body.getY()},
                    {s.body.getW(), s.body.getH()},
                    f.rows, f.w, f.h, f.pal, f.palCount,
                    s.body.facing);
            } else {
                s.bodyParts.rebuild({s.body.getX(), s.body.getY()},
                                     s.body.facing);
            }
        });
    }
}

} // namespace support
