/**
 * @file tests/test_facing.cpp
 * @brief Teste headless da Fase A: facingFromInput, espelho e baseDir.
 * @details Puro, sem GL. Roda com make test em build/tests/test_facing.
 */
#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Combat/Facing.h"

int main() {
    using support::Facing;
    using support::baseDir;
    using support::facingFromInput;
    using support::facingSign;
    using support::isMirrored;

    { // ParadoMantem (vx=vy=0 não muda)
        assert(facingFromInput(0.f, 0.f, Facing::N) == Facing::N);
        assert(facingFromInput(0.f, 0.f, Facing::SW) == Facing::SW);
    }
    { // Cardeais (vy+ = baixo, screen-space)
        assert(facingFromInput(1.f, 0.f, Facing::N) == Facing::E);
        assert(facingFromInput(-1.f, 0.f, Facing::N) == Facing::W);
        assert(facingFromInput(0.f, -1.f, Facing::E) == Facing::N);
        assert(facingFromInput(0.f, 1.f, Facing::E) == Facing::S);
    }
    { // Diagonais
        assert(facingFromInput(1.f, -1.f, Facing::E) == Facing::NE);
        assert(facingFromInput(-1.f, -1.f, Facing::E) == Facing::NW);
        assert(facingFromInput(1.f, 1.f, Facing::E) == Facing::SE);
        assert(facingFromInput(-1.f, 1.f, Facing::E) == Facing::SW);
    }
    { // EspelhoSoEmNW_W_SW
        assert(isMirrored(Facing::NW) && isMirrored(Facing::W) &&
               isMirrored(Facing::SW));
        assert(!isMirrored(Facing::E) && !isMirrored(Facing::NE) &&
               !isMirrored(Facing::N) && !isMirrored(Facing::S) &&
               !isMirrored(Facing::SE));
    }
    { // BaseDirDesfazEspelho
        assert(baseDir(Facing::NW) == Facing::NE);
        assert(baseDir(Facing::W) == Facing::E);
        assert(baseDir(Facing::SW) == Facing::SE);
        assert(baseDir(Facing::E) == Facing::E);
        assert(baseDir(Facing::N) == Facing::N);
        assert(baseDir(Facing::S) == Facing::S);
    }
    { // SignDerivaPM1 (compat com Entity::facing)
        assert(facingSign(Facing::W) == -1);
        assert(facingSign(Facing::NW) == -1);
        assert(facingSign(Facing::SW) == -1);
        assert(facingSign(Facing::E) == 1);
        assert(facingSign(Facing::N) == 1);
        assert(facingSign(Facing::S) == 1);
    }
    { // DeadzoneMantem (ruído ~0 não flipa; eixo dominante manda)
        using support::facingFromVelocity;
        assert(facingFromVelocity(0.5f, 0.5f, Facing::W) == Facing::W);
        assert(facingFromVelocity(0.f, 0.f, Facing::S) == Facing::S);
        assert(facingFromVelocity(-9.8f, 2.f, Facing::E) == Facing::W);
        assert(facingFromVelocity(9.8f, 0.f, Facing::W) == Facing::E);
    }
    { // HibridoPlayer (movimento → W/E; swing congela no snapshot)
        Player p;
        p.respawn(0.f, 0.f);
        assert(p.facing8 == Facing::E && p.facing == 1);
        p.setFacing8(Facing::SW);
        assert(p.facing8 == Facing::SW && p.facing == -1);
        // Swing: corpo vira para o snapshot, mesmo parado.
        p.swingAim = support::AimDir::N;
        p.meleePhase = MeleePhase::Windup;
        p.meleeTimer = 10.f;
        p.tick();
        assert(p.facing8 == Facing::N);
        assert(p.facing == 1); // N não espelha: derivado volta a +1
    }

    std::printf("facing test OK\n");
    return 0;
}
