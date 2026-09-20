#include "DwarfAI.h"

#include <cmath>

#include "entities/Player/Player.h"
#include "BehaviorRegistry.h"
#include "Barks.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Progression/PatienceSystem.h"
#include "support/Skills/Skill.h"
#include "support/Skills/SkillSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Skills/UtilityAI.h"

namespace support {

namespace {
constexpr float kAlertTime = 0.4f;
constexpr float kThrowRelease = 0.10f;
constexpr float kRecoverTime = 0.60f;
constexpr float kMeleeRecover = 0.40f;
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
    e.barkCd.tick(dt);
    if (e.barkTimer > 0.f) e.barkTimer -= dt;
    patienceTick(e.patience, dt);

    // Traição consome: volta hostil uma vez, com bark.
    if (patienceShouldBetray(e.patience)) {
        e.patience.betrayed = false;
        lastWarning_ = 0;
        emitBark(e, BarkId::Betrayal);
    }
    // Avisos sobem com warningLevel (o sistema só avisa após reconhecer).
    if (e.patience.warningLevel != lastWarning_) {
        lastWarning_ = e.patience.warningLevel;
        if (lastWarning_ == 1) emitBark(e, BarkId::Warning1);
        else if (lastWarning_ == 2) emitBark(e, BarkId::Warning2);
        else if (lastWarning_ >= 3) emitBark(e, BarkId::Warning3);
    }

    const sf::Vector2f c{e.body.getCenterX(), e.body.getCenterY()};
    const sf::Vector2f pp = ctx.player
        ? sf::Vector2f{ctx.player->getCenterX(), ctx.player->getCenterY()}
        : home_;
    const float dp = ctx.player ? dist(c, pp) : 1e9f;

    // Passivo (estágio 3): não ataca, segue a ~200px.
    if (e.patience.passiveStage == 3 && ctx.player) {
        e.body.setVx(0.f);
        if (dp > 220.f) e.body.setVx((pp.x < c.x ? -1.f : 1.f) * kPatrolSpeed);
        e.body.facing = (pp.x < c.x) ? -1 : 1;
        if (state_ != DwarfState::Patrol) changeState(DwarfState::Patrol);
        return;
    }

    if (state_ == DwarfState::Patrol || state_ == DwarfState::Retreat) {
        if (ctx.player && dp < cfg_.aggroRange) {
            changeState(DwarfState::Alert, kAlertTime);
            emitBark(e, BarkId::Alert);
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
            // Decisão data-driven: UtilityAI escolhe, flags disparam o estado.
            // Cooldown/recursos/custos moram no SkillSystem (tryUse paga).
            // Windup honra o telegraph da skill (collapse: 1.8s).
            const SkillDef *chosen = UtilityAI::choose(e, ctx, e.skillIds);
            if (!chosen) {
                const float dir = (pp.x < c.x) ? -1.f : 1.f;
                e.body.setVx(dir * kApproachSpeed);
            } else if (chosen->isMelee) {
                pendingSkill_ = chosen->id;
                changeState(DwarfState::Melee, chosen->telegraph);
            } else {
                pendingSkill_ = chosen->id;
                changeState(DwarfState::ThrowWindup, chosen->telegraph);
            }
            break;
        }
        case DwarfState::ThrowWindup: {
            e.body.setVx(0.f);
            if (stateTimer_.ready()) {
                if (SkillSystem::tryUse(e, ctx, pendingSkill_))
                    emitBark(e, BarkId::Attack);
                changeState(DwarfState::ThrowRelease, kThrowRelease);
            }
            break;
        }
        case DwarfState::ThrowRelease: {
            e.body.setVx(0.f);
            if (stateTimer_.ready()) {
                changeState(DwarfState::Recover, kRecoverTime);
            }
            break;
        }
        case DwarfState::Melee: {
            e.body.setVx(0.f);
            if (!stateTimer_.ready()) return;
            SkillSystem::tryUse(e, ctx, pendingSkill_);
            changeState(DwarfState::Recover, kMeleeRecover);
            break;
        }
        default: break;
    }
}

void DwarfAI::onTakeHit(Enemy &e, int /*applied*/, GameContext & /*ctx*/) {
    emitBark(e, BarkId::Hurt); // bark tem o próprio cooldown (1.5s)
}

void DwarfAI::emitBark(Enemy &e, BarkId id) {
    if (!e.barkCd.ready()) return;
    e.currentBark =
        BarkRegistry::instance().pick(id, static_cast<uint32_t>(e.body.getCenterX()));
    e.barkTimer = 1.5f;
    e.barkCd.trigger();
}

SUPPORT_REGISTER_BEHAVIOR("dwarf", DwarfAI);

} // namespace support
