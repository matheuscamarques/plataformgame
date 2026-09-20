#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Skills/Skill.h"
#include "support/Skills/UtilityAI.h"

// Especial de teste: +100 com hp<20%, -50 caso contrário.
REGISTER_SKILL("test_special", [] {
    support::SkillDef s;
    s.name = "Special";
    s.isSpecial = true;
    s.minRange = 0.f;
    s.maxRange = 999.f;
    s.baseWeight = 10.f;
    s.execute = [](support::Enemy &, support::GameContext &) {};
    return s;
}());

// UtilityAI: gates eliminam, score ordena. Determinístico, sem RNG.
int main() {
    using namespace support;
    // Anão com skillIds do archetype, parado em (0,0).
    auto makeDwarf = []() {
        EnemySystem enemies;
        enemies.spawn("dwarf", 0.f, 0.f);
        return enemies;
    };
    auto place = [](Player &p, float x) { p.setX(x); p.setY(0.f); };

    { // PicksRangedAtDistance (100px: dinamite; melee fora de range)
        Player p;
        place(p, 100.f);
        auto enemies = makeDwarf();
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        const SkillDef *s = nullptr;
        enemies.forEach([&](Enemy &e) {
            s = UtilityAI::choose(e, ctx, e.skillIds);
        });
        assert(s != nullptr && s->id == "dwarf_dynamite");
    }
    { // MeleeWinsClose (20px: picaretada)
        Player p;
        place(p, 20.f);
        auto enemies = makeDwarf();
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        const SkillDef *s = nullptr;
        enemies.forEach([&](Enemy &e) {
            s = UtilityAI::choose(e, ctx, e.skillIds);
        });
        assert(s != nullptr && s->id == "dwarf_melee");
    }
    { // NullWhenAllOnCooldown
        Player p;
        place(p, 100.f);
        auto enemies = makeDwarf();
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        const SkillDef *s = nullptr;
        enemies.forEach([&](Enemy &e) {
            e.skillCds["dwarf_dynamite"].trigger(1.8f);
            e.skillCds["dwarf_melee"].trigger(0.8f);
            s = UtilityAI::choose(e, ctx, e.skillIds);
        });
        assert(s == nullptr);
    }
    { // NullWhenOutOfRange (500px: nada alcança)
        Player p;
        place(p, 500.f);
        auto enemies = makeDwarf();
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        const SkillDef *s = reinterpret_cast<const SkillDef *>(0x1);
        enemies.forEach([&](Enemy &e) {
            s = UtilityAI::choose(e, ctx, e.skillIds);
        });
        assert(s == nullptr);
    }
    { // RepeatPenaltyDoesNotVeto (melee penalizada ainda vence sozinha)
        Player p;
        place(p, -7.f); // centro a ~3px: bônus melee conta (15+20-25=10)
        auto enemies = makeDwarf();
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        const SkillDef *s = nullptr;
        enemies.forEach([&](Enemy &e) {
            e.lastSkillId = "dwarf_melee"; // 15+20-25 = 10 > 0
            s = UtilityAI::choose(e, ctx, e.skillIds);
        });
        assert(s != nullptr && s->id == "dwarf_melee");
    }
    { // SpecialSwingsLowHp (skill de teste com isSpecial)
        Player p;
        place(p, 20.f);
        auto enemies = makeDwarf();
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        enemies.forEach([&](Enemy &e) {
            e.resources.hp = 10; // 10/60 < 20%: +100
            const SkillDef *s =
                UtilityAI::choose(e, ctx, {"test_special", "dwarf_melee"});
            assert(s != nullptr && s->id == "test_special");
            e.resources.hp = 60; // cheia: -50 → melee (35) vence
            s = UtilityAI::choose(e, ctx, {"test_special", "dwarf_melee"});
            assert(s != nullptr && s->id == "dwarf_melee");
        });
    }
    { // UnknownIdsSkipped
        Player p;
        place(p, 100.f);
        auto enemies = makeDwarf();
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        const SkillDef *s = nullptr;
        enemies.forEach([&](Enemy &e) {
            s = UtilityAI::choose(e, ctx, {"nope", "dwarf_dynamite"});
        });
        assert(s != nullptr && s->id == "dwarf_dynamite");
    }

    std::printf("utility test OK\n");
    return 0;
}
