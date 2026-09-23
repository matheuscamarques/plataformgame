/**
 * @file src/core/BodyPart.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Enum que identifica partes do corpo para hitboxes e renderização.
 * @details Define BodyPartId com cabeça, torso, braços, pernas e arma, usado por Body, Renderer e sprite_from_ascii para ligar pixel a dano.
 */

#pragma once
#include <cstdint>

namespace core {

// Dono do enum: core (nunca o contrário — target test-layers no
// Makefile trava dependência de core para support).
enum class BodyPartId : uint8_t {
    None = 0, // pixel sem dono / parte ausente
    Head,
    Torso,
    ArmL,
    ArmR,
    LegL,
    LegR,
    Weapon,
    COUNT
};

} // namespace core
