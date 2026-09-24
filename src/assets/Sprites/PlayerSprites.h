/**
 * @file src/assets/Sprites/PlayerSprites.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define paleta e dims do jogador 12x40 (frames em PlayerParts.h).
 * @details Contém largura, altura e paleta com partes do corpo. Fase E3:
 * frames monolíticos deletados (fonte única: partes). Slime/dwarf ficam.
 */

#pragma once

#include "core/sprite_from_ascii.h"

namespace sprites {

// Player 12x40 (stretch 2x vertical do 12x20; slime 14x12, dwarf 14x18).
// Top compartilhado via macros; teste trava widths (linha errada =
// sprite deslocada).

inline constexpr int kPlayerW = 12;
inline constexpr int kPlayerH = 40;

inline const core::PaletteEntry kPlayerPal[] = {
    {'.', {0, 0, 0, 0}, core::BodyPartId::None},
    {'K', {30, 20, 20}, core::BodyPartId::None},
    {'F', {230, 180, 140}, core::BodyPartId::Head},
    {'H', {230, 180, 140}, core::BodyPartId::ArmR},
    {'G', {230, 180, 140}, core::BodyPartId::ArmL},
    {'E', {20, 15, 15}, core::BodyPartId::Head},
    {'C', {60, 90, 160}, core::BodyPartId::Torso},
    {'B', {50, 35, 30}, core::BodyPartId::LegR},
    {'L', {50, 35, 30}, core::BodyPartId::LegL},
    {'W', {190, 190, 200}, core::BodyPartId::Weapon},
    {'T', {220, 60, 50}, core::BodyPartId::Weapon},
    {'t', {90, 30, 25}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kPlayerPalCount = 12;


} // namespace sprites
