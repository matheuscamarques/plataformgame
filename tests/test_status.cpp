#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Combat/ContactDamageSystem.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// F7: veneno/sangramento — acúmulo, trigger, cura, fontes, drops.
int main() {
    using namespace support;

    { // ThresholdScales (100 + 5 RES + 2 ATT; seed 186)
        assert(core::Attributes::statusThreshold(10, 10) == 170.f);
        assert(core::Attributes::statusThreshold(20, 10) == 220.f);
        Player p; // seed RES 10/ATT 18
        assert(p.statusThreshold() == 186.f);
    }
    { // PoisonTriggersAndDots (enche -> 8s de DoT 3/s, barra reseta)
        Player p;
        p.addPoison(200.f);
        assert(p.poisonTimer == 8.f && p.poisonBuildup == 0.f);
        for (int i = 0; i < 30; ++i) p.tick();
        assert(p.hp == 100 - 3); // 3/s por 1s
        assert(p.poisonTimer > 0.f);
        p.addPoison(50.f); // ativo: não reacumula
        assert(p.poisonBuildup == 0.f);
    }
    { // BleedBurstsAndResets (15% do max em burst, sem timer)
        Player p;
        p.addBleed(200.f);
        assert(p.hp == 100 - 15 && p.bleedBuildup == 0.f);
        assert(p.poisonTimer == 0.f);
    }
    { // MossCures (onUse dos defs limpa; J usa via slot também)
        Player p;
        p.addPoison(200.f);
        p.addBleed(50.f);
        const core::ItemDef* pm =
            core::ItemRegistry::instance().find("purple_moss");
        const core::ItemDef* bm =
            core::ItemRegistry::instance().find("bloodred_moss");
        assert(pm && bm && pm->onUse && bm->onUse);
        pm->onUse(p);
        assert(p.poisonTimer == 0.f && p.poisonBuildup == 0.f);
        assert(p.bleedBuildup == 50.f); // roxo não toca sangue
        bm->onUse(p);
        assert(p.bleedBuildup == 0.f);
    }
    { // RespawnClears (morte limpa tudo)
        Player p;
        p.addPoison(200.f);
        p.addBleed(50.f);
        p.respawn(0.f, 0.f);
        assert(p.poisonTimer == 0.f && p.poisonBuildup == 0.f);
        assert(p.bleedBuildup == 0.f);
    }
    { // ContactApplies (slime -> veneno; anão -> sangue)
        auto recol = [](EnemySystem& e) {
            e.forEach([](Enemy& s) {
                s.body.setX(10.f);
                s.body.setY(10.f);
            });
        };
        {
            Player p;
            EnemySystem enemies;
            enemies.spawn("slime", 10.f, 10.f);
            ContactDamageSystem cs;
            GameContext ctx{};
            ctx.player = &p;
            ctx.enemies = &enemies;
            for (int i = 0; i < 11; ++i) {
                recol(enemies);
                cs.tick(1.f / 30.f, ctx);
            }
            assert(p.poisonBuildup > 0.f);
            assert(p.bleedBuildup > 0.f); // slime aplica os dois
        }
        {
            Player p;
            EnemySystem enemies;
            enemies.spawn("dwarf", 10.f, 10.f);
            ContactDamageSystem cs;
            GameContext ctx{};
            ctx.player = &p;
            ctx.enemies = &enemies;
            for (int i = 0; i < 40; ++i) { // anão morde ~35 ticks
                recol(enemies);
                cs.tick(1.f / 30.f, ctx);
            }
            assert(p.bleedBuildup > 0.f);
            assert(p.poisonBuildup == 0.f);
        }
    }
    { // ModifiersPipeline (identidade limpo; slow no proc; clamp)
        Player p;
        core::StatusModifiers clean = p.computeModifiers();
        assert(clean.moveSpeedMult == 1.f);
        assert(clean.staminaRegenMult == 1.f);
        assert(clean.staminaCostMult == 1.f);
        assert(clean.damageTakenMult == 1.f);
        assert(clean.hpMaxMult == 1.f);
        assert(clean.canRoll && clean.canAttack && !clean.blocksHealing);
        assert(p.effectiveHpMax() == p.hpMax);
    }
    { // BleedSlowCutsRun (burst arma 0.3s; sprint cai; expira sozinho)
        Player p;
        p.runFast = true;
        p.addBleed(200.f); // burst + slow
        assert(p.computeModifiers().moveSpeedMult == 0.9f);
        p.tick();
        assert(!p.runFast); // micro-stagger comportamental
        for (int i = 0; i < 10; ++i) p.tick();
        assert(p.computeModifiers().moveSpeedMult == 1.f);
    }
    { // PoisonFaithful (só dano: speeds intactos com timer ativo)
        Player p;
        p.addPoison(200.f);
        const core::StatusModifiers m = p.computeModifiers();
        assert(m.moveSpeedMult == 1.f && m.staminaRegenMult == 1.f);
        assert(m.staminaCostMult == 1.f && m.damageTakenMult == 1.f);
    }
    { // EffectiveHpMaxClamp (overheal volta ao teto no tick)
        Player p;
        p.hp = p.hpMax + 50;
        p.tick();
        assert(p.hp == p.effectiveHpMax());
        assert(p.effectiveHpMax() == p.hpMax); // sem curse: igual
    }
    { // SlimeDropsMoss (tabela do archetype tem os 2 musgos)
        const EnemyArchetype* a =
            ArchetypeRegistry::instance().find("slime");
        assert(a != nullptr);
        bool purple = false, red = false;
        for (const auto& e : a->drops.entries) {
            if (e.itemId == "purple_moss") purple = true;
            if (e.itemId == "bloodred_moss") red = true;
        }
        assert(purple && red);
    }

    std::printf("status test OK\n");
    return 0;
}
