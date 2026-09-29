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
    static constexpr const char *kNames[] = {
        "physical", "fire", "frost", "lightning",
    };
    static_assert(sizeof(kNames) / sizeof(kNames[0]) ==
                      static_cast<std::size_t>(DamageType::COUNT),
                  "damageTypeName: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(t);
    if (i < 0 || i >= static_cast<int>(DamageType::COUNT)) return "?";
    return kNames[i];
}

} // namespace core
