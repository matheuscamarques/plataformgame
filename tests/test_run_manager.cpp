#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Progression/DropSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Input/InputMap.h"
#include "support/Progression/RunManager.h"
#include "support/Progression/StratumManager.h"
#include "support/Effects/ThrowSystem.h"
#include "world/World.h"

static sf::Event keyEvent(sf::Event::EventType t, sf::Keyboard::Key k) {
    sf::Event e{};
    e.type = t;
    e.key.code = k;
    return e;
}

using namespace support;

// Run: morte em hp 0, restart limpa e respawna no checkpoint mais fundo,
// pause alterna (morto não pausa).
int main() {
    { // DeathOnZeroHp
        Player p;
        RunManager run;
        GameContext ctx{};
        ctx.player = &p;
        p.hp = 0;
        run.tick(1.f / 30.f, ctx);
        assert(run.isDead());
    }
    { // RestartResetsEverything (R: hp, pos, inimigos, pools)
        Player p;
        EnemySystem enemies;
        ThrowSystem throws;
        DropSystem drops;
        RunManager run;
        InputMap in;

        p.hp = 0;
        p.setX(999.f);
        enemies.spawn("slime", 0.f, 0.f);
        throws.throwItem({0.f, 0.f}, {0.f, 0.f});
        drops.spawnXP({0.f, 0.f});

        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.throws = &throws;
        ctx.drops = &drops;
        ctx.input = &in;

        run.tick(1.f / 30.f, ctx); // marca morto
        assert(run.isDead());

        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::R));
        run.tick(1.f / 30.f, ctx); // R: restart
        assert(!run.isDead());
        assert(p.hp == 100 && p.getX() == 999.f && p.getY() == 0.f); // x atual
        assert(enemies.count() == 2); // 2 slimes perto do respawn
        assert(throws.activeCount() == 0u && drops.activeCount() == 0u);
    }
    { // RespawnAtDeepestCheckpoint (mundo real: sai da rocha)
        Player p;
        EnemySystem enemies;
        StratumManager sm;
        RunManager run;
        run.setStratumManager(&sm);
        InputMap in;
        World world(1337u);

        p.setY(5000.f * 50.f);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.input = &in;
        ctx.world = &world;

        sm.tick(1.f / 30.f, ctx); // desbloqueia até estrato 5
        assert(sm.deepest() == 5);
        world.update(0, 5000);

        p.hp = 0;
        run.tick(1.f / 30.f, ctx);
        assert(run.isDead());
        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::R));
        run.tick(1.f / 30.f, ctx);
        assert(!run.isDead() && p.hp == 100);
        // No checkpoint do estrato 5 (ty 5000), fora da rocha:
        const int ty = static_cast<int>(p.getY() / 50.f);
        assert(ty <= 5000);
        const int tx = static_cast<int>(p.getCenterX() / 50.f);
        assert(!world.isSolid(tx, ty) && !world.isSolid(tx, ty - 1));
    }
    { // PauseToggle (ESC alterna; morto não pausa)
        Player p;
        RunManager run;
        InputMap in;
        GameContext ctx{};
        ctx.player = &p;
        ctx.input = &in;

        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Escape));
        run.tick(1.f / 30.f, ctx);
        assert(run.isPaused());

        in.beginFrame(); // sem evento: edge morreu, mantém
        run.tick(1.f / 30.f, ctx);
        assert(run.isPaused());

        // Novo edge: solta, vira frame, aperta de novo.
        in.handleEvent(keyEvent(sf::Event::KeyReleased, sf::Keyboard::Escape));
        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Escape));
        run.tick(1.f / 30.f, ctx);
        assert(!run.isPaused());

        p.hp = 0;
        in.handleEvent(keyEvent(sf::Event::KeyReleased, sf::Keyboard::Escape));
        in.beginFrame(); // edge do ESC morreu antes da morte
        run.tick(1.f / 30.f, ctx); // morreu
        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Escape));
        run.tick(1.f / 30.f, ctx);
        assert(run.isDead() && !run.isPaused());
    }
    { // RWhileAliveRespawns (R vivo = reset no checkpoint atual)
        Player p;
        EnemySystem enemies;
        RunManager run;
        InputMap in;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.input = &in;

        p.hp = 40;
        p.setX(500.f);
        enemies.spawn("slime", 0.f, 0.f);
        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::R));
        run.tick(1.f / 30.f, ctx);
        assert(!run.isDead() && p.hp == 100 && enemies.count() == 2);
    }

    std::printf("run manager test OK\n");
    return 0;
}
