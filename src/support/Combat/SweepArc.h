/**
 * @file src/support/Combat/SweepArc.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Arco de varredura do golpe (Fase E): cone + anel, puro.
 * @details Modelo cone (não start/end): a mão B.3 fica parada no Active,
 * então o setor varrido num tick é a pegada estática da lâmina —
 * o sweep efetivo sai da aplicação repetida + 1 hit por swing.
 * Recuo→extensão seriam 180° (degenerado), por isso centro+abertura.
 * Ângulos em radianos screen-space (0=E, +horário, como o IK).
 * Sem SFML/GL: testável headless.
 */

#pragma once

#include <cmath>

#include "core/Vec.h"

namespace support {

// Meia-abertura do cone (lâmina cobre ±22° do eixo da mão: na ponta
// da espada ≈ ±18px, o envelope do rect legado ±14px).
inline constexpr float kSweepHalfWidth = 0.38397244f; // 22° em rad

struct SweepArc {
    core::Vec2f origin{0.f, 0.f}; // ombro
    float centerAngle = 0.f;       // eixo ombro→mão (rad)
    float halfWidth = kSweepHalfWidth;
    float rInner = 0.f; // dentro: zona morta (braço/corpo)
    float rOuter = 0.f; // ponta da lâmina
    bool empty = true;
};

inline bool sweepHitsCircle(const SweepArc& arc, core::Vec2f center,
                            float radius) {
    if (arc.empty) return false;
    constexpr float kPi = 3.14159265f;
    const float dx = center.x - arc.origin.x;
    const float dy = center.y - arc.origin.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 1e-6f) return true;                // em cima do ombro
    if (dist > arc.rOuter + radius) return false;  // além da ponta
    if (dist + radius < arc.rInner) return false;  // dentro da zona morta
    // Margem angular pelo tamanho do alvo (graze conta).
    float margin = radius / dist;
    if (margin > kPi) margin = kPi;
    float d = std::atan2(dy, dx) - arc.centerAngle;
    while (d > kPi) d -= 2.f * kPi;
    while (d < -kPi) d += 2.f * kPi;
    return std::fabs(d) <= arc.halfWidth + margin;
}

} // namespace support
