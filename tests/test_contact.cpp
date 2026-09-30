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
#include "support/Combat/BodySystem.h"
#include "support/Combat/ContactDamageSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// Contato com telegraph 0.35s: morde após ~11 ticks colado (recolando
// a cada tick, pois a separação ejeta). Separação em 1 tick.
int main() {
    using namespace support;
    auto recol = [](EnemySystem &e) {
        e.forEach([](Enemy &s) { s.body.setX(20.f); s.body.setY(10.f); });
    };

    { // DamagesAfterWindup + pushback (11 ticks colado → hp 91, x -6)
        Player p; // (0,0) 60x100, centro x=30; slime à direita (centro 40)
        EnemySystem enemies;
        enemies.spawn("slime", 20.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 5; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        assert(p.hp == 100); // windup (~0.18s) ainda não esgotou
        for (int i = 0; i < 6; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        // Contato 10 × universal Nv1 (1.0) = 10
        assert(p.hp == 90 && p.getX() == -6.f);
    }
    { // DeadSlimeNoDamage + NoOverlapNoDamage
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);
        enemies.spawn("slime", 500.f, 500.f);
        enemies.forEach([](Enemy &s) {
            if (s.body.getX() < 100.f) (void)s.resources.takeDamage(9999);
        });

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        cs.tick(1.f / 30.f, ctx);
        assert(p.hp == 100);
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
    { // PixelHurtbox (arte 30px, não AABB 60px: fantasma não morde)
        // Player (0,0) Idle: arte x[15,45]. Slime 40x30 à esquerda com
        // AABB invadindo o player ([-25,15] cruza x=0) mas arte em
        // x[-22,12]: 30 ticks sem dano. Encostando a arte, morde.
        Player p; // Idle default, facing E
        EnemySystem enemies;
        enemies.spawn("slime", -25.f, 90.f);
        enemies.forEach([](Enemy &s) {
            s.currentFrameId = support::SpriteFrameId::SlimeIdle;
        });

        BodySystem bs;
        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 30; ++i) {
            bs.tick(1.f / 30.f, ctx); // ordem de produção: corpos antes
            cs.tick(1.f / 30.f, ctx);
        }
        assert(p.hp == 100); // AABB encosta, pixel não: sem mordida
        float sx = 0.f;
        enemies.forEach([&](Enemy &s) { sx = s.body.getX(); });
        assert(sx == -25.f); // sem contato: sem separação

        auto touch = [](EnemySystem &e) {
            e.forEach([](Enemy &s) {
                s.body.setX(0.f);
                s.body.setY(90.f);
            });
        };
        for (int i = 0; i < 30; ++i) {
            touch(enemies); // recolado: separação ejeta a cada tick
            bs.tick(1.f / 30.f, ctx);
            cs.tick(1.f / 30.f, ctx);
        }
        assert(p.hp < 100); // arte com arte: morde após o windup
    }

    std::printf("contact test OK\n");
    return 0;
}
