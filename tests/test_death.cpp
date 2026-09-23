/**
 * @file tests/test_death.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava remover morto, manter vivo e dropar XP.
 * @details Cobre DeathSystem e DropSystem, roda com make test que compila em build/tests/test_death.
 */

#include <cassert>
#include <cstdio>
#include "support/Combat/DeathSystem.h"
#include "support/Progression/DropSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// Death: remove morto, mantém vivo, dropa XP (sistemas reais, sem mock).
int main() {
    using namespace support;

    { // RemovesDeadEnemy
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);
        assert(enemies.count() == 1u);
        enemies.forEach([](Enemy &s) { s.resources.takeDamage(9999); });

        GameContext ctx{};
        ctx.enemies = &enemies;

        DeathSystem ds;
        ds.tick(0.f, ctx);
        assert(enemies.count() == 0u);
    }
    { // KeepsAliveEnemy
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);

        GameContext ctx{};
        ctx.enemies = &enemies;

        DeathSystem ds;
        ds.tick(0.f, ctx);
        assert(enemies.count() == 1u);
    }
    { // SpawnsDropOnDeath (sem particles: null-safe)
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);
        enemies.forEach([](Enemy &s) { s.resources.takeDamage(9999); });

        DropSystem drops;
        DeathSystem ds;
        ds.setDropSystem(&drops);

        GameContext ctx{};
        ctx.enemies = &enemies;

        ds.tick(0.f, ctx);
        assert(enemies.count() == 0u);
        assert(drops.activeCount() == 1u);
    }

    std::printf("death test OK\n");
    return 0;
}
