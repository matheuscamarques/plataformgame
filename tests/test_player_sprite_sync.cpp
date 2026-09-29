/**
 * @file tests/test_player_sprite_sync.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava sprite seguindo meleePhase e swingAim.
 * @details Cobre resolvePlayerSprite, roda com make test que compila em build/tests/test_player_sprite_sync.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "assets/PlayerSprite.h"
#include "entities/Player/Player.h"

// Sincronia sprite ↔ hitbox: fonte única meleePhase (via inMeleeSwing)
// + snapshot swingAim. Sem timer duplicado, sem input vivo.
namespace {

support::SpriteFrameId resolveFor(const Player &p) {
    return game::resolvePlayerSprite(true, 0.f, false, p.inMeleeSwing(),
                                     p.swingAim, false, 0);
}

} // namespace

int main() {
    using support::AimDir;
    using support::SpriteFrameId;

    { // SpriteMatchesPhaseThroughoutSwing (Idle→W→A→R, sempre soco em E)
        Player p;
        assert(!p.inMeleeSwing());
        assert(resolveFor(p) == SpriteFrameId::PlayerIdle);
        assert(p.startSwing());
        assert(p.inMeleeSwing());
        assert(resolveFor(p) == SpriteFrameId::PlayerPunch);
        p.updateMelee(0.10f); // Windup → Active
        assert(p.meleePhase == MeleePhase::Active);
        assert(resolveFor(p) == SpriteFrameId::PlayerPunch);
        p.updateMelee(0.10f); // Active → Recovery
        assert(p.meleePhase == MeleePhase::Recovery);
        assert(resolveFor(p) == SpriteFrameId::PlayerPunch);
    }
    { // Combo3SpriteHoldsThroughRecovery (hit3 Recovery 0.22s segura)
        Player p;
        assert(p.startSwing()); // hit1 W0
        p.updateMelee(0.10f); // → A0
        p.updateMelee(0.10f); // → R0
        assert(p.startSwing()); // hit2 W1
        p.updateMelee(0.10f); // → A1
        p.updateMelee(0.10f); // → R1 (0.12 > 0.10, fica)
        assert(p.meleePhase == MeleePhase::Recovery);
        assert(p.startSwing() && p.meleeCombo == 2); // hit3 W2
        p.updateMelee(0.10f); // → A2
        p.updateMelee(0.10f); // → R2
        assert(p.meleePhase == MeleePhase::Recovery);
        assert(p.inMeleeSwing());
        assert(resolveFor(p) == SpriteFrameId::PlayerPunch);
    }
    { // SpriteUsesSwingAimNotLiveAim (N congelado, E vivo → Up)
        Player p;
        p.aimDir = AimDir::N;
        assert(p.startSwing());
        assert(p.swingAim == AimDir::N);
        p.aimDir = AimDir::E; // jogador soltou o ↑
        assert(resolveFor(p) == SpriteFrameId::PlayerPunchUp);
    }
    { // HitboxAndSpriteReadSameAim (money test: N divergente de E)
        Player p; // espada: arco direcional por swingAim
        p.equipment.equip(core::Item{"iron_sword", 1});
        p.body.rebuild({0.f, 0.f}, 1);
        p.aimDir = AimDir::N;
        assert(p.startSwing());
        p.aimDir = AimDir::E; // divergente de propósito
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        assert(p.inMeleeSwing());
        const support::SweepArc arc = p.sweepArc();
        assert(!arc.empty); // N → acima (snapshot, não o E vivo)
        assert(std::fabs(arc.centerAngle + 3.14159265f * 0.5f) < 0.01f);
        assert(resolveFor(p) == SpriteFrameId::PlayerPunchUp);
    }

    { // PoseForFrameId (Fase D: 10 frames mapeiam p/ pose certa)
        using support::SpriteFrameId;
        using sprites::PlayerPose;
        assert(game::poseForFrameId(SpriteFrameId::PlayerIdle) ==
               PlayerPose::Idle);
        assert(game::poseForFrameId(SpriteFrameId::PlayerWalkA) ==
               PlayerPose::WalkA);
        assert(game::poseForFrameId(SpriteFrameId::PlayerWalkB) ==
               PlayerPose::WalkB);
        assert(game::poseForFrameId(SpriteFrameId::PlayerWalkC) ==
               PlayerPose::WalkC);
        assert(game::poseForFrameId(SpriteFrameId::PlayerWalkD) ==
               PlayerPose::WalkD);
        assert(game::poseForFrameId(SpriteFrameId::PlayerJump) ==
               PlayerPose::Jump);
        assert(game::poseForFrameId(SpriteFrameId::PlayerThrow) ==
               PlayerPose::Throw);
        assert(game::poseForFrameId(SpriteFrameId::PlayerPunch) ==
               PlayerPose::Punch);
        assert(game::poseForFrameId(SpriteFrameId::PlayerPunchUp) ==
               PlayerPose::PunchUp);
        assert(game::poseForFrameId(SpriteFrameId::PlayerPunchDown) ==
               PlayerPose::PunchDown);
        assert(game::poseForFrameId(SpriteFrameId::PlayerHurt) ==
               PlayerPose::Hurt);
        assert(game::poseForFrameId(SpriteFrameId::PlayerDeath) ==
               PlayerPose::Death);
        assert(game::poseForFrameId(SpriteFrameId::SlimeIdle) ==
               PlayerPose::Idle); // fallback: nunca usado p/ player
        assert(game::poseForFrameId(SpriteFrameId::COUNT) ==
               PlayerPose::Idle);
    }
    { // AimSurvivesJumpConsumption (↑ segurado no ar mantém N)
        Player p;
        p.moveUp = true;
        p.jumping = true;
        p.jumpingRecharge = 999.f; // estoura o limiar: pulo consome moveUp
        p.tick();
        assert(!p.moveUp); // subida limitada preservada
        assert(p.aimDir == support::AimDir::N); // mira viu o input físico
    }

    std::puts("player sprite sync test OK");
    return 0;
}
