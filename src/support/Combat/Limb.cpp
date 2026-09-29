/**
 * @file src/support/Combat/Limb.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IK 2-bone (lei dos cossenos) + FK (B.1, sem SFML/GL).
 * @details Resolve no espaço local do corpo (facing=+1) e espelha os
 * pontos de volta quando facing < 0. A mão SEMPRE coincide com o alvo
 * alcançável, nos dois facings; o cotovelo espelha.
 */

#include "support/Combat/Limb.h"

#include <algorithm>
#include <cmath>

namespace support {

namespace {
inline constexpr float kPi = 3.14159265f;
inline constexpr float kReachEps = 1e-3f; // folga do clamp de alcance
inline constexpr float kRestEps = 1e-6f;  // raio do repouso

// Espelha pontos em X em torno do ombro (facing < 0).
core::Vec2f mirrorX(core::Vec2f p, core::Vec2f shoulder) {
    return {2.f * shoulder.x - p.x, p.y};
}
} // namespace

LimbPose solveIK(const Limb &limb, core::Vec2f shoulderWorld,
                 core::Vec2f targetHandWorld, bool bendForward, int facing) {
    LimbPose out;
    out.shoulderWorld = out.elbowWorld = out.handWorld = shoulderWorld;
    const float lu = limb.upper.length;
    const float ll = limb.lower.length;
    const float reach = lu + ll;
    if (!(reach > kRestEps)) return out; // sem comprimento: repouso

    // Problema no espaço local: facing<0 espelha o X de entrada.
    const float mir = (facing < 0) ? -1.f : 1.f;
    const float dx = (targetHandWorld.x - shoulderWorld.x) * mir;
    const float dy = targetHandWorld.y - shoulderWorld.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= kRestEps) return out; // alvo = ombro: tudo coincide

    const float clamped = std::min(dist, reach - kReachEps);
    const float base = std::atan2(dy, dx);
    // Lei dos cossenos: abertura do ombro e interior do cotovelo.
    const float cosShoulder =
        (lu * lu + clamped * clamped - ll * ll) / (2.f * lu * clamped);
    const float shoulderOffset =
        std::acos(std::max(-1.f, std::min(1.f, cosShoulder)));
    const float cosElbow =
        (lu * lu + ll * ll - clamped * clamped) / (2.f * lu * ll);
    const float elbowInterior =
        std::acos(std::max(-1.f, std::min(1.f, cosElbow)));
    const float s = bendForward ? 1.f : -1.f;
    const float aU = base - s * shoulderOffset;
    const float aL = s * (kPi - elbowInterior);

    core::Vec2f elbow{shoulderWorld.x + std::cos(aU) * lu,
                      shoulderWorld.y + std::sin(aU) * lu};
    core::Vec2f hand{elbow.x + std::cos(aU + aL) * ll,
                     elbow.y + std::sin(aU + aL) * ll};
    if (mir < 0.f) { // volta para o mundo: espelha os pontos
        elbow = mirrorX(elbow, shoulderWorld);
        hand = mirrorX(hand, shoulderWorld);
    }
    out.elbowWorld = elbow;
    out.handWorld = hand;
    out.angleUpper = aU;
    out.angleLower = aL;
    return out;
}

LimbPose forwardKinematics(const Limb &limb, core::Vec2f shoulderWorld,
                           int facing) {
    LimbPose out;
    out.shoulderWorld = shoulderWorld;
    out.angleUpper = limb.angleUpper;
    out.angleLower = limb.angleLower;
    core::Vec2f elbow{shoulderWorld.x + std::cos(limb.angleUpper) *
                                            limb.upper.length,
                      shoulderWorld.y + std::sin(limb.angleUpper) *
                                            limb.upper.length};
    core::Vec2f hand{elbow.x + std::cos(limb.angleUpper + limb.angleLower) *
                                   limb.lower.length,
                     elbow.y + std::sin(limb.angleUpper + limb.angleLower) *
                                   limb.lower.length};
    if (facing < 0) {
        elbow = mirrorX(elbow, shoulderWorld);
        hand = mirrorX(hand, shoulderWorld);
    }
    out.elbowWorld = elbow;
    out.handWorld = hand;
    return out;
}

} // namespace support
