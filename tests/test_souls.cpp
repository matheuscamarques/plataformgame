#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/GameContext.h"
#include "support/Progression/DropSystem.h"
#include "support/Progression/RunManager.h"

// F1 Souls: XP vira carteira, morte derruba mancha, R conserva.
int main() {
    using namespace support;

    { // CollectCreditsSouls (orbe no pé: carteira + estatística)
        Player p;
        DropSystem drops;
        GameContext ctx{};
        ctx.player = &p;
        ctx.drops = &drops;
        assert(p.souls == 0);
        drops.spawnXP({p.getCenterX(), p.getCenterY()}, 30);
        for (int i = 0; i < 5; ++i) drops.tick(1.f / 30.f, ctx);
        assert(p.souls == 30);
        assert(drops.totalCollected() == 30);
        assert(drops.activeCount() == 0u);
    }
    { // DeathDropsStain (500 some da carteira e nasce 1 orbe)
        Player p;
        DropSystem drops;
        RunManager run;
        p.souls = 500;
        p.hp = 0;
        GameContext ctx{};
        ctx.player = &p;
        ctx.drops = &drops;
        run.tick(1.f / 30.f, ctx);
        assert(run.isDead());
        assert(p.souls == 0);
        assert(drops.activeCount() == 1u);
    }
    { // DeathBrokeNoStain (liso: sem orbe)
        Player p;
        DropSystem drops;
        RunManager run;
        p.hp = 0;
        GameContext ctx{};
        ctx.player = &p;
        ctx.drops = &drops;
        run.tick(1.f / 30.f, ctx);
        assert(run.isDead() && drops.activeCount() == 0u);
    }
    { // RestartKeepsWalletAlive (R vivo não mexe nas souls)
        Player p;
        DropSystem drops;
        RunManager run;
        p.souls = 500;
        GameContext ctx{};
        ctx.player = &p;
        ctx.drops = &drops;
        run.restart(ctx);
        assert(p.souls == 500);
        assert(drops.activeCount() == 0u);
    }
    { // RestartMovesStain (morreu + R: carteira 0, 1 orbe no spawn)
        Player p;
        DropSystem drops;
        RunManager run;
        p.souls = 500;
        p.hp = 0;
        GameContext ctx{};
        ctx.player = &p;
        ctx.drops = &drops;
        run.tick(1.f / 30.f, ctx); // morte: mancha no cadáver
        assert(drops.activeCount() == 1u);
        run.restart(ctx); // clear limpa + mancha renasce no spawn
        assert(p.souls == 0);
        assert(drops.activeCount() == 1u);
    }
    { // RecoverThenRestart (recuperou no pé + R: zera e renasce)
        Player p;
        DropSystem drops;
        RunManager run;
        p.souls = 500;
        p.hp = 0;
        GameContext ctx{};
        ctx.player = &p;
        ctx.drops = &drops;
        run.tick(1.f / 30.f, ctx);
        p.souls = 500; // simulou recuperar a mancha no pé
        run.restart(ctx);
        assert(p.souls == 0);
        assert(drops.activeCount() == 1u); // sem duplicar
    }

    std::printf("souls test OK\n");
    return 0;
}
