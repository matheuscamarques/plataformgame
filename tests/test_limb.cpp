/**
 * @file tests/test_limb.cpp
 * @brief Teste headless de B.1: Limb + solveIK puros, zero SFML.
 * @details Roda com make test em build/tests/test_limb.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/Combat/Limb.h"

static bool near(float a, float b, float eps) {
    return std::fabs(a - b) <= eps;
}

static support::Limb makeArm() {
    support::Limb l;
    l.upper.length = 10.f;
    l.lower.length = 10.f;
    return l;
}

int main() {
    using support::applyPose;
    using support::forwardKinematics;
    using support::LimbPose;
    using support::solveIK;

    { // ReachClampaSemNaN (alvo a 30, alcance 20: estica na direção)
        const LimbPose p =
            solveIK(makeArm(), {0.f, 0.f}, {30.f, 0.f}, true, 1);
        assert(std::isfinite(p.elbowWorld.x) &&
               std::isfinite(p.handWorld.x));
        const float hx = p.handWorld.x - 0.f;
        assert(near(hx, 20.f, 0.05f) && near(p.handWorld.y, 0.f, 1e-3f));
        // Diagonal também preserva a direção.
        const LimbPose d =
            solveIK(makeArm(), {0.f, 0.f}, {30.f, 30.f}, true, 1);
        const float dl =
            std::sqrt(d.handWorld.x * d.handWorld.x +
                      d.handWorld.y * d.handWorld.y);
        assert(near(dl, 20.f, 0.05f));
        assert(d.handWorld.x > 0.f && d.handWorld.y > 0.f);
    }
    { // RestCoincide (alvo = ombro: sem divisão por zero)
        const LimbPose p =
            solveIK(makeArm(), {5.f, 5.f}, {5.f, 5.f}, true, 1);
        assert(p.elbowWorld.x == 5.f && p.elbowWorld.y == 5.f);
        assert(p.handWorld.x == 5.f && p.handWorld.y == 5.f);
    }
    { // BendSideOpostos (mesmo alvo: cotovelo troca de lado, mão fica)
        const LimbPose fwd =
            solveIK(makeArm(), {0.f, 0.f}, {10.f, 0.f}, true, 1);
        const LimbPose bwd =
            solveIK(makeArm(), {0.f, 0.f}, {10.f, 0.f}, false, 1);
        assert(fwd.elbowWorld.y < 0.f && bwd.elbowWorld.y > 0.f);
        assert(near(fwd.elbowWorld.x, bwd.elbowWorld.x, 1e-3f));
        assert(near(fwd.handWorld.x, 10.f, 1e-3f) &&
               near(fwd.handWorld.y, 0.f, 1e-3f));
        assert(near(bwd.handWorld.x, 10.f, 1e-3f) &&
               near(bwd.handWorld.y, 0.f, 1e-3f));
    }
    { // FacingMirror (alvo espelhado + facing -1: cotovelo espelha em X)
        const LimbPose r =
            solveIK(makeArm(), {0.f, 0.f}, {10.f, 0.f}, true, 1);
        const LimbPose l =
            solveIK(makeArm(), {0.f, 0.f}, {-10.f, 0.f}, true, -1);
        assert(near(l.elbowWorld.x, -r.elbowWorld.x, 1e-3f));
        assert(near(l.elbowWorld.y, r.elbowWorld.y, 1e-3f));
        assert(near(l.handWorld.x, -10.f, 1e-3f));
        // Mesmo alvo nos dois facings: a mão fica, o lado dobra junto.
        const LimbPose same =
            solveIK(makeArm(), {0.f, 0.f}, {10.f, 0.f}, true, -1);
        assert(near(same.handWorld.x, 10.f, 1e-3f) &&
               near(same.handWorld.y, 0.f, 1e-3f));
    }
    { // FKReproduzIK (ângulos da pose → mesmos cotovelo/mão)
        support::Limb arm = makeArm();
        const LimbPose p =
            solveIK(arm, {0.f, 0.f}, {10.f, 0.f}, true, 1);
        applyPose(arm, p);
        const LimbPose fk = forwardKinematics(arm, {0.f, 0.f}, 1);
        assert(near(fk.elbowWorld.x, p.elbowWorld.x, 1e-4f) &&
               near(fk.elbowWorld.y, p.elbowWorld.y, 1e-4f));
        assert(near(fk.handWorld.x, p.handWorld.x, 1e-4f) &&
               near(fk.handWorld.y, p.handWorld.y, 1e-4f));
        // FK também espelha pontos com facing -1.
        const LimbPose fkm = forwardKinematics(arm, {0.f, 0.f}, -1);
        assert(near(fkm.elbowWorld.x, -fk.elbowWorld.x, 1e-4f) &&
               near(fkm.elbowWorld.y, fk.elbowWorld.y, 1e-4f));
    }

    std::printf("limb test OK\n");
    return 0;
}
