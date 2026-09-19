#include <cassert>
#include <cstdio>
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
        // Recoloca sobre o slime (foi empurrado) e encosta de novo.
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

    std::printf("contact test OK\n");
    return 0;
}
