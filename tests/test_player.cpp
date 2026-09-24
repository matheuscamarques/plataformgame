/**
 * @file tests/test_player.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava i-frames expirando via tick e cooldown.
 * @details Cobre Player hurt e throw, roda com make test que compila em build/tests/test_player.
 */

#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"

// Player: i-frames expiram via tick (sem isso o sprite trava em Hurt).
int main() {
    { // HurtIframesDecayAfterTick
        Player p;
        assert(p.hurt(10));
        assert(p.hurtIframes.running());   // arrancou
        for (int i = 0; i < 20; ++i) p.tick(); // ~0.66s > 0.6s
        assert(!p.hurtIframes.running());  // expirou
        assert(p.hp == 9990);
    }
    { // ThrowCooldownTicksInPlayerTick (1 só lugar, sem Game)
        Player p;
        p.throwCooldown.trigger();
        assert(p.throwCooldown.running());
        for (int i = 0; i < 20; ++i) p.tick();
        assert(p.throwCooldown.ready());
    }
    { // DerivedFromAttributes (seed VIT/END 10: 10000/150/60)
        Player p;
        assert(p.hpMax == 10000);
        assert(p.staminaMax == 150.f && p.stamina == 150.f);
        assert(p.maxEquipLoad() == 60.f);
        assert(!p.heavilyLoaded()); // set ferro 28 <= 30
        int souls = 1000000;
        assert(p.attrs.buy(core::Attr::Vitality, souls));
        assert(p.attrs.buy(core::Attr::Endurance, souls));
        p.refreshDerived();
        assert(p.hpMax == 10200 && p.staminaMax == 155.f);
        assert(p.maxEquipLoad() == 62.f);
    }
    { // StaminaRegen (1/tick até o teto)
        Player p;
        p.stamina = 100.f;
        p.tick();
        assert(p.stamina == 101.f);
        p.stamina = 149.5f;
        p.tick();
        assert(p.stamina == 150.f); // trava no teto, sem passar
    }
    { // SwingDrainsBlocks (20 por golpe; sem fôlego não sai)
        Player p;
        assert(p.startSwing());
        assert(p.stamina == 130.f);
        p.meleePhase = MeleePhase::Idle;
        p.stamina = 10.f;
        assert(!p.startSwing());
        assert(p.meleePhase == MeleePhase::Idle);
        assert(p.stamina == 10.f); // sem gasto no bloqueio
    }
    { // SprintDrainsCutsRun (10/s; zerou corta a corrida)
        Player p;
        p.runFast = true;
        p.moveRight = true;
        p.stamina = 5.f;
        p.tick();
        assert(p.stamina < 5.f && p.runFast); // ainda corre
        p.stamina = 0.2f;
        p.tick();
        assert(p.stamina == 0.f && !p.runFast); // cortou
    }
    { // RegenDelay (0.8s sem regen após gastar)
        Player p;
        assert(p.startSwing()); // drena + arma o delay
        p.stamina = 100.f;
        for (int i = 0; i < 10; ++i) p.tick();
        assert(p.stamina == 100.f); // delay segurando
        for (int i = 0; i < 30; ++i) p.tick();
        assert(p.stamina > 100.f); // voltou a regenar
    }

    std::printf("player test OK\n");
    return 0;
}
