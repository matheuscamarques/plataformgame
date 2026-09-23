/**
 * @file src/core/Math.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Utilitários escalares puros para clamp, lerp e aproximação.
 * @details Oferece PI, clamp, lerp, sign e approach sem estado, incluído por física, IA e câmera.
 */

#pragma once

// Utilitários escalares puros (header-only, sem estado).
// Helpers de vetor SFML entram quando o primeiro usuário aparecer.
namespace core {

inline constexpr float PI = 3.14159265358979323846f;
inline constexpr float TAU = 2.0f * PI;
inline constexpr float EPSILON = 1e-6f;

template <typename T>
inline T clamp(T v, T lo, T hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

template <typename T>
inline int sign(T v) {
    return (v > T(0)) - (v < T(0));
}

// Move `v` em direção a `target` no máximo `maxDelta`. Sem overshoot.
inline float approach(float v, float target, float maxDelta) {
    float d = target - v;
    if (d > maxDelta) return v + maxDelta;
    if (d < -maxDelta) return v - maxDelta;
    return target;
}

} // namespace core
