#include <cassert>
#include <cmath>
#include <cstdio>
#include "entities/player/player.h"
#include "support/BehaviorRegistry.h"
#include "support/DeathSystem.h"
#include "support/DwarfAI.h"
#include "support/EnemySystem.h"
#include "support/GameContext.h"
#include "support/SpawnSystem.h"
#include "support/ThrowSystem.h"

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
    { // PickKindByStratum (puro, determinístico)
        assert(SpawnSystem::pickKind(1, 0.5f) == "slime");
        assert(SpawnSystem::pickKind(2, 0.0f) == "slime");
        assert(SpawnSystem::pickKind(3, 0.1f) == "dwarf");
        assert(SpawnSystem::pickKind(3, 0.2f) == "slime");
        assert(SpawnSystem::pickKind(5, 0.29f) == "dwarf");
        assert(SpawnSystem::pickKind(5, 0.5f) == "slime");
        EnemySystem enemies;
        assert(!SpawnSystem::hasLiveDwarf(enemies));
        enemies.spawn("dwarf", 0.f, 0.f);
        assert(SpawnSystem::hasLiveDwarf(enemies));
        enemies.forEach([](Enemy &s) { s.resources.takeDamage(9999); });
        assert(!SpawnSystem::hasLiveDwarf(enemies)); // morto não conta
    }

    std::printf("dwarf test OK\n");
    return 0;
}
