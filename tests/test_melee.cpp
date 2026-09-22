#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Combat/MeleeSystem.h"

// Melee K: windup→active→recovery, 1 hit por swing, combo encadeia.
int main() {
    using namespace support;

    { // HitsSlimeInFrontOnce (head 2x: 30 → 14, sem duplo hit)
        Player p; // (0,0) 30x50 facing 1 → soco combo0 x=[30,46] y=[15,35]
        p.equipment.unequip(core::EquipSlot::RightHand); // soco (seed equipa espada)
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f); // 40x30 sobre a hitbox
        // Body posicionado (como BodySystem 250 faz em jogo): hitbox
        // toca Head+Torso+ArmL → best=head → 8 * 2.0 = 16.
        enemies.forEach([](Enemy &s) {
            s.bodyParts.rebuild({s.body.getX(), s.body.getY()}, 1);
        });

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        assert(p.startSwing());
        for (int i = 0; i < 6; ++i) ms.tick(1.f / 30.f, ctx);
        int hp = -1;
        enemies.forEach([&](Enemy &s) { hp = s.resources.hp; });
        assert(hp == 14);
    }
    { // HitsSlimeBehindWhenFacingLeft (regressão: W ia p/ direita)
        Player p; // (100,0) 30x50, centro (115,25)
        p.equipment.unequip(core::EquipSlot::RightHand); // soco (seed equipa espada)
        p.setX(100.f);
        p.facing = -1;
        p.aimDir = support::AimDir::W; // tecla esquerda = esquerda da tela
        EnemySystem enemies;
        enemies.spawn("slime", 60.f, 10.f); // [60,100] sobre a hitbox [85,105]

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        assert(p.startSwing());
        assert(p.swingAim == support::AimDir::W);
        // Hitbox [84,100]x[15,35] toca Head+Torso+ArmR+LegR → best=head.
        enemies.forEach([](Enemy &s) {
            s.bodyParts.rebuild({s.body.getX(), s.body.getY()}, 1);
        });
        for (int i = 0; i < 6; ++i) ms.tick(1.f / 30.f, ctx);
        int hp = -1;
        enemies.forEach([&](Enemy &s) { hp = s.resources.hp; });
        assert(hp == 14);
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
    { // ChainsComboInRecovery (combo 0 → 1; combo1 head overkilla)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand); // soco (seed equipa espada)
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        enemies.forEach([](Enemy &s) {
            s.bodyParts.rebuild({s.body.getX(), s.body.getY()}, 1);
        });

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        assert(p.startSwing());
        // Avança até Recovery do combo 0.
        for (int i = 0; i < 20 && p.meleePhase != MeleePhase::Recovery; ++i)
            ms.tick(1.f / 30.f, ctx);
        assert(p.meleePhase == MeleePhase::Recovery);
        int hp0 = -1;
        enemies.forEach([&](Enemy &s) { hp0 = s.resources.hp; });
        assert(hp0 == 14); // combo0 head: 8 * 2.0 = 16
        assert(p.startSwing() && p.meleeCombo == 1);
        // Combo1 head (10 * 2.0 = 20) overkilla os 14 restantes.
        for (int i = 0; i < 10; ++i) ms.tick(1.f / 30.f, ctx);
        int hp = -1;
        bool dead = false;
        enemies.forEach([&](Enemy &s) {
            hp = s.resources.hp;
            dead = s.resources.isDead();
        });
        assert(hp == 0 && dead);
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
