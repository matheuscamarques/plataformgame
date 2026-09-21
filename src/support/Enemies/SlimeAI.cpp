#include "SlimeAI.h"
#include "BehaviorRegistry.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Skills/SkillSystem.h"

#include <cmath>

#include "entities/Player/Player.h"

namespace support {

namespace {
constexpr float PATROL_SPEED = 3.0f;
constexpr float CHASE_SPEED = 6.0f;
constexpr int FLIP_TICKS = 90;
constexpr int HOP_COOLDOWN = 80;
constexpr float HOP_VY = -30.0f;
constexpr float AGGRO_X = 400.0f;
constexpr float AGGRO_Y = 400.0f;
} // namespace

void SlimeAI::onTick(Enemy &e, float dt, GameContext &ctx) {
    (void)dt;
    Player *p = ctx.player;

    bool chase = false;
    float dx = 0.0f, dy = 0.0f;
    if (p) {
        dx = p->getCenterX() - e.body.getCenterX();
        dy = p->getCenterY() - e.body.getCenterY();
        chase = std::fabs(dx) < AGGRO_X && std::fabs(dy) < AGGRO_Y;
    }
    chasing_ = chase;

    // Skill ranged: cospe entre 40 e 220px (cooldown+stamina no tryUse).
    if (p) {
        const float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > 40.f && dist < 220.f) SkillSystem::tryUse(e, ctx, "slime_spit");
    }

    float speed = chase ? CHASE_SPEED : PATROL_SPEED;
    if (chase) {
        dir_ = (dx >= 0.0f) ? 1.0f : -1.0f;
    } else {
        flipTimer_++;
        if (flipTimer_ >= FLIP_TICKS) {
            flipTimer_ = 0;
            dir_ = -dir_;
        }
    }

    // Travou na parede (tentou andar e não saiu do lugar)? Vira.
    if (hasLast_ && std::fabs(e.body.getX() - lastX_) < 0.5f) dir_ = -dir_;
    lastX_ = e.body.getX();
    hasLast_ = true;

    e.body.setVx(dir_ * speed);
    e.body.facing = (dir_ >= 0.0f) ? 1 : -1;

    if (hopCooldown_ > 0) hopCooldown_--;
    if (grounded && hopCooldown_ <= 0) {
        e.body.setVy(HOP_VY);
        grounded = false;
        hopCooldown_ = HOP_COOLDOWN;
    }
}

SUPPORT_REGISTER_BEHAVIOR("slime", SlimeAI);

} // namespace support
