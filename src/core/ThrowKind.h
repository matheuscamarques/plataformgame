/**
 * @file src/core/ThrowKind.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Enum neutro de projéteis/arremessáveis (sem SFML, sem support).
 * @details Morava em support/Effects/Throwable.h e forçava core/ItemDef.h a incluir support (test-layers quebrava); incluído por ItemDef e Throwable.
 */

#pragma once

#include <cstdint>

namespace core {

// Kind de arremessável (dado puro; comportamento mora nos sistemas).
enum class ThrowKind : uint8_t {
    Dynamite,
    GoldNugget,   // stub — passivo futuro
    Rock,         // stub — variação futura
    Spit,         // projétil de slime: linear, sem fuse, dano no impacto
    Barrel,       // barril do anão: rola, detona no fuse (sem quique ainda)
    Tnt,          // escada de bombas: TNT (média)
    C4,           // C4 militar (grande)
    Daisy,        // BLU-82 Daisy Cutter (enorme)
    Moab,         // MOAB (colossal, 1 chunk)
    Bolt          // magia do player (F8b): linear, dano no impacto
};

} // namespace core
