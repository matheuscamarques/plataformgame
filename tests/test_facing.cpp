/**
 * @file tests/test_facing.cpp
 * @brief Teste headless da Fase A: facingFromInput, espelho e baseDir.
 * @details Puro, sem GL. Roda com make test em build/tests/test_facing.
 */
#include <cassert>
#include <cmath>
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
    { // DirectedPose (plataforma 2D: Idle/Walk/Jump sempre side-view;
        // combate/morte segue 8-way via baseDir)
        using game::artDirFor;
        using game::directedPose;
        using sprites::PlayerPose;
        // Locomoção nunca olha p/ o jogador: S/SE/E/NE/N → E,
        // NW/W/SW → W (flip cuida do espelho).
        assert(artDirFor(PlayerPose::Idle, Facing::S) == Facing::E);
        assert(artDirFor(PlayerPose::Idle, Facing::SE) == Facing::E);
        assert(artDirFor(PlayerPose::Idle, Facing::E) == Facing::E);
        assert(artDirFor(PlayerPose::Idle, Facing::NE) == Facing::E);
        assert(artDirFor(PlayerPose::Idle, Facing::N) == Facing::E);
        assert(artDirFor(PlayerPose::Idle, Facing::NW) == Facing::W);
        assert(artDirFor(PlayerPose::Idle, Facing::W) == Facing::W);
        assert(artDirFor(PlayerPose::Idle, Facing::SW) == Facing::W);
        const PlayerPose locom[] = {PlayerPose::WalkA, PlayerPose::WalkB,
                                    PlayerPose::WalkC, PlayerPose::WalkD,
                                    PlayerPose::Jump};
        for (PlayerPose w : locom) {
            assert(artDirFor(w, Facing::S) == Facing::E);
            assert(artDirFor(w, Facing::N) == Facing::E);
            assert(artDirFor(w, Facing::SE) == Facing::E);
            assert(artDirFor(w, Facing::NE) == Facing::E);
            assert(artDirFor(w, Facing::W) == Facing::W);
            assert(artDirFor(w, Facing::SW) == Facing::W);
            assert(artDirFor(w, Facing::NW) == Facing::W);
        }
        // Onda 2a: Punch entra no conjunto (PunchUp/Jump seguem em E).
        assert(artDirFor(PlayerPose::Punch, Facing::N) == Facing::N);
        assert(artDirFor(PlayerPose::Punch, Facing::SW) == Facing::SE);
        assert(artDirFor(PlayerPose::PunchUp, Facing::N) == Facing::N);
        assert(artDirFor(PlayerPose::PunchUp, Facing::SW) == Facing::SE);
        assert(artDirFor(PlayerPose::PunchDown, Facing::S) == Facing::S);
        assert(artDirFor(PlayerPose::PunchDown, Facing::NE) == Facing::NE);
        // Onda 3a: Hurt entra (Jump é marcha aérea: side-view acima).
        assert(artDirFor(PlayerPose::Hurt, Facing::S) == Facing::S);
        assert(artDirFor(PlayerPose::Hurt, Facing::NW) == Facing::NE);
        // Ataque aéreo: Punch/Throw viram side-view (perna tucked);
        // no chão seguem 8-way. Hurt/Death não mudam no ar.
        const PlayerPose airAtk[] = {PlayerPose::Punch,
                                     PlayerPose::PunchUp,
                                     PlayerPose::PunchDown,
                                     PlayerPose::Throw};
        for (PlayerPose a : airAtk) {
            for (Facing f : {Facing::S, Facing::SE, Facing::NE,
                             Facing::N})
                assert(artDirFor(a, f, true) == Facing::E);
            for (Facing f : {Facing::W, Facing::SW, Facing::NW})
                assert(artDirFor(a, f, true) == Facing::W);
            assert(artDirFor(a, Facing::N, false) ==
                   artDirFor(a, Facing::N)); // chão: 8-way intacto
            const auto da = directedPose(a, Facing::SW, true);
            assert(da.artDir == Facing::W && da.mirror);
        }
        assert(artDirFor(PlayerPose::Hurt, Facing::S, true) == Facing::S);
        assert(artDirFor(PlayerPose::Death, Facing::NW, true) ==
               Facing::NE);
        // directedPose carrega pose + artDir + espelho juntos.
        const auto d = directedPose(PlayerPose::Idle, Facing::NW);
        assert(d.pose == PlayerPose::Idle);
        assert(d.artDir == Facing::W && d.mirror); // locomoção: side-view
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
    { // ChaoIgnoraVy (parado no chão: gravidade residual não vira S)
        Player p;
        p.respawn(0.f, 0.f);
        assert(p.facing8 == Facing::E);
        p.jumping = true; // chão (no jogo, collide poria)
        p.tick(); // step aplica gravidade: vy≈2, vx=0
        assert(p.jumping && std::fabs(p.getVy()) > 1.f); // cenário armado
        assert(p.facing8 == Facing::E); // mantém E, nunca S
        p.moveRight = true;
        p.tick();
        assert(p.facing8 == Facing::E); // andando: side-view
    }
    { // ArSegueEixoDominante (no ar vy real manda: caindo vira S)
        Player p;
        p.respawn(0.f, 0.f);
        p.jumping = false; // ar
        p.setVy(50.f);
        p.tick();
        assert(!p.jumping && std::fabs(p.getVy()) > 1.f);
        assert(p.facing8 == Facing::S);
    }

    std::printf("facing test OK\n");
    return 0;
}
