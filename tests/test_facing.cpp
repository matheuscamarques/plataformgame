/**
 * @file tests/test_facing.cpp
 * @brief Teste headless da Fase A: facingFromInput, espelho e baseDir.
 * @details Puro, sem GL. Roda com make test em build/tests/test_facing.
 */
#include <cassert>
#include <cstdio>
#include "assets/PlayerSprite.h"
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
    { // FlipInvariant (produção: facing espelha facing8 via setFacing8)
        Player p;
        const Facing all[] = {Facing::E,  Facing::NE, Facing::N,
                              Facing::NW, Facing::W,  Facing::SW,
                              Facing::S,  Facing::SE};
        for (Facing f : all) {
            p.setFacing8(f);
            assert(p.facing8 == f);
            assert(p.facing == facingSign(f));
        }
    }
    { // DirectedPose (Fase D: Idle/Walk têm 5 dirs; resto cai em E)
        using game::artDirFor;
        using game::directedPose;
        using sprites::PlayerPose;
        // Onda 1: base dobra NW→NE, W→E, SW→SE.
        assert(artDirFor(PlayerPose::Idle, Facing::S) == Facing::S);
        assert(artDirFor(PlayerPose::Idle, Facing::SE) == Facing::SE);
        assert(artDirFor(PlayerPose::Idle, Facing::E) == Facing::E);
        assert(artDirFor(PlayerPose::Idle, Facing::NE) == Facing::NE);
        assert(artDirFor(PlayerPose::Idle, Facing::N) == Facing::N);
        assert(artDirFor(PlayerPose::Idle, Facing::NW) == Facing::NE);
        assert(artDirFor(PlayerPose::Idle, Facing::W) == Facing::E);
        assert(artDirFor(PlayerPose::Idle, Facing::SW) == Facing::SE);
        assert(artDirFor(PlayerPose::WalkA, Facing::N) == Facing::N);
        assert(artDirFor(PlayerPose::WalkB, Facing::SW) == Facing::SE);
        assert(artDirFor(PlayerPose::WalkC, Facing::NE) == Facing::NE);
        assert(artDirFor(PlayerPose::WalkD, Facing::S) == Facing::S);
        // Onda 2a: Punch entra no conjunto (PunchUp/Jump seguem em E).
        assert(artDirFor(PlayerPose::Punch, Facing::N) == Facing::N);
        assert(artDirFor(PlayerPose::Punch, Facing::SW) == Facing::SE);
        assert(artDirFor(PlayerPose::PunchUp, Facing::N) == Facing::N);
        assert(artDirFor(PlayerPose::PunchUp, Facing::SW) == Facing::SE);
        assert(artDirFor(PlayerPose::PunchDown, Facing::S) == Facing::S);
        assert(artDirFor(PlayerPose::PunchDown, Facing::NE) == Facing::NE);
        // Onda 3a: Hurt/Jump entram (Death/Throw seguem em E).
        assert(artDirFor(PlayerPose::Hurt, Facing::S) == Facing::S);
        assert(artDirFor(PlayerPose::Hurt, Facing::NW) == Facing::NE);
        assert(artDirFor(PlayerPose::Jump, Facing::S) == Facing::S);
        assert(artDirFor(PlayerPose::Jump, Facing::NW) == Facing::NE);
        // directedPose carrega pose + artDir + espelho juntos.
        const auto d = directedPose(PlayerPose::Idle, Facing::NW);
        assert(d.pose == PlayerPose::Idle);
        assert(d.artDir == Facing::NE && d.mirror);
        const auto e = directedPose(PlayerPose::Idle, Facing::E);
        assert(e.artDir == Facing::E && !e.mirror);
        const auto h = directedPose(PlayerPose::Hurt, Facing::SW);
        assert(h.pose == PlayerPose::Hurt);
        assert(h.artDir == Facing::SE && h.mirror); // onda 3a: tem SE
        const auto t = directedPose(PlayerPose::Throw, Facing::W);
        assert(t.artDir == Facing::E && t.mirror); // Throw segue em E
    }
    { // PosePartsForCaiEmE (sem arte direcional: mesmo ponteiro)
        for (int pi = 0; pi < sprites::kPlayerPoseCount; ++pi) {
            const auto pose = static_cast<sprites::PlayerPose>(pi);
            assert(sprites::posePartsFor(pose, Facing::E) ==
                   sprites::poseParts(pose));
        }
        // Índices 0..4 cobrem S/SE/E/NE/N; fora cai em E.
        assert(sprites::artDirIndex(Facing::S) == 0);
        assert(sprites::artDirIndex(Facing::SE) == 1);
        assert(sprites::artDirIndex(Facing::E) == 2);
        assert(sprites::artDirIndex(Facing::NE) == 3);
        assert(sprites::artDirIndex(Facing::N) == 4);
        assert(sprites::artDirForIndex(0) == Facing::S);
        assert(sprites::artDirForIndex(4) == Facing::N);
        assert(sprites::artDirForIndex(99) == Facing::E);
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
