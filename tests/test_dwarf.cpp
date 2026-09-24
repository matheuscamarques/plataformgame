/**
 * @file tests/test_dwarf.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava IA completa do anão com patrulha e ataque.
 * @details Cobre DwarfAI e SpawnSystem, roda com make test que compila em build/tests/test_dwarf.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>
#include "entities/Player/Player.h"
#include "support/Enemies/BehaviorRegistry.h"
#include "support/Combat/DeathSystem.h"
#include "support/Enemies/DwarfAI.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Enemies/SpawnSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "world/Generation.h"
#include "world/World.h"

// Anão básico: registro, factory Elite, patrulha, aggro, throw, melee, morte.
int main() {
    using namespace support;
    // Trava Y (sem mundo não há chão; X livre p/ patrulha/aproximação).
    auto pinY = [](EnemySystem &e) {
        e.forEach([](Enemy &s) { s.body.setY(100.f); s.body.setVy(0.f); });
    };
    auto dwarfState = [](EnemySystem &e) {
        DwarfState st = DwarfState::Patrol;
        e.forEach([&](Enemy &s) {
            if (auto *d = dynamic_cast<DwarfAI *>(s.ai.get())) st = d->state();
        });
        return st;
    };

    { // RegisteredInBehaviorRegistry
        auto b = BehaviorRegistry::instance().create("dwarf");
        assert(b != nullptr && std::string(b->name()) == "DwarfAI");
        assert(BehaviorRegistry::instance().create("nope") == nullptr);
    }
    { // FactoryInitializesEliteResources
        auto d = Factory::spawnEnemy("dwarf", 0.f, 0.f);
        assert(d != nullptr);
        assert(d->resources.hp == 60 && d->resources.hpMax == 60);
        assert(!d->resources.isTrash);
        assert(d->bodyParts.schema != nullptr);
    }
    { // PatrolStaysNearHome (player longe, 5s)
        Player p;
        p.setX(2000.f);
        p.setY(100.f);
        EnemySystem enemies;
        enemies.spawn("dwarf", 100.f, 100.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        for (int i = 0; i < 150; ++i) { pinY(enemies); enemies.tick(1.f / 30.f, ctx); }
        float hx = -1.f;
        enemies.forEach([&](Enemy &s) { hx = s.body.getX(); });
        assert(std::fabs(hx - 100.f) <= 106.f);
        assert(dwarfState(enemies) == DwarfState::Patrol);
    }
    { // AggroOnPlayerInRange + NoAggroFar
        Player p;
        EnemySystem enemies;
        enemies.spawn("dwarf", 100.f, 100.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        p.setX(600.f);
        p.setY(100.f); // dx=500: fora
        pinY(enemies);
        enemies.tick(1.f / 30.f, ctx);
        assert(dwarfState(enemies) == DwarfState::Patrol);

        p.setX(250.f); // dx=150: dentro
        pinY(enemies);
        enemies.tick(1.f / 30.f, ctx);
        assert(dwarfState(enemies) == DwarfState::Alert);
    }
    { // ThrowDeliversOnceThenCooldownBlocks
        Player p;
        p.setX(200.f);
        p.setY(100.f);
        EnemySystem enemies;
        enemies.spawn("dwarf", 100.f, 100.f);
        ThrowSystem throws;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.throws = &throws;
        for (int i = 0; i < 30; ++i) { pinY(enemies); enemies.tick(1.f / 30.f, ctx); }
        assert(throws.activeCount() == 1u);
        for (int i = 0; i < 20; ++i) { pinY(enemies); enemies.tick(1.f / 30.f, ctx); }
        assert(throws.activeCount() == 1u); // cooldown 1.8s segura
    }
    { // MeleeHitsInRange
        Player p;
        p.setX(120.f);
        p.setY(100.f);
        EnemySystem enemies;
        enemies.spawn("dwarf", 100.f, 100.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        for (int i = 0; i < 35; ++i) { pinY(enemies); enemies.tick(1.f / 30.f, ctx); }
        assert(p.hp == 88); // 100 - 12, exatamente 1 hit
    }
    { // MeleeMissesOutOfRange (sai no meio do windup)
        Player p;
        p.setX(120.f);
        p.setY(100.f);
        EnemySystem enemies;
        enemies.spawn("dwarf", 100.f, 100.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        for (int i = 0; i < 15; ++i) { pinY(enemies); enemies.tick(1.f / 30.f, ctx); }
        p.setX(280.f); // dx=180: longe do golpe, perto p/ não recuar
        for (int i = 0; i < 15; ++i) { pinY(enemies); enemies.tick(1.f / 30.f, ctx); }
        assert(p.hp == 100);
    }
    { // DeathRemovesDwarf (caminho genérico do DeathSystem)
        EnemySystem enemies;
        enemies.spawn("dwarf", 0.f, 0.f);
        enemies.forEach([](Enemy &s) { s.resources.takeDamage(9999); });
        DeathSystem ds;
        GameContext ctx{};
        ctx.enemies = &enemies;
        ds.tick(0.f, ctx);
        assert(enemies.count() == 0u);
    }
    { // ArchetypeDataDrivesSpawn (dwarf S3+, peso 0.3, cap via maxAlive)
        const EnemyArchetype *d = ArchetypeRegistry::instance().find("dwarf");
        assert(d != nullptr);
        assert(d->minStratum == 3 && d->maxStratum == 99);
        assert(d->maxAlive == 1);
        const EnemyArchetype *s = ArchetypeRegistry::instance().find("slime");
        assert(s != nullptr && s->minStratum == 0);
    }
    { // DwarfCapBlocksSecond (1 vivo → novos spawns são slime)
        Player p;
        int s0 = support::surfaceHeight(0, 1337u);
        p.setX(0.f);
        p.setY(static_cast<float>(s0 - 1) * core::kBlockSize);
        EnemySystem enemies;
        SpawnSystem ss;
        World world(1337u);
        world.update(0, s0);
        enemies.spawn("dwarf", 0.f, 0.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.world = &world;
        auto dwarfs = [&]() {
            int n = 0;
            enemies.forEach([&](Enemy &e) {
                if (e.ai && e.ai->kind() == core::EntityKind::Dwarf &&
                    !e.resources.isDead())
                    ++n;
            });
            return n;
        };
        for (int i = 0; i < 1200 && enemies.count() < 2u; ++i)
            ss.tick(1.f / 30.f, ctx);
        assert(enemies.count() >= 2u); // slimes chegaram
        assert(dwarfs() == 1);         // nenhum anão novo (cap)
    }

    std::printf("dwarf test OK\n");
    return 0;
}
