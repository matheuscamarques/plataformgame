/**
 * @file tests/test_frost.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava frost status (Fase 2 elementais).
 * @details Cobre buildup, ativação, expiração, attackSpeedMult e swing lento, roda com make test que compila em build/tests/test_frost.
 */

#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Skills/Skill.h"
#include "support/Skills/SkillSystem.h"

int main() {
    { // BuildupActivates (acumula até o limiar, ativa 6s)
        Player p;
        const float th = p.statusThreshold();
        assert(p.frostTimer == 0.f);
        p.addFrost(th * 0.5f);
        assert(p.frostTimer == 0.f && p.frostBuildup == th * 0.5f);
        p.addFrost(th * 0.5f);
        assert(p.frostTimer == Player::kFrostDur && p.frostBuildup == 0.f);
    }
    { // NoReaccumulateWhileActive (ativo ignora buildup novo)
        Player p;
        p.addFrost(p.statusThreshold());
        assert(p.frostTimer > 0.f);
        p.addFrost(999.f);
        assert(p.frostBuildup == 0.f);
    }
    { // ExpiresInTick (6s a 30Hz; margem p/ erro float de 1/30)
        Player p;
        p.addFrost(p.statusThreshold());
        for (int i = 0; i < 200; ++i) p.tick();
        assert(p.frostTimer <= 0.f);
    }
    { // SlowMultWhileActive (0.7 ativo, 1.0 fora)
        Player p;
        assert(p.computeModifiers().attackSpeedMult == 1.f);
        p.addFrost(p.statusThreshold());
        assert(p.computeModifiers().attackSpeedMult == Player::kFrostSlow);
    }
    { // SwingRunsSlow (windup dura mais ticks sob frost)
        auto ticksToActive = [](bool frosted) {
            Player p;
            if (frosted) p.addFrost(p.statusThreshold());
            assert(p.startSwing());
            int n = 0;
            while (p.meleePhase == MeleePhase::Windup && n < 1000) {
                p.updateMelee(1.f / 30.f);
                ++n;
            }
            return n;
        };
        const int slow = ticksToActive(true);
        const int fast = ticksToActive(false);
        assert(fast > 0 && slow > fast); // 0.7x desce mais devagar
    }

    { // FrostTouchApplies (skill registrada, dano frost + buildup)
        const support::SkillDef *s =
            support::SkillRegistry::instance().find("frost_touch");
        assert(s != nullptr && s->isMelee);
        assert(s->damageType == core::DamageType::Frost);
        Player p;
        p.setX(100.f);
        p.setY(100.f);
        p.hp = 100;
        support::EnemySystem enemies;
        enemies.spawn("dwarf", 100.f, 100.f); // elite paga stamina
        support::GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        bool ran = false;
        enemies.forEach([&](support::Enemy &e) {
            ran = support::SkillSystem::tryUse(e, ctx, "frost_touch");
        });
        assert(ran);
        // 8 × universal Nv15 (0.972) = 7
        assert(p.hp == 93); // 8 frost, resist 1.0
        assert(p.frostBuildup == 20.f);
    }
    { // RespawnCuresFrost (morte congelada não volta congelada)
        Player p;
        p.addFrost(p.statusThreshold());
        assert(p.frostTimer > 0.f);
        p.respawn(0.f, 0.f);
        assert(p.frostTimer == 0.f && p.frostBuildup == 0.f);
    }

    std::printf("frost test OK\n");
    return 0;
}
