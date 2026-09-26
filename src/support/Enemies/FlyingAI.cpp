/**
 * @file src/support/Enemies/FlyingAI.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa FlyingAI onTick com perseguição 2D e skills por dado.
 * @details Perto: orbita o player; longe: vai até ele; fora do aggro: deriva. Skills: primeira Melee a 60px, primeira Ranged a 220px (harpia tem pena, olho só encosta), registrado via subclasses HarpyAI/DemonEyeAI.
 */

#include "FlyingAI.h"

#include <cmath>

#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Skills/Skill.h"
#include "support/Skills/SkillSystem.h"

namespace support {

void FlyingAI::onTick(Enemy &e, float dt, GameContext &ctx) {
    (void)dt;
    ++tick_;
    Player *p = ctx.player;
    if (!p) {
        e.body.setVx(0.f);
        e.body.setVy(0.f);
        return;
    }
    const float dx = p->getCenterX() - e.body.getCenterX();
    const float dy = p->getCenterY() - e.body.getCenterY();
    const float dist = std::sqrt(dx * dx + dy * dy);
    e.body.facing = (dx >= 0.f) ? 1 : -1;

    if (dist > cfg_.aggroRange) {
        // Deriva em patrulha (seno; sem mundo, sem rota).
        e.body.setVx(std::sin(tick_ * 0.03f) * 2.f);
        e.body.setVy(std::cos(tick_ * 0.021f) * 1.5f);
        return;
    }
    if (dist > cfg_.orbitRadius) {
        // Aproxima em 2D (velocidade constante, sem gravidade).
        if (dist < 1.f) return;
        e.body.setVx(dx / dist * cfg_.flySpeed);
        e.body.setVy(dy / dist * cfg_.flySpeed);
    } else {
        // Orbita: tangente ao vetor do player.
        if (dist < 1.f) return;
        e.body.setVx(-dy / dist * cfg_.flySpeed * 0.6f);
        e.body.setVy(dx / dist * cfg_.flySpeed * 0.6f);
    }

    // Skills por dado (mesmo padrão do slime, em 2D).
    const SkillDef *melee = nullptr;
    const SkillDef *ranged = nullptr;
    for (auto &id : e.skillIds) {
        const SkillDef *sd = SkillRegistry::instance().find(id);
        if (!sd) continue;
        if (!melee && sd->isMelee) melee = sd;
        if (!ranged && sd->isRanged) ranged = sd;
    }
    if (melee && dist < 60.f)
        SkillSystem::tryUse(e, ctx, melee->id);
    else if (ranged && dist < 220.f)
        SkillSystem::tryUse(e, ctx, ranged->id);
}

} // namespace support
