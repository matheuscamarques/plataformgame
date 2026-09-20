#include "MeleeSystem.h"

#include <SFML/Graphics/Rect.hpp>

#include "../entities/player/player.h"
#include "EnemySystem.h"
#include "GameContext.h"
#include "Input/InputMap.h"
#include "ParticleSystem.h"

namespace support {

void MeleeSystem::tick(float dt, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;

    // K (Heavy) inicia/encadeia. Input pode ser nulo em testes
    // (swing disparado direto por startSwing).
    if (ctx.input && ctx.input->pressed(Action::Heavy)) p->startSwing();

    if (p->updateMelee(dt) != MeleePhase::Active) return;

    const sf::FloatRect box = p->meleeHitbox();
    if (box.width <= 0.f) return;

    const int dmg = p->meleeDamage();
    const float post = p->meleePosture();
    ctx.enemies->forEach([&](Enemy &s) {
        if (s.resources.isDead()) return;
        if (s.lastHitSwing == p->meleeSwingId) return; // 1 hit por swing
        const sf::FloatRect sb{s.body.getX(), s.body.getY(),
                               s.body.getW(), s.body.getH()};
        if (!box.intersects(sb)) return;
        const int applied = s.resources.takeDamage(dmg);
        s.resources.damagePosture(post);
        if (applied > 0 && s.ai) s.ai->onTakeHit(s, applied, ctx);
        s.lastHitSwing = p->meleeSwingId;
        if (particles_) {
            particles_->spawnHitSpark(
                {sb.left + sb.width * 0.5f, sb.top + sb.height * 0.5f});
        }
    });
}

} // namespace support
