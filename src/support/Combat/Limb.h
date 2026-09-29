/**
 * @file src/support/Combat/Limb.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Membro articulado em 2 segmentos (ombro→cotovelo→mão).
 * @details Fase B do plano de profundidade: substitui o bloco único de
 * braço de PlayerParts por hierarquia com juntas. Matemática pura
 * (sem SFML/GL): elbowPos/handPos derivam dos ângulos na mesma
 * convenção de IK.h (graus, 0=E, horário+). O Renderer mapeia para
 * sf::Sprite com setOrigin(pivot)+setRotation() — sem rasterizar
 * rotação em compose().
 */

#pragma once

#include <cmath>

#include "core/Vec.h"

namespace support {

struct LimbSegment {
    const char *const *rows = nullptr; // arte do segmento (pode ser nulo no spike)
    int w = 0, h = 0;
    core::Vec2f pivot{0.f, 0.f}; // ponto de rotação em px (ombro/cotovelo)
    float restAngle = 90.f;      // repouso: para baixo
};

// Comprimento útil do segmento: do pivot até a ponta (em px de sprite).
inline float segmentLength(const LimbSegment &s) {
    const float len = static_cast<float>(s.h) - s.pivot.y;
    return len > 0.f ? len : 0.f;
}

struct Limb {
    LimbSegment upper; // ombro → cotovelo
    LimbSegment lower; // cotovelo → mão
    float angleUpper = 90.f; // graus, convenção IK.h
    float angleLower = 0.f;  // relativo ao upper
    core::Vec2f rootOffset{0.f, 0.f}; // ombro relativo ao centro do corpo

    // Cotovelo em coords de mundo dado o ombro.
    core::Vec2f elbowPos(core::Vec2f shoulder) const {
        constexpr float kPi = 3.14159265f;
        const float rad = angleUpper * kPi / 180.f;
        const float len = segmentLength(upper);
        return {shoulder.x + std::cos(rad) * len,
                shoulder.y + std::sin(rad) * len};
    }

    // Mão em coords de mundo dado o ombro.
    core::Vec2f handPos(core::Vec2f shoulder) const {
        constexpr float kPi = 3.14159265f;
        const float rad = (angleUpper + angleLower) * kPi / 180.f;
        const float len = segmentLength(lower);
        const core::Vec2f elbow = elbowPos(shoulder);
        return {elbow.x + std::cos(rad) * len,
                elbow.y + std::sin(rad) * len};
    }
};

} // namespace support
