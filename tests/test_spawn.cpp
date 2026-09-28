/**
 * @file tests/test_spawn.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava player assentado com faixa, peso e teto.
 * @details Cobre SpawnSystem e surfaceHeight, roda com make test que compila em build/tests/test_spawn.
 */

#include <cassert>
#include <cstdio>
#include "core/Config.h"
#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Enemies/SpawnSystem.h"
#include "world/Generation.h"
#include "world/World.h"

// Player assentado no solo (pés no topo do chão).
static void settle(Player &p, support::World &w, int tx) {
    int s = support::surfaceHeight(tx, 1337u);
    p.setX(static_cast<float>(tx) * core::kBlockSize);
    p.setY(static_cast<float>(s - 1) * core::kBlockSize);
    w.update(tx, s);
}

// Spawn: budget por estrato, intervalo, chão sólido, despawn longe.
int main() {
    using namespace support;

    { // BudgetTable (hordas: 100 por estrato, teto global 100)
        assert(SpawnSystem::budgetForStratum(0) == 100);
        assert(SpawnSystem::budgetForStratum(1) == 100);
        assert(SpawnSystem::budgetForStratum(2) == 100);
        assert(SpawnSystem::budgetForStratum(5) == 100);
        assert(SpawnSystem::budgetForStratum(10) == 100);
        // Tarô cósmico: Diabo ×1.5, Lua dobra a noite, sangue dobra.
        assert(SpawnSystem::scaledBudget(100, 1.5f, 1.f) == 150);
        assert(SpawnSystem::scaledBudget(100, 1.f, 2.f) == 200);
        assert(SpawnSystem::scaledBudget(2, 1.5f, 1.5f) == 4);
        assert(SpawnSystem::scaledBudget(100, 1.f, 1.f) == 100);
    }
    { // SpawnsUpToBudgetThenStops (mundo real, com chão)
        Player p;
        EnemySystem enemies;
        SpawnSystem ss;
        World world(1337u);
        settle(p, world, 0);

        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.world = &world;

        // Budget real S0=100 (hordas): roda até encher.
        for (int i = 0; i < 24000 && enemies.count() < 100u; ++i)
            ss.tick(1.f / 30.f, ctx);
        // S0 budget 100 (parte de 0) → exatamente 100 (pack respeita).
        assert(enemies.count() == 100u);
        for (int i = 0; i < 200; ++i) ss.tick(1.f / 30.f, ctx);
        assert(enemies.count() == 100u); // não passa do budget
    }
    { // SpawnedOnGroundNotInsideRock
        Player p;
        EnemySystem enemies;
        SpawnSystem ss;
        World world(1337u);
        settle(p, world, 0);

        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.world = &world;

        for (int i = 0; i < 900 && enemies.count() == 0u; ++i)
            ss.tick(1.f / 30.f, ctx); // até 20 janelas: alguma acerta chão
        assert(enemies.count() > 0u);
        enemies.forEach([&](Enemy &s) {
            const int tx = static_cast<int>(s.body.getX() / core::kBlockSize);
            const int ty = static_cast<int>(s.body.getY() / core::kBlockSize);
            // Tile do slime livre (pés caem na física depois, sem enterrar).
            assert(!world.isSolid(tx, ty));
        });
    }
    { // DespawnFarRemoves (economia todo tick, sem timer)
        Player p;
        EnemySystem enemies;
        SpawnSystem ss;
        p.setX(0.f);
        p.setY(0.f);
        enemies.spawn("slime", 0.f, 0.f);
        enemies.spawn("slime", 5000.f, 5000.f);

        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        ss.tick(1.f / 30.f, ctx);
        assert(enemies.count() == 1u);
    }
    { // NoPlayerNoCrash (sistema tolera ctx vazio)
        EnemySystem enemies;
        SpawnSystem ss;
        GameContext ctx{};
        ctx.enemies = &enemies;
        ss.tick(1.f / 30.f, ctx);
        assert(enemies.count() == 0u);
    }
    { // IntervalGatesSpawns (1 tick isolado não spawna)
        Player p;
        EnemySystem enemies;
        SpawnSystem ss;
        World world(1337u);
        settle(p, world, 0);

        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.world = &world;

        ss.tick(1.f / 30.f, ctx);
        assert(enemies.count() == 0u); // 1.5s ainda não passou
    }

    std::printf("spawn test OK\n");
    return 0;
}
