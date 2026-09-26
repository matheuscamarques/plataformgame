/**
 * @file tests/test_roll.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless da rolagem DS (i-frames, custo, fat roll).
 * @details Cobre startRoll (custo/direção), janela de i-frames, fat roll sem i-frames, gates e distância, roda com make test que compila em build/tests/test_roll.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "entities/Player/Player.h"

int main() {
    { // StartsAndCosts (25 stamina, direção do input, vira o facing)
        Player p;
        p.jumping = true; // chão
        p.moveLeft = true;
        const float st = p.stamina;
        assert(p.startRoll());
        assert(p.rolling() && p.rollDir == -1 && p.facing == -1);
        assert(p.stamina == st - 25.f);
        assert(!p.startRoll()); // rolando: sem retrigger
    }
    { // IFramesWindow (0.35s protege, depois pega)
        Player p;
        p.jumping = true;
        assert(p.startRoll());
        assert(!p.hurt(50)); // i-frame do roll
        for (int i = 0; i < 11; ++i) p.tick(); // 0.367s > 0.35
        p.hurtIframes.tick(1.f); // garante hurt pronto (não é o roll)
        for (int i = 0; i < 2; ++i) p.tick(); // roll expirou (0.4s)
        assert(!p.rolling());
        assert(p.hurt(50) && p.hp < 100); // sem i-frame: pega
    }
    { // FatRollNoIframes (carga pesada rola mas apanha)
        Player p;
        p.equipment = core::Equipment{};
        assert(p.equipment.equip(core::Item{"gold_sword", 1}));
        assert(p.equipment.equip(core::Item{"gold_helm", 1}));
        assert(p.equipment.equip(core::Item{"gold_chest", 1}));
        assert(p.equipment.equip(core::Item{"gold_legs", 1}));
        assert(p.equipment.equip(core::Item{"gold_boots", 1}));
        assert(p.heavilyLoaded()); // 37 > 30
        p.jumping = true;
        assert(p.startRoll());
        assert(p.rolling());
        assert(p.hurt(50)); // sem i-frames: pega no meio do roll
    }
    { // Gates (ar, golpe, morte, sem fôlego)
        Player p;
        p.jumping = false; // no ar
        assert(!p.startRoll());
        p.jumping = true;
        assert(p.startSwing());
        while (p.meleePhase == MeleePhase::Windup)
            p.updateMelee(1.f / 30.f);
        assert(p.meleePhase == MeleePhase::Active);
        assert(!p.startRoll());
        Player d;
        d.hp = 0;
        assert(!d.startRoll());
        Player t;
        t.stamina = 0.f;
        assert(!t.startRoll());
    }
    { // Distance (rajada ~16px/tick por 0.4s)
        Player p;
        p.jumping = true;
        p.moveRight = true;
        const float x0 = p.getX();
        assert(p.startRoll());
        for (int i = 0; i < 12; ++i) p.tick();
        const float dx = p.getX() - x0;
        assert(dx > 150.f && dx < 230.f); // 16×12=192 ± atrito de borda
    }

    std::printf("roll test OK\n");
    return 0;
}
