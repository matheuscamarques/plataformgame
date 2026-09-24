/**
 * @file tests/test_melee.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava windup active recovery com 1 hit e combo.
 * @details Cobre MeleeSystem e head 2x, roda com make test que compila em build/tests/test_melee.
 */

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
        assert(hp == 44);
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
        assert(hp == 44);
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
        assert(hp == 60);
    }
    { // ChainsComboInRecovery (combo 0 → 1 → 2 mata slime 60)
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
        assert(hp0 == 44); // combo0 head: 60 - 8 * 2.0
        assert(p.startSwing() && p.meleeCombo == 1);
        // Combo1 head (10 * 2.0 = 20): 44 -> 24, vivo. Espera o
        // Recovery (hit cai no Active, antes dele).
        for (int i = 0; i < 30 && p.meleePhase != MeleePhase::Recovery; ++i)
            ms.tick(1.f / 30.f, ctx);
        assert(p.meleePhase == MeleePhase::Recovery);
        int hp1 = -1;
        enemies.forEach([&](Enemy &s) { hp1 = s.resources.hp; });
        assert(hp1 == 24);
        // Combo2 head (16 * 2.0 = 32) fecha a conta.
        assert(p.startSwing() && p.meleeCombo == 2);
        for (int i = 0; i < 12; ++i) ms.tick(1.f / 30.f, ctx);
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
    { // ScalingStrDex (espada ferro: D FOR + B DES; base 8 intacta)
        Player p; // seed: espada, STR/DEX 10 → bônus 0
        assert(p.meleeDamage() == 8);
        int souls = 1000000000;
        for (int i = 0; i < 30; ++i)
            assert(p.attrs.buy(core::Attr::Strength, souls));
        assert(p.meleeDamage() == 8 + 2); // 12×0.2×1.0
        for (int i = 0; i < 30; ++i)
            assert(p.attrs.buy(core::Attr::Dexterity, souls));
        assert(p.meleeDamage() == 8 + 2 + 7); // +12×0.6×1.0
    }
    { // ReqPenalty (machado diamante pede 12 FOR: metade sem, cheio com)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand);
        assert(p.equipment.equip(core::Item{"diamond_axe", 1}));
        assert(p.meleeDamage() == 4); // (8 + 0) / 2
        int souls = 1000000000;
        assert(p.attrs.buy(core::Attr::Strength, souls));
        assert(p.attrs.buy(core::Attr::Strength, souls)); // FOR 12
        assert(p.meleeDamage() == 8 + 1); // +30×0.8×(2/30)
    }

    std::printf("melee test OK\n");
    return 0;
}
