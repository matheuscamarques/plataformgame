/**
 * @file tests/test_weapon_rotation.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava idle no input e swing no snapshot.
 * @details Cobre Player effectiveAim, roda com make test que compila em build/tests/test_weapon_rotation.
 */

#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"

// effectiveAim: mira efetiva da arma sem duplicar a regra no Renderer.
// Idle segue o input (aimDir); swing congela no snapshot (swingAim).
int main() {
    using support::AimDir;

    { // IdleUsesAimDirNotSwingAim (último golpe W não contamina idle)
        Player p;
        p.swingAim = AimDir::W; // último golpe foi W
        p.aimDir = AimDir::E; // olhando pra E agora
        p.meleePhase = MeleePhase::Idle;
        assert(!p.inMeleeSwing());
        assert(p.effectiveAim() == AimDir::E);
    }
    { // SwingUsesSwingAimNotAimDir (input vivo não move o golpe)
        Player p;
        p.swingAim = AimDir::N;
        p.aimDir = AimDir::E;
        p.meleePhase = MeleePhase::Active;
        assert(p.inMeleeSwing());
        assert(p.effectiveAim() == AimDir::N);
    }
    { // WindupStillSwing (snapshot vale desde o 1º tick do golpe)
        Player p;
        p.aimDir = AimDir::S;
        assert(p.startSwing());
        assert(p.swingAim == AimDir::S);
        assert(p.meleePhase == MeleePhase::Windup);
        p.aimDir = AimDir::W;
        assert(p.effectiveAim() == AimDir::S);
    }

    std::puts("weapon rotation test OK");
    return 0;
}
