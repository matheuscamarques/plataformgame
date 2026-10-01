/**
 * @file tests/test_hand_target.cpp
 * @brief Teste headless de B.3: alvos procedurais da mão por fase.
 * @details Via Player (startSwing/updateMelee/tick): recuo no Windup,
 * extensão no Active, assentamento no Recovery, repouso+senoide fora.
 * Roda com make test em build/tests/test_hand_target.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Combat/Limb.h"

static bool near(float a, float b, float eps) {
    return std::fabs(a - b) <= eps;
}

// Ombro esquerdo esperado (mesma âncora, box ArmL).
static core::Vec2f shoulderL(const Player& p) {
    const auto* t = p.body.find(support::BodyPartId::Torso);
    const auto* a = p.body.find(support::BodyPartId::ArmL);
    assert(t && a);
    const float cx = a->worldBox.left + a->worldBox.width * 0.5f;
    return support::limbShoulder({t->worldBox.left, t->worldBox.top},
                                 {t->worldBox.width, t->worldBox.height},
                                 cx);
}
// Ombro direito esperado a partir dos boxes atuais (mesma âncora).
static core::Vec2f shoulderR(const Player& p) {
    const auto* t = p.body.find(support::BodyPartId::Torso);
    const auto* a = p.body.find(support::BodyPartId::ArmR);
    assert(t && a);
    const float cx = a->worldBox.left + a->worldBox.width * 0.5f;
    return support::limbShoulder({t->worldBox.left, t->worldBox.top},
                                 {t->worldBox.width, t->worldBox.height},
                                 cx);
}

int main() {
    constexpr float kRow = 100.f / 40.f; // getH()=100: 2.5 mundo/row

    { // WindupRecua (mão 5 rows ATRÁS do eixo do golpe E)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing()); // aim E
        assert(p.handTargetsLive_);
        const core::Vec2f sh = shoulderR(p);
        assert(near(p.targetHandR_.x, sh.x - 5.f * kRow, 1e-3f) &&
               near(p.targetHandR_.y, sh.y, 1e-3f));
    }
    { // ActiveEstende (mão 8 rows À FRENTE no eixo E)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        const core::Vec2f sh = shoulderR(p);
        assert(near(p.targetHandR_.x, sh.x + 8.f * kRow, 1e-3f) &&
               near(p.targetHandR_.y, sh.y, 1e-3f));
    }
    { // RecoveryAssenta (mão 2 rows à frente: ainda no golpe)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        assert(p.updateMelee(0.10f) == MeleePhase::Recovery);
        const core::Vec2f sh = shoulderR(p);
        assert(near(p.targetHandR_.x, sh.x + 2.f * kRow, 1e-3f) &&
               near(p.targetHandR_.y, sh.y, 1e-3f));
    }
    { // FasesDistintas (recuo/extensão/assento não coincidem)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing());
        const core::Vec2f w = p.targetHandR_;
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        const core::Vec2f a = p.targetHandR_;
        assert(p.updateMelee(0.10f) == MeleePhase::Recovery);
        const core::Vec2f r = p.targetHandR_;
        assert(w.x < r.x && r.x < a.x); // -5 < +2 < +8 rows
    }
    { // DiagonalNE (ativo sobe à direita: x+, y-)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        p.aimDir = support::AimDir::NE;
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        const core::Vec2f sh = shoulderR(p);
        assert(p.targetHandR_.x > sh.x && p.targetHandR_.y < sh.y);
    }
    { // SwingGuardaDoTick (tick não sobrescreve alvo no swing)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing());
        const core::Vec2f w = p.targetHandR_;
        p.tick();
        assert(p.inMeleeSwing());
        assert(p.targetHandR_.x == w.x && p.targetHandR_.y == w.y);
    }
    { // RepousoParado (sem velocidade: à frente+abaixo do ombro)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        p.setVx(0.f);
        p.setVy(0.f);
        p.updateLimbs();
        assert(p.handTargetsLive_);
        const core::Vec2f sh = shoulderR(p);
        assert(near(p.targetHandR_.x, sh.x + 3.5f * kRow, 1e-3f) &&
               near(p.targetHandR_.y, sh.y + 2.f * kRow, 1e-3f));
    }
    { // MarchaOscila (frame A, contact dir: contrapposto, esq lidera)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        p.moveRight = true;
        p.jumping = true; // chão: walkFrame anda
        p.tick();
        assert(!p.inMeleeSwing() && p.handTargetsLive_);
        const core::Vec2f sh = shoulderR(p);
        const core::Vec2f sl = shoulderL(p);
        // fase +pi: R = 3.5+cos(pi) = +2.5, L = 2.0+cos(2pi) = +3.0:
        // perna dir à frente, braço ESQ à frente (oposição).
        assert(near(p.targetHandR_.x, sh.x + 2.5f * kRow, 1e-3f) &&
               near(p.targetHandR_.y, sh.y + 2.f * kRow, 1e-3f));
        assert(near(p.targetHandL_.x, sl.x + 3.f * kRow, 1e-3f) &&
               near(p.targetHandL_.y, sl.y + 2.f * kRow, 1e-3f));
    }
    { // JumpErgue (pose Jump: mãos ~10 rows ACIMA do ombro, sem senoide)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        p.setVx(0.f);
        p.setVy(-300.f); // subindo: senoide tentaria puxar p/ baixo
        p.currentFrameId = support::SpriteFrameId::PlayerJump;
        p.updateLimbs();
        assert(!p.inMeleeSwing() && p.handTargetsLive_);
        const core::Vec2f shR = shoulderR(p);
        const core::Vec2f shL = shoulderL(p);
        assert(near(p.targetHandR_.x, shR.x + 3.5f * kRow, 1e-3f) &&
               near(p.targetHandR_.y, shR.y - 10.f * kRow, 1e-3f));
        assert(near(p.targetHandL_.x, shL.x + 2.f * kRow, 1e-3f) &&
               near(p.targetHandL_.y, shL.y - 10.f * kRow, 1e-3f));
        // Arma segue a mão erguida (fim da arma flutuante).
        const core::Vec2f wh = p.weaponHand();
        assert(near(wh.x, p.poseR_.handWorld.x, 1e-3f) &&
               near(wh.y, p.poseR_.handWorld.y, 1e-3f));
        assert(wh.y < shR.y);
    }
    { // OffHandEspelha (live: poseL; Jump: esquerda sobe junto)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        p.setVx(0.f);
        p.setVy(0.f);
        p.updateLimbs();
        assert(p.handTargetsLive_);
        const core::Vec2f oh = p.offHand();
        assert(near(oh.x, p.poseL_.handWorld.x, 1e-3f) &&
               near(oh.y, p.poseL_.handWorld.y, 1e-3f));
        p.currentFrameId = support::SpriteFrameId::PlayerJump;
        p.updateLimbs();
        const core::Vec2f shL = shoulderL(p);
        assert(near(p.offHand().y, shL.y - 10.f * kRow, 1e-3f));
    }
    { // RespawnLimpa (volta a {0,0} + sem live até o próximo tick)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing());
        assert(p.handTargetsLive_);
        p.respawn(0.f, 0.f);
        assert(!p.handTargetsLive_);
        assert(p.targetHandR_.x == 0.f && p.targetHandR_.y == 0.f);
    }

    std::printf("hand target test OK\n");
    return 0;
}
