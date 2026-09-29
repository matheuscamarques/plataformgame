/**
 * @file src/support/Combat/IK.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IK 2-bone (lei dos cossenos) para braços/pernas articulados.
 * @details Fase B do plano de profundidade: dado onde a mão deve estar,
 * calcula os ângulos do ombro e do cotovelo. Puro, sem SFML/GL.
 *
 * Convenção de ângulos (graus, screen-space Y para baixo — igual à
 * rotação horária do sf::Sprite, então o Renderer copia direto):
 * - 0 = apontando para +X (leste), 90 = para +Y (baixo/sul).
 * - outAngleUpper: ângulo absoluto do segmento superior.
 * - outAngleLower: ângulo relativo do inferior (0 = esticado,
 *   sinal dado por bendForward).
 */

#pragma once

#include <algorithm>
#include <cmath>

#include "core/Vec.h"

namespace support {

inline void solveIK(core::Vec2f shoulder, core::Vec2f target,
                    float lenUpper, float lenLower,
                    bool bendForward, float &outAngleUpper,
                    float &outAngleLower) {
    constexpr float kPi = 3.14159265f;
    if (!(lenUpper > 0.f) || !(lenLower > 0.f)) {
        outAngleUpper = 0.f;
        outAngleLower = 0.f;
        return;
    }
    const float dx = target.x - shoulder.x;
    const float dy = target.y - shoulder.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    // Alvo inalcançável = estica o braço na direção do alvo.
    const float clamped =
        std::min(dist, lenUpper + lenLower - 0.01f);
    if (clamped <= 1e-6f) {
        outAngleUpper = 90.f; // ombro para baixo (repouso)
        outAngleLower = 0.f;
        return;
    }
    const float baseAngle = std::atan2(dy, dx); // rad, 0=E horário+
    // Lei dos cossenos: ângulo interno do cotovelo.
    const float cosElbow =
        (lenUpper * lenUpper + lenLower * lenLower - clamped * clamped) /
        (2.f * lenUpper * lenLower);
    const float elbowInterior =
        std::acos(std::max(-1.f, std::min(1.f, cosElbow)));
    // Offset do ombro em relação à linha ombro→alvo.
    const float cosShoulder =
        (lenUpper * lenUpper + clamped * clamped - lenLower * lenLower) /
        (2.f * lenUpper * clamped);
    const float shoulderOffset =
        std::acos(std::max(-1.f, std::min(1.f, cosShoulder)));
    const float sign = bendForward ? 1.f : -1.f;
    outAngleUpper =
        (baseAngle - sign * shoulderOffset) * 180.f / kPi;
    outAngleLower = sign * (180.f - elbowInterior * 180.f / kPi);
}

} // namespace support
