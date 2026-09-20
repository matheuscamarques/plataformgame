#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Enemies/SpawnSystem.h"
#include "world/Generation.h"
#include "world/World.h"

// Player assentado no solo (pés no topo do chão).
static void settle(Player &p, support::World &w, int tx) {
    int s = support::surfaceHeight(tx, 1337u);
    p.setX(static_cast<float>(tx) * 50.f);
    p.setY(static_cast<float>(s - 1) * 50.f);
    w.update(tx, s);
}

// Spawn: budget por estrato, intervalo, chão sólido, despawn longe.
int main() {
    using namespace support;

    { // BudgetTable (densidade cai com profundidade)
        assert(SpawnSystem::budgetForStratum(0) == 3);
        assert(SpawnSystem::budgetForStratum(1) == 4);
        assert(SpawnSystem::budgetForStratum(2) == 4);
        assert(SpawnSystem::budgetForStratum(5) == 2);
        assert(SpawnSystem::budgetForStratum(10) == 2);
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

        // Budget real S0=3: roda até encher (scan tem sorte; tabela é lei).
        for (int i = 0; i < 2400 && enemies.count() < 3u; ++i)
            ss.tick(1.f / 30.f, ctx);
        // S0 budget 3 (parte de 0) → exatamente 3.
        assert(enemies.count() == 3u);
        for (int i = 0; i < 200; ++i) ss.tick(1.f / 30.f, ctx);
        assert(enemies.count() == 3u); // não passa do budget
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
            const int tx = static_cast<int>(s.body.getX() / 50.f);
            const int ty = static_cast<int>(s.body.getY() / 50.f);
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
