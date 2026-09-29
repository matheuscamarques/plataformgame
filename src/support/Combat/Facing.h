/**
 * @file src/support/Combat/Facing.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Direção do corpo em 8 vias + helpers de espelhamento e input.
 * @details Fase A do plano de profundidade: Facing é alias de AimDir
 * (um único enum de 8 valores, sem duplicação). Arte só existe para
 * 5 direções base (S, SE, E, NE, N); NW/W/SW espelham horizontalmente.
 * Puro, sem SFML/GL: testável headless. Entity::facing (±1) continua
 * como derivado via facingSign() até a migração completa.
 */

#pragma once

#include <cmath>

#include "support/Combat/AimDir.h"

namespace support {

// Alias intencional: corpo e mira falam a mesma língua.
// Mantém o nome Facing para o código de sprite ler intenção
// (corpo), sem criar um segundo enum de 8 valores que divergiria.
using Facing = AimDir;

// Espelha? NW/W/SW desenham como NE/E/SE com flip horizontal.
inline bool isMirrored(Facing f) {
    return f == Facing::NW || f == Facing::W || f == Facing::SW;
}

// Direção base com arte própria (desfaz o espelho).
inline Facing baseDir(Facing f) {
    switch (f) {
        case Facing::NW: return Facing::NE;
        case Facing::W:  return Facing::E;
        case Facing::SW: return Facing::SE;
        default:         return f;
    }
}

// Derivação para os sistemas legados que leem Entity::facing (±1):
// esquerda espelhada = -1, resto = +1.
inline int facingSign(Facing f) { return isMirrored(f) ? -1 : 1; }

// Setor de 45° a partir do input. vx = direita+, vy = baixo+
// (screen-space, igual a aimVector). Parado mantém a direção antiga.
inline Facing facingFromInput(float vx, float vy, Facing old) {    if (vx == 0.f && vy == 0.f) return old;
    constexpr float kPi = 3.14159265f;
    float angle = std::atan2(-vy, vx) * 180.f / kPi; // -180..180
    if (angle < 0.f) angle += 360.f;                 // 0..360
    const int sector = static_cast<int>((angle + 22.5f) / 45.f) % 8;
    static const Facing kTable[8] = {
        Facing::E,  Facing::NE, Facing::N,  Facing::NW,
        Facing::W,  Facing::SW, Facing::S,  Facing::SE,
    };
    return kTable[sector];
}

// Como acima, mas com deadzone de magnitude: |v| ~ 0 mantém a direção
// antiga em vez de fatiar ruído (evita flicker com vx ≈ 0 caindo).
// O eixo dominante manda (8 setores); plataforma usa vx, top-down usa os dois.
inline Facing facingFromVelocity(float vx, float vy, Facing old,
                                 float dead = 1.f) {
    if (std::fabs(vx) <= dead && std::fabs(vy) <= dead) return old;
    return facingFromInput(vx, vy, old);
}

} // namespace support
