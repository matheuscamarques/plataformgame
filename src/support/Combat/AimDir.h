#include "core/Vec.h"
/**
 * @file src/support/Combat/AimDir.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define a direção de mira em 8 vias com helpers de conversão.
 * @details Expõe enum AimDir mais aimVector e resolveAim, incluído por Player e IAs para tiro e melee direcional.
 */

#pragma once
#include <SFML/System/Vector2.hpp>
#include <cstdint>

namespace support {

// Direção de mira em 8 vias. Não é Player-specific (inimigos usam no
// futuro); por isso mora em header compartilhado, não em Player.h.
enum class AimDir : uint8_t {
    E = 0,
    NE,
    N,
    NW,
    W,
    SW,
    S,
    SE,
    COUNT
};

inline core::Vec2f aimVector(AimDir d) {
    constexpr float k = 0.70710678f;
    switch (d) {
        case AimDir::E: return {1.f, 0.f};
        case AimDir::NE: return {k, -k};
        case AimDir::N: return {0.f, -1.f};
        case AimDir::NW: return {-k, -k};
        case AimDir::W: return {-1.f, 0.f};
        case AimDir::SW: return {-k, k};
        case AimDir::S: return {0.f, 1.f};
        case AimDir::SE: return {k, k};
        default: return {1.f, 0.f};
    }
}

inline AimDir resolveAim(bool up, bool down, bool left, bool right, int facing) {
    if (up && right) return AimDir::NE;
    if (up && left) return AimDir::NW;
    if (down && right) return AimDir::SE;
    if (down && left) return AimDir::SW;
    if (up) return AimDir::N;
    if (down) return AimDir::S;
    if (left) return AimDir::W;
    if (right) return AimDir::E;
    return (facing >= 0) ? AimDir::E : AimDir::W;
}

} // namespace support
