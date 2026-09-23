/**
 * @file tests/test_itempickup.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava coleta integrando morte, explosão e inventário.
 * @details Cobre DropTable e World, roda com make test que compila em build/tests/test_itempickup.
 */

#include <cassert>
#include <cstdio>

#include "core/Config.h"
#include "core/DropTable.h"
#include "entities/Player/Player.h"
#include "support/Combat/DeathSystem.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Progression/DropSystem.h"
#include "world/World.h"

// Integração fases 2+3: morte dropa, explosão dropa bloco, TNT consome
// pilha, inventário cheio deixa sobra. Headless (sem GL).
int main() {
    using namespace support;

    { // RollDeterministico (mesmo salt = mesmo resultado; chance respeitada)
        core::DropTable t;
        t.entries.push_back({"slime_gel", 0.8f, 1, 2});
        t.entries.push_back({"iron_ore", 0.0f, 1, 1}); // nunca
        const auto a = core::rollDrops(t, 1, 1234u);
        const auto b = core::rollDrops(t, 1, 1234u);
        assert(a == b);
        for (const auto& [id, qty] : a) {
            assert(id == "slime_gel");
            assert(qty >= 1 && qty <= 2);
        }
        assert(core::rollDrops(t, 999, 1234u).empty()); // fora do nível
    }
    { // MorteDroppa (archetype de teste 100%: 1 orb com qty 2)
        EnemyArchetype arch;
        arch.color = {0, 200, 0};
        arch.hitboxSize = {36.f, 44.f};
        arch.behaviorKind = "slime";
        arch.kind = core::EntityKind::Slime;
        arch.drops.entries.push_back({"slime_gel", 1.f, 2, 2});
        ArchetypeRegistry::instance().add("test_dummy_gel", arch);

        EnemySystem enemies;
        enemies.spawn("test_dummy_gel", 500.f, 500.f);
        assert(enemies.count() == 1u);
        enemies.forEach([](Enemy& e) { e.resources.hp = 0; });

        DropSystem drops;
        DeathSystem deaths;
        deaths.setDropSystem(&drops);
        GameContext ctx{};
        ctx.enemies = &enemies;
        deaths.tick(1.f / 30.f, ctx);
        assert(enemies.count() == 0u);
        assert(drops.activeItemCount() == 1u);
    }
    { // ExplosaoDroppaBloco (pedra vira orb via dropId)
        World world(1337u);
        world.update(0, 26);
        // Acha uma pedra próxima da superfície para explodir.
        int stx = -1, sty = -1;
        for (int ty = 20; ty < 60 && stx < 0; ++ty)
            for (int tx = -40; tx < 40; ++tx)
                if (world.tileAt(tx, ty) == Tile::Stone) {
                    stx = tx;
                    sty = ty;
                    break;
                }
        assert(stx >= 0); // mundo de teste tem pedra acessível
        DropSystem drops;
        ExplosionSystem boom;
        boom.setDropSystem(&drops);
        GameContext ctx{};
        ctx.world = &world;
        ExplosionDef def;
        def.radius = 40.f;
        def.damage = 0;
        def.tilesRadius = 1; // mínimo: quebra a pedra alvo
        boom.explode({(stx + 0.5f) * core::kBlockSize,
                      (sty + 0.5f) * core::kBlockSize},
                     def, ctx);
        assert(drops.activeItemCount() >= 1u); // pedra dropou
    }
    { // ArremessoConsomePilha (migração TNT: sem campo avulso)
        Player p;
        ThrowSystem ts;
        assert(p.inventory.count("dynamite") == 999);
        assert(p.tryThrow(ts));
        assert(p.inventory.count("dynamite") == 998);
        p.inventory.remove("dynamite", 998);
        assert(!p.tryThrow(ts)); // pilha vazia: sem arremesso
        assert(ts.activeCount() == 1u);
    }
    { // InventarioCheioDeixaSobra (coleta parcial não perde item)
        DropSystem drops;
        Player p;
        p.inventory = core::Inventory{}; // zera o kit do ctor
        for (int i = 0; i < 40; ++i) p.inventory.add({"stone", 99});
        assert(p.inventory.usedSlots() == 40);
        GameContext ctx{};
        ctx.player = &p;
        drops.spawnItem("stone", 200, {p.getCenterX(), p.getCenterY()});
        for (int i = 0; i < 60; ++i) drops.tick(1.f / 30.f, ctx);
        assert(p.inventory.count("stone") == 40 * 99);
        assert(drops.activeItemCount() == 1u); // sobra ficou no chão
    }

    std::printf("itempickup test OK\n");
    return 0;
}
