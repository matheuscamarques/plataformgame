#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Combat/MeleeSystem.h"

// Melee K: windup→active→recovery, 1 hit por swing, combo encadeia.
int main() {
    using namespace support;

    { // HitsSlimeInFrontOnce (30 → 22, sem duplo hit no swing)
        Player p; // (0,0) 30x50 facing 1 → hitbox combo0 x=[30,46]
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f); // 40x30 sobre a hitbox [30,46]

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        assert(p.startSwing());
        for (int i = 0; i < 6; ++i) ms.tick(1.f / 30.f, ctx);
        int hp = -1;
        enemies.forEach([&](Enemy &s) { hp = s.resources.hp; });
        assert(hp == 22);
    }
    { // WhiffsWhenFar
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 500.f, 500.f);

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        assert(p.startSwing());
        for (int i = 0; i < 6; ++i) ms.tick(1.f / 30.f, ctx);
        int hp = -1;
        enemies.forEach([&](Enemy &s) { hp = s.resources.hp; });
        assert(hp == 30);
    }
    { // ChainsComboInRecovery (combo 0 → 1, dano 10)
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        assert(p.startSwing());
        // Avança até Recovery do combo 0.
        for (int i = 0; i < 20 && p.meleePhase != MeleePhase::Recovery; ++i)
            ms.tick(1.f / 30.f, ctx);
        assert(p.meleePhase == MeleePhase::Recovery);
        assert(p.startSwing() && p.meleeCombo == 1);
        // Deixa o combo 1 acertar (slime ainda vivo com 22).
        for (int i = 0; i < 10; ++i) ms.tick(1.f / 30.f, ctx);
        int hp = -1;
        enemies.forEach([&](Enemy &s) { hp = s.resources.hp; });
        assert(hp == 12); // 22 - 10
    }
    { // MidSwingIgnoresNewPress
        Player p;
        assert(p.startSwing());
        assert(!p.startSwing()); // ainda em Windup
    }
    { // HurtGatesOnIframes
        Player p;
        assert(p.hp == 100);
        assert(p.hurt(10) && p.hp == 90);
        assert(!p.hurt(10) && p.hp == 90); // i-frame segurou
        p.hurtIframes.tick(1.f);
        assert(p.hurt(90) && p.hp == 0);
        assert(!p.hurt(10)); // já em 0, sem efeito
    }

    std::printf("melee test OK\n");
    return 0;
}
