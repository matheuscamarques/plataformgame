#include "SlimeAI.h"
#include "BehaviorRegistry.h"
#include "GameContext.h"

#include <cmath>

#include "../entities/player/player.h"

namespace support {

namespace {
constexpr float PATROL_SPEED = 3.0f;
constexpr float CHASE_SPEED = 6.0f;
constexpr int FLIP_TICKS = 90;
constexpr int HOP_COOLDOWN = 80;
constexpr float HOP_VY = -30.0f;
constexpr float AGGRO_X = 250.0f;
constexpr float AGGRO_Y = 150.0f;
} // namespace

void SlimeAI::onTick(Entity &e, float dt, GameContext &ctx) {
    (void)dt;
    Player *p = ctx.player;

    bool chase = false;
    float dx = 0.0f;
    if (p) {
        dx = p->getCenterX() - e.getCenterX();
        float dy = p->getCenterY() - e.getCenterY();
        chase = std::fabs(dx) < AGGRO_X && std::fabs(dy) < AGGRO_Y;
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
    if (hasLast_ && std::fabs(e.getX() - lastX_) < 0.5f) dir_ = -dir_;
    lastX_ = e.getX();
    hasLast_ = true;

    e.setVx(dir_ * speed);

    if (hopCooldown_ > 0) hopCooldown_--;
    if (grounded && hopCooldown_ <= 0) {
        e.setVy(HOP_VY);
        grounded = false;
        hopCooldown_ = HOP_COOLDOWN;
    }
}

SUPPORT_REGISTER_BEHAVIOR("slime", SlimeAI);

} // namespace support
