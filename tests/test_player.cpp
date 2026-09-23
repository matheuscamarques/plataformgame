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

    std::printf("player test OK\n");
    return 0;
}
