/**
 * @file src/core/DamageType.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Tipos de dano elementais (Fase 1 elementais, sem SFML).
 * @details Enum DamageType com Physical/Fire/Frost/Lightning mais nome, incluído por Skills, combate e resistências.
 */

#pragma once

#include <cstdint>

namespace core {

// Tipo de dano (Fase 1: só classificação + resistência; frost vira
// status na Fase 2, spells elementais na Fase 3).
enum class DamageType : uint8_t {
    Physical = 0,
    Fire = 1,
    Frost = 2,
    Lightning = 3,
    COUNT
};

inline const char *damageTypeName(DamageType t) {
    switch (t) {
        case DamageType::Physical:  return "physical";
        case DamageType::Fire:      return "fire";
        case DamageType::Frost:     return "frost";
        case DamageType::Lightning: return "lightning";
        default:                    return "?";
    }
}

} // namespace core
