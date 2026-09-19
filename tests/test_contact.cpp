#include <cassert>
#include <cstdio>
#include <SFML/Graphics/Rect.hpp>
#include "entities/player/player.h"
#include "support/ContactDamageSystem.h"
#include "support/EnemySystem.h"
#include "support/GameContext.h"

// Contato: 10 de dano, i-frame 0.6s, empurrão, morto não bate.
int main() {
    using namespace support;

    { // ContactDamagesOnceThenIframesHold
        Player p; // (0,0) 50x50
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f); // sobreposto

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        cs.tick(1.f / 30.f, ctx);
        assert(p.hp == 90);
        // Recola o slime (foi ejetado) e encosta de novo: i-frame segura.
        enemies.forEach([](Slime &s) { s.body.setX(10.f); s.body.setY(10.f); });
        cs.tick(1.f / 30.f, ctx);
        assert(p.hp == 90); // i-frame
    }
    { // IframesExpireThenDamageAgain + pushback
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f); // à direita do centro

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        cs.tick(1.f / 30.f, ctx);
        assert(p.hp == 90 && p.getX() == -6.f); // empurrado p/ longe
        p.hurtIframes.tick(1.f);
        // Recola o slime e encosta de novo.
        enemies.forEach([](Slime &s) { s.body.setX(10.f); s.body.setY(10.f); });
        p.setX(0.f);
        cs.tick(1.f / 30.f, ctx);
        assert(p.hp == 80);
    }
    { // DeadSlimeNoDamage + NoOverlapNoDamage
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);
        enemies.spawn("slime", 500.f, 500.f);
        enemies.forEach([](Slime &s) {
            if (s.body.getX() < 100.f) s.resources.takeDamage(9999);
        });

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        cs.tick(1.f / 30.f, ctx);
        assert(p.hp == 100);
    }

    { // SeparatesSlimeOut (1 tick: rects não se tocam mais)
        Player p; // (0,0) 50x50
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f); // sobreposto

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        cs.tick(1.f / 30.f, ctx);
        bool overlap = false;
        enemies.forEach([&](Slime &s) {
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
