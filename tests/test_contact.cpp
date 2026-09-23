/**
 * @file tests/test_contact.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava contato com telegraph 0,35s e separação em 1 tick.
 * @details Cobre ContactDamageSystem colado, roda com make test que compila em build/tests/test_contact.
 */

#include <cassert>
#include <cstdio>
#include <SFML/Graphics/Rect.hpp>
#include "entities/Player/Player.h"
#include "support/Combat/ContactDamageSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// Contato com telegraph 0.35s: morde após ~11 ticks colado (recolando
// a cada tick, pois a separação ejeta). Separação em 1 tick.
int main() {
    using namespace support;
    auto recol = [](EnemySystem &e) {
        e.forEach([](Enemy &s) { s.body.setX(10.f); s.body.setY(10.f); });
    };

    { // DamagesAfterWindup + pushback (11 ticks colado → hp 90, x -6)
        Player p; // (0,0) 50x50
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 5; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        assert(p.hp == 10000); // windup (~0.18s) ainda não esgotou
        for (int i = 0; i < 6; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        assert(p.hp == 9990 && p.getX() == -6.f);
    }
    { // DeadSlimeNoDamage + NoOverlapNoDamage
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);
        enemies.spawn("slime", 500.f, 500.f);
        enemies.forEach([](Enemy &s) {
            if (s.body.getX() < 100.f) s.resources.takeDamage(9999);
        });

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        cs.tick(1.f / 30.f, ctx);
        assert(p.hp == 10000);
    }
    { // SeparatesSlimeOut (1 tick: rects não se tocam mais)
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        cs.tick(1.f / 30.f, ctx);
        bool overlap = false;
        enemies.forEach([&](Enemy &s) {
            const sf::FloatRect sb{s.body.getX(), s.body.getY(),
                                   s.body.getW(), s.body.getH()};
            const sf::FloatRect pb{p.getX(), p.getY(), p.getW(), p.getH()};
            if (pb.intersects(sb)) overlap = true;
        });
        assert(!overlap);
    }

    std::printf("contact test OK\n");
    return 0;
}
