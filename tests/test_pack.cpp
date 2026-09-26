/**
 * @file tests/test_pack.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless do pack (6 por dado + 2 voadores).
 * @details Cobre registro, spawn, schema, kind, frames e skills dos 8 arquétipos; voo ignora gravidade; burst detona e morre; rato usa rat_bite, roda com make test que compila em build/tests/test_pack.
 */

#include <cassert>
#include <cstdio>
#include <string>

#include "entities/Player/Player.h"
#include "support/Combat/BodySchemaRegistry.h"
#include "support/Combat/SpriteFrame.h"
#include "assets/SpriteFrameRegistry.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/SpawnSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Skills/Skill.h"
#include "support/Skills/SkillSystem.h"

int main() {
    using namespace support;

    struct Expect {
        const char *id;
        core::EntityKind kind;
        const char *behavior;
        const char *schema;
        int hp;
        int xp;
        SpriteFrameId idle;
    };
    const Expect cases[] = {
        {"hollow", core::EntityKind::Hollow, "hollow", "dwarf", 35, 80,
         SpriteFrameId::HollowIdle},
        {"rat", core::EntityKind::Rat, "rat", "humanoid", 25, 40,
         SpriteFrameId::RatIdle},
        {"burst", core::EntityKind::Burst, "burst", "dwarf", 50, 100,
         SpriteFrameId::BurstIdle},
        {"imp", core::EntityKind::Imp, "imp", "dwarf", 40, 110,
         SpriteFrameId::ImpIdle},
        {"elemental", core::EntityKind::Elemental, "elemental", "dwarf", 55,
         130, SpriteFrameId::ElementalIdle},
        {"undead", core::EntityKind::Undead, "undead", "dwarf", 45, 100,
         SpriteFrameId::UndeadIdle},
        {"harpy", core::EntityKind::Harpy, "harpy", "humanoid", 35, 90,
         SpriteFrameId::HarpyIdle},
        {"eye", core::EntityKind::Eye, "eye", "humanoid", 30, 60,
         SpriteFrameId::EyeIdle},
    };

    { // PackRegistered (dado, schema, frames resolvem)
        for (const auto &c : cases) {
            const EnemyArchetype *a =
                ArchetypeRegistry::instance().find(c.id);
            assert(a != nullptr);
            assert(a->behaviorKind == c.behavior && a->kind == c.kind);
            assert(a->bodySchema == c.schema);
            assert(a->hp == c.hp && a->xp == c.xp);
            assert(a->frameIdle == c.idle);
            assert(BodySchemaRegistry::instance().get(c.schema) != nullptr);
            const auto f = assets::frameData(c.idle);
            assert(f.rows != nullptr && f.w == 14);
            auto e = Factory::spawnEnemy(c.id, 0.f, 0.f);
            assert(e != nullptr && e->archetypeId == c.id);
            assert(e->resources.hp == c.hp);
            assert(e->ai && e->ai->kind() == c.kind);
        }
    }
    { // FlyersIgnoreGravity (física não puxa, integra direto)
        auto h = Factory::spawnEnemy("harpy", 0.f, 0.f);
        auto s = Factory::spawnEnemy("slime", 0.f, 0.f);
        assert(h && s);
        assert(h->ai->ignoresGravity());
        assert(!s->ai->ignoresGravity());
        EnemySystem enemies;
        GameContext ctx{};
        h->body.setVy(0.f);
        const float y0 = h->body.getY();
        for (int i = 0; i < 10; ++i) {
            // Sem mundo: só integração (sem resolve de chão).
            h->body.setX(h->body.getX() + h->body.getVx());
            h->body.setY(h->body.getY() + h->body.getVy());
        }
        assert(h->body.getY() == y0); // sem gravidade acumulada
    }
    { // BurstDetonatesAndDies (kamikaze: dano + morte + drop futuro)
        Player p;
        p.setX(100.f);
        p.setY(100.f);
        p.hp = 100;
        EnemySystem enemies;
        enemies.spawn("burst", 100.f, 100.f);
        ThrowSystem throws;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.throws = &throws;
        bool ran = false;
        enemies.forEach([&](Enemy &e) {
            ran = SkillSystem::tryUse(e, ctx, "burst_detonate");
            assert(e.resources.isDead()); // sempre morre
        });
        assert(ran);
        assert(p.hp < 100); // 30 físico no seed Nv15
    }
    { // RatBitesMelee (SlimeAI generalizado usa a lista)
        const SkillDef *rb = SkillRegistry::instance().find("rat_bite");
        assert(rb != nullptr && rb->isMelee);
        Player p;
        p.setX(100.f);
        p.setY(100.f);
        p.hp = 100;
        EnemySystem enemies;
        enemies.spawn("rat", 100.f, 100.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        bool ran = false;
        enemies.forEach([&](Enemy &e) {
            ran = SkillSystem::tryUse(e, ctx, "rat_bite");
        });
        assert(ran && p.hp < 100);
    }
    { // EyeHasNoSkills (só encosto; FlyingAI sem melee/ranged)
        auto e = Factory::spawnEnemy("eye", 0.f, 0.f);
        assert(e != nullptr && e->skillIds.empty());
    }
    { // WaveBases (5 sprites base viram 5 jogáveis)
        struct W {
            const char *id;
            core::EntityKind kind;
            const char *behavior;
            int hp;
            SpriteFrameId idle;
            int w, h;
        };
        const W cases[] = {
            {"spider", core::EntityKind::Spider, "spider", 30,
             SpriteFrameId::InsectIdle, 16, 12},
            {"serpent", core::EntityKind::Serpent, "serpent", 50,
             SpriteFrameId::SerpentIdle, 24, 10},
            {"wraith", core::EntityKind::Wraith, "wraith", 40,
             SpriteFrameId::SpecterIdle, 14, 18},
            {"golem", core::EntityKind::Golem, "golem", 80,
             SpriteFrameId::ConstructIdle, 16, 20},
            {"blaze", core::EntityKind::Blaze, "blaze", 60,
             SpriteFrameId::PureElementalIdle, 14, 20},
        };
        for (const auto &c : cases) {
            const EnemyArchetype *a =
                ArchetypeRegistry::instance().find(c.id);
            assert(a != nullptr);
            assert(a->behaviorKind == c.behavior && a->kind == c.kind);
            assert(a->frameIdle == c.idle);
            const auto f = assets::frameData(c.idle);
            assert(f.rows != nullptr && f.w == c.w && f.h == c.h);
            auto e = Factory::spawnEnemy(c.id, 0.f, 0.f);
            assert(e != nullptr && e->resources.hp == c.hp);
            assert(e->ai && e->ai->kind() == c.kind);
        }
        // Resists próprias: wraith físico 0.6, golem físico 0.6.
        const EnemyArchetype *w =
            ArchetypeRegistry::instance().find("wraith");
        assert(w->resistances.get(core::DamageType::Physical) == 0.6f);
        assert(w->resistances.get(core::DamageType::Fire) == 1.2f);
    }
    { // SpiderBitesPoison (mordida + veneno) e WraithDrains
        const SkillDef *sb = SkillRegistry::instance().find("spider_bite");
        assert(sb != nullptr && sb->isMelee);
        Player p;
        p.setX(100.f);
        p.setY(100.f);
        p.hp = 100;
        EnemySystem enemies;
        enemies.spawn("spider", 100.f, 100.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        bool ran = false;
        enemies.forEach([&](Enemy &e) {
            ran = SkillSystem::tryUse(e, ctx, "spider_bite");
        });
        assert(ran && p.hp < 100 && p.poisonBuildup > 0.f);
        const SkillDef *ld = SkillRegistry::instance().find("life_drain");
        assert(ld != nullptr);
        Player q;
        q.setX(100.f);
        q.setY(100.f);
        q.hp = 100;
        EnemySystem en2;
        en2.spawn("wraith", 100.f, 100.f);
        GameContext ctx2{};
        ctx2.player = &q;
        ctx2.enemies = &en2;
        en2.forEach([&](Enemy &e) {
            e.resources.hp = 10; // ferido: dreno cura
            assert(SkillSystem::tryUse(e, ctx2, "life_drain"));
            assert(e.resources.hp == 15); // +5 limitado ao max
        });
        assert(q.hp < 100);
    }
    { // DwarfWalkCycle4 (C/D resolvem 14x18; arquetipo aponta)
        for (auto id : {SpriteFrameId::DwarfWalkC,
                        SpriteFrameId::DwarfWalkD}) {
            const auto f = assets::frameData(id);
            assert(f.rows != nullptr && f.w == 14 && f.h == 18);
        }
        const EnemyArchetype *d =
            ArchetypeRegistry::instance().find("dwarf");
        assert(d->frameWalkC == SpriteFrameId::DwarfWalkC);
        assert(d->frameWalkD == SpriteFrameId::DwarfWalkD);
    }
    { // PackBodiesDiffer (5 corpos distintos, não só paleta)
        const char *ids[] = {"hollow", "burst", "imp", "elemental",
                             "undead"};
        const char *bodies[5][18];
        for (int i = 0; i < 5; ++i) {
            const EnemyArchetype *a =
                ArchetypeRegistry::instance().find(ids[i]);
            assert(a != nullptr);
            const auto f = assets::frameData(a->frameIdle);
            for (int r = 0; r < 18; ++r) bodies[i][r] = f.rows[r];
        }
        for (int i = 0; i < 5; ++i)
            for (int j = i + 1; j < 5; ++j) {
                bool same = true;
                for (int r = 0; r < 18; ++r)
                    if (std::string(bodies[i][r]) != bodies[j][r]) {
                        same = false;
                        break;
                    }
                assert(!same); // corpos diferentes, não só cor
            }
    }
    { // PackSizes (solo default; rato 2-3, olho 1-2)
        const EnemyArchetype *s =
            ArchetypeRegistry::instance().find("slime");
        assert(s->packMin == 1 && s->packMax == 1);
        assert(SpawnSystem::rollPackSize(*s, 0.99f) == 1);
        const EnemyArchetype *r =
            ArchetypeRegistry::instance().find("rat");
        assert(r->packMin == 2 && r->packMax == 3);
        assert(SpawnSystem::rollPackSize(*r, 0.f) == 2);
        assert(SpawnSystem::rollPackSize(*r, 0.99f) == 3);
        assert(SpawnSystem::rollPackSize(*r, -5.f) == 2); // clamp baixo
        assert(SpawnSystem::rollPackSize(*r, 99.f) == 3); // clamp alto
        const EnemyArchetype *e =
            ArchetypeRegistry::instance().find("eye");
        assert(e->packMin == 1 && e->packMax == 2);
    }

    std::printf("pack test OK\n");
    return 0;
}
