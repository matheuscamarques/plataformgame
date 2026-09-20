#include "DwarfAI.h"

#include <cmath>

#include "../entities/player/player.h"
#include "BehaviorRegistry.h"
#include "EnemySystem.h"
#include "GameContext.h"
#include "ThrowSystem.h"

namespace support {

namespace {
constexpr float kAlertTime = 0.4f;
constexpr float kThrowWindup = 0.30f;
constexpr float kThrowRelease = 0.10f;
constexpr float kRecoverTime = 0.60f;
constexpr float kMeleeWindup = 0.25f;
constexpr float kMeleeRecover = 0.40f;
constexpr float kMeleeDamage = 12.f;
// Velocidades em px/tick (física integra vx direto, como SlimeAI:
// patrol 3.0, chase 6.0 — NÃO px/s).
constexpr float kPatrolSpeed = 2.0f;
constexpr float kApproachSpeed = 2.5f;
constexpr float kRetreatSpeed = 3.0f;

float dist(sf::Vector2f a, sf::Vector2f b) {
    const float dx = a.x - b.x, dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}
} // namespace

void DwarfAI::changeState(DwarfState s, float duration) {
    state_ = s;
    if (duration > 0.f) stateTimer_.trigger(duration);
    else stateTimer_.reset();
}

void DwarfAI::onTick(Enemy &e, float dt, GameContext &ctx) {
    if (!homed_) {
        home_ = {e.body.getCenterX(), e.body.getCenterY()};
        homed_ = true;
    }
    stateTimer_.tick(dt);
    throwCd_.tick(dt);
    meleeCd_.tick(dt);

    const sf::Vector2f c{e.body.getCenterX(), e.body.getCenterY()};
    const sf::Vector2f pp = ctx.player
        ? sf::Vector2f{ctx.player->getCenterX(), ctx.player->getCenterY()}
        : home_;
    const float dp = ctx.player ? dist(c, pp) : 1e9f;

    if (state_ == DwarfState::Patrol || state_ == DwarfState::Retreat) {
        if (ctx.player && dp < cfg_.aggroRange) {
            changeState(DwarfState::Alert, kAlertTime);
        } else if (state_ == DwarfState::Patrol) {
            tickPatrol(e, dt, ctx);
        } else {
            // Retreat: volta à home; chegou → Patrol.
            const float dx = home_.x - c.x;
            if (std::fabs(dx) < 8.f) {
                e.body.setVx(0.f);
                changeState(DwarfState::Patrol);
            } else {
                e.body.setVx((dx > 0.f ? 1.f : -1.f) * kRetreatSpeed);
            }
        }
        e.body.facing = (pp.x < c.x) ? -1 : 1;
        return;
    }

    if (state_ == DwarfState::Alert) {
        e.body.setVx(0.f);
        e.body.facing = (pp.x < c.x) ? -1 : 1;
        if (stateTimer_.ready()) changeState(DwarfState::Recover);
        return;
    }

    tickCombat(e, dt, ctx);
}

void DwarfAI::tickPatrol(Enemy &e, float /*dt*/, GameContext & /*ctx*/) {
    const float dx = e.body.getCenterX() - home_.x;
    if (dx > cfg_.homeRadius) patrolDir_ = -1;
    else if (dx < -cfg_.homeRadius) patrolDir_ = 1;
    e.body.setVx(static_cast<float>(patrolDir_) * kPatrolSpeed);
    e.body.facing = patrolDir_;
}

void DwarfAI::tickCombat(Enemy &e, float /*dt*/, GameContext &ctx) {
    const sf::Vector2f c{e.body.getCenterX(), e.body.getCenterY()};
    const sf::Vector2f pp = ctx.player
        ? sf::Vector2f{ctx.player->getCenterX(), ctx.player->getCenterY()}
        : home_;
    const float dp = ctx.player ? dist(c, pp) : 1e9f;
    e.body.facing = (pp.x < c.x) ? -1 : 1;

    // Perdeu o player de vista: recua.
    if (dp > cfg_.aggroRange * 1.5f) {
        changeState(DwarfState::Retreat);
        return;
    }

    switch (state_) {
        case DwarfState::Recover: {
            e.body.setVx(0.f);
            if (!stateTimer_.ready()) return;
            if (dp < cfg_.meleeRange && meleeCd_.ready()) {
                changeState(DwarfState::Melee, kMeleeWindup);
            } else if (dp < cfg_.throwRange && throwCd_.ready()) {
                changeState(DwarfState::ThrowWindup, kThrowWindup);
            } else {
                const float dir = (pp.x < c.x) ? -1.f : 1.f;
                e.body.setVx(dir * kApproachSpeed);
            }
            break;
        }
        case DwarfState::ThrowWindup: {
            e.body.setVx(0.f);
            if (stateTimer_.ready()) {
                throwDynamite(e, ctx);
                changeState(DwarfState::ThrowRelease, kThrowRelease);
            }
            break;
        }
        case DwarfState::ThrowRelease: {
            e.body.setVx(0.f);
            if (stateTimer_.ready()) {
                throwCd_.trigger();
                changeState(DwarfState::Recover, kRecoverTime);
            }
            break;
        }
        case DwarfState::Melee: {
            e.body.setVx(0.f);
            if (!stateTimer_.ready()) return;
            if (dp < cfg_.meleeRange + 8.f && ctx.player) {
                ctx.player->hurt(static_cast<int>(kMeleeDamage));
            }
            meleeCd_.trigger();
            changeState(DwarfState::Recover, kMeleeRecover);
            break;
        }
        default: break;
    }
}

void DwarfAI::throwDynamite(Enemy &e, GameContext &ctx) {
    if (!ctx.throws || !ctx.player) return;
    const sf::Vector2f from{e.body.getCenterX(), e.body.getCenterY()};
    const float dx = ctx.player->getCenterX() - from.x;
    const float dir = (dx < 0.f) ? -1.f : 1.f;
    // Arco na direção do player (unidades px/s do Throwable).
    ctx.throws->throwItem(from, {dir * 180.f, -320.f});
}

SUPPORT_REGISTER_BEHAVIOR("dwarf", DwarfAI);

} // namespace support
