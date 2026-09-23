/**
 * @file src/support/Skills/UtilityAI.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Pontua skills por alcance, custo, repetição e HP para elite.
 * @details Implementa UtilityAI choose com score determinístico sobre SkillRegistry, chamado por DwarfAI para escolher skill.
 */

#include "UtilityAI.h"

#include <cmath>

#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Skills/Skill.h"

namespace support {

namespace {

float distanceToPlayer(Enemy &self, const GameContext &ctx) {
    if (!ctx.player) return 99999.f;
    const float dx = ctx.player->getCenterX() - self.body.getCenterX();
    const float dy = ctx.player->getCenterY() - self.body.getCenterY();
    return std::sqrt(dx * dx + dy * dy);
}

float scoreFor(const SkillDef &s, Enemy &self, float dist) {
    Cost c{s.manaCost, s.staminaCost, s.postureCost};
    if (!self.resources.canPay(c)) return -1.f;

    auto it = self.skillCds.find(s.id);
    if (it != self.skillCds.end() && it->second.running()) return -1.f;

    if (dist < s.minRange) return -1.f;
    if (dist > s.maxRange) return -1.f;

    float score = s.baseWeight;
    const float mid = (s.minRange + s.maxRange) * 0.5f;
    if (s.isRanged && dist > mid) score += 15.f;
    if (s.isMelee && dist < mid) score += 20.f;
    if (self.lastSkillId == s.id) score -= 25.f;
    if (s.isSpecial) {
        const float hpRatio = static_cast<float>(self.resources.hp) /
                              static_cast<float>(self.resources.hpMax);
        score += (hpRatio < 0.2f) ? 100.f : -50.f;
    }
    return score;
}

} // namespace

const SkillDef *UtilityAI::choose(Enemy &self,
                                  const GameContext &ctx,
                                  const std::vector<std::string> &candidates) {
    const float dist = distanceToPlayer(self, ctx);
    const SkillDef *best = nullptr;
    float bestScore = 0.f; // score <= 0 não é escolha (só gates zerados passam)
    for (const auto &id : candidates) {
        const SkillDef *def = SkillRegistry::instance().find(id);
        if (!def) continue;
        const float s = scoreFor(*def, self, dist);
        if (s > bestScore) {
            bestScore = s;
            best = def;
        }
    }
    return best;
}

} // namespace support
