/**
 * @file src/assets/Sprites/PlayerSprites.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define paleta e dims do jogador 40x40 (frames em PlayerParts.h).
 * @details Contém largura, altura e paleta com partes do corpo. Fase E3:
 * frames monolíticos deletados (fonte única: partes). Slime/dwarf ficam.
 */

#pragma once

#include "assets/Sprites/PlayerPalette.h"
#include "core/sprite_from_ascii.h"

namespace sprites {

// Player 40x40, arte 12-wide centrada (cols 14-25); slime 14x12, dwarf 14x18.
// Top compartilhado via macros; teste trava widths (linha errada =
// sprite deslocada).

inline constexpr int kPlayerW = 40;
inline constexpr int kPlayerH = 40;

// Paleta (cores+hitbox): fonte única em PlayerPalette.h.
// Este header guarda só dims.


} // namespace sprites
