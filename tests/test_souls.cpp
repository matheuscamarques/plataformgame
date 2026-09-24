#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Combat/DeathSystem.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"
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
    { // SlimeDeathPays100 (XP por arquétipo; slime 100, anão 150)
        const EnemyArchetype* slime =
            ArchetypeRegistry::instance().find("slime");
        const EnemyArchetype* dwarf =
            ArchetypeRegistry::instance().find("dwarf");
        assert(slime && slime->xp == 100);
        assert(dwarf && dwarf->xp == 150);
        Player p;
        EnemySystem enemies;
        DropSystem drops;
        DeathSystem deaths;
        deaths.setDropSystem(&drops);
        enemies.spawn("slime", p.getX(), p.getY());
        enemies.forEach([](Enemy& e) { e.resources.hp = 0; });
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.drops = &drops;
        deaths.tick(1.f / 30.f, ctx);
        for (int i = 0; i < 60; ++i) drops.tick(1.f / 30.f, ctx);
        assert(p.souls >= 100); // 100 do XP (+soul item se rolou)
    }
    { // SoulUse (almas consumíveis viram souls na hora)
        Player p;
        const core::ItemDef* lost =
            core::ItemRegistry::instance().find("soul_lost");
        const core::ItemDef* great =
            core::ItemRegistry::instance().find("soul_great");
        assert(lost && great && lost->onUse && great->onUse);
        lost->onUse(p);
        assert(p.souls == 50);
        great->onUse(p);
        assert(p.souls == 250);
    }

    std::printf("souls test OK\n");
    return 0;
}
