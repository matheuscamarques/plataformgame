/**
 * @file src/core/Resistances.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Multiplicadores de dano recebido por tipo (Fase 1 elementais).
 * @details Struct Resistances com get/set/scale e clamp 0..3 mais applyResistance, incluído por Player, Enemy e testes.
 */

#pragma once

#include <algorithm>

#include "core/DamageType.h"

namespace core {

// Multiplicador de dano recebido. 1.0 = neutro, <1 = resistente,
// >1 = fraco. Ganho por atributo (player) e por dado (inimigos).
struct Resistances {
    float mult[static_cast<int>(DamageType::COUNT)] = {1.f, 1.f, 1.f, 1.f};

    float get(DamageType t) const {
        return mult[static_cast<int>(t)];
    }

    void set(DamageType t, float v) {
        mult[static_cast<int>(t)] = std::clamp(v, 0.f, 3.f);
    }

    void scale(DamageType t, float factor) {
        set(t, get(t) * factor);
    }
};

// Aplica o tipo: damage * resistência do alvo (trunca p/ int).
inline int applyResistance(int damage, DamageType t, const Resistances &r) {
    return static_cast<int>(damage * r.get(t));
}

} // namespace core
