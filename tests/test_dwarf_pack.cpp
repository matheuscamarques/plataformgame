#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
#include "entities/player/player.h"
#include "support/Barks.h"
#include "support/EnemySystem.h"
#include "support/GameContext.h"
#include "support/PatienceSystem.h"
#include "support/Skill.h"
#include "support/UtilityAI.h"
#include "support/VariantRegistry.h"

// Pacote anão: variant por estrato, 6 skills, paciência, barks, special.
int main() {
    using namespace support;

    { // VariantLevelsByStratum (S3→1 ... S7+→5; slime sem variante)
        assert(VariantRegistry::instance().forDepth("dwarf", 3)->level == 1);
        assert(VariantRegistry::instance().forDepth("dwarf", 5)->level == 3);
        assert(VariantRegistry::instance().forDepth("dwarf", 7)->level == 5);
        assert(VariantRegistry::instance().forDepth("dwarf", 20)->level == 5);
        assert(VariantRegistry::instance().forDepth("slime", 3) == nullptr);
        const VariantDef *v5 = VariantRegistry::instance().forDepth("dwarf", 9);
        assert(v5->hpBonus == 40 && v5->damageMult == 1.5f);
    }
    { // AllDwarfSkillsRegistered
        const char *ids[] = {"dwarf_dynamite", "dwarf_melee", "dwarf_smoke",
                             "dwarf_barrel", "dwarf_dig", "dwarf_collapse"};
        for (auto id : ids) assert(SkillRegistry::instance().find(id) != nullptr);
    }
    { // PatienceWarningEscalates (pedra: -8; teto por estágio não trava aviso)
        PatienceState p;
        p.passiveStage = 3;
        patienceOnMine(p, false, 30.f);
        patienceOnMine(p, false, 30.f);
        patienceOnMine(p, false, 30.f); // 100-24 = 76
        assert(p.warningLevel == 0);
        patienceOnMine(p, false, 30.f); // 68
        assert(p.warningLevel == 1);
        assert(!patienceShouldBetray(p));
    }
    { // PatienceOreCutsFaster (minério: -25; 4x = traição)
        PatienceState p;
        p.passiveStage = 3;
        patienceOnMine(p, true, 30.f);
        patienceOnMine(p, true, 30.f);
        patienceOnMine(p, true, 30.f);
        patienceOnMine(p, true, 30.f); // 0
        assert(p.betrayed && p.passiveStage == 0);
        assert(patienceShouldBetray(p));
        // Longe (>128px) e hostil (estágio<2): sem efeito.
        PatienceState q;
        patienceOnMine(q, true, 500.f);
        assert(q.value == 100.f && q.warningLevel == 0);
    }
    { // PatienceNuggetProgression (0→1→2, +60s de trust →3)
        PatienceState p;
        assert(patienceOnNugget(p) == 1);
        assert(patienceOnNugget(p) == 2);
        patienceTick(p, 60.f);
        assert(p.passiveStage == 3);
    }
    { // BarksAllIdsHaveLines
        auto &b = BarkRegistry::instance();
        for (int i = 0; i < static_cast<int>(BarkId::COUNT); ++i) {
            assert(!b.pick(static_cast<BarkId>(i), 0).empty());
        }
    }
    { // SpecialGateLowHp (collapse +100 com hp<20%)
        Player pl;
        pl.setX(20.f);
        pl.setY(0.f);
        EnemySystem enemies;
        enemies.spawn("dwarf", 0.f, 0.f);
        GameContext ctx{};
        ctx.player = &pl;
        ctx.enemies = &enemies;
        enemies.forEach([&](Enemy &e) {
            e.skillIds = {"dwarf_collapse", "dwarf_melee"};
            e.resources.hp = 10;
            const SkillDef *s = UtilityAI::choose(e, ctx, e.skillIds);
            assert(s != nullptr && s->id == "dwarf_collapse");
            e.resources.hp = 60;
            s = UtilityAI::choose(e, ctx, e.skillIds);
            assert(s != nullptr && s->id == "dwarf_melee");
        });
    }
    { // VariantAppliedOnSpawn (S7: hp 100, dig+collapse no pool)
        EnemySystem enemies;
        // y de estrato 7: ty 7500 → S7 → nv5.
        enemies.spawn("dwarf", 0.f, 7500.f * 50.f);
        enemies.forEach([&](Enemy &e) {
            assert(e.variantLevel == 5 && e.damageMult == 1.5f);
            assert(e.resources.hp == 100 && e.resources.hpMax == 100);
            bool dig = false, col = false;
            for (auto &id : e.skillIds) {
                if (id == "dwarf_dig") dig = true;
                if (id == "dwarf_collapse") col = true;
            }
            assert(dig && col);
        });
    }

    std::printf("dwarf pack test OK\n");
    return 0;
}
