/**
 * @file src/assets/Sprites/EquipSprites.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define sprites ASCII de armas e armaduras por material.
 * @details Guarda medidas e matrizes de espada, elmo, peitoral, pernas, botas e luvas mais paleta base, usadas por SpriteSet para criar uma textura por material.
 */

#pragma once

#include "core/sprite_from_ascii.h"

namespace sprites {
// ============================================================
// EQUIPAMENTO — forma compartilhada, 1 textura por material.
// Espada 8x20 (idle/windup), 16x8 (swing); elmo 12x5; peitoral 12x8;
// perneiras 12x6. Paleta de 5 entradas (., W, w, G, E) por material.
inline constexpr int kSwordW = 8;
inline constexpr int kSwordH = 20;
inline constexpr int kSwordSwingW = 16;
inline constexpr int kSwordSwingH = 8;
inline constexpr int kHelmW = 12;
inline constexpr int kHelmH = 8;
inline constexpr int kChestW = 12;
inline constexpr int kChestH = 18;
inline constexpr int kLegsW = 12;
inline constexpr int kLegsH = 10;

inline constexpr int kBootsW = 12;
inline constexpr int kBootsH = 6;

inline const char *const kIronBootsIdle[] = {
    "..WWWWWWWW..",
    "..WwwwwwwW..",
    "..WwwwwwwW..",
    "..WWWWWWWW..",
    "..WWWWWWWW..",
    "............",
};

inline constexpr int kGloveW = 6;
inline constexpr int kGloveH = 5;

inline const char *const kIronGlovesIdle[] = {
    ".WWWW.",
    "WWWWWW",
    "WWwWWW",
    "WWWWWW",
    ".WWWW.",
};

inline const char *const kIronSwordIdle[] = {
    "....WWWW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "..GGGGGG",
    "....ww..",
    "....ww..",
    "....ww..",
    "....GG..",
    "....GG..",
    "........",
    "........",
    "........",
};

inline const char *const kIronAxeIdle[] = {
    "..WWWW..",
    "..WwwW..",
    "..WwwW..",
    "...WwW..",
    "....w...",
    "....w...",
    "....w...",
    "....w...",
    "....w...",
    "....w...",
    "....w...",
    "..GGGG..",
    "....w...",
    "....w...",
    "....GG..",
    "....GG..",
    "........",
    "........",
    "........",
    "........",
};

inline const char *const kIronSwordWindup[] = {
    "........",
    "........",
    "WWWW....",
    ".WwwW...",
    ".WwwW...",
    "..WwwW..",
    "..WwwW..",
    "...WwwW.",
    "...WwwW.",
    "....WwwW",
    "....WwwW",
    "....WwwW",
    "..GGGGGG",
    "....ww..",
    "....ww..",
    "....GG..",
    "....GG..",
    "........",
    "........",
    "........",
};

inline const char *const kIronSwordSwing[] = {
    "................",
    "................",
    "................",
    "................",
    "..GGGGGGWWWWWWWW",
    "..GGGGGGwwwwwwww",
    "....wwww........",
    "....GG..........",
};

inline const char *const kIronHelmIdle[] = {
    "...GGGGGG...",
    "..GGGGGGGG..",
    "..GWWWWWWG..",
    "..GWWWWWWG..",
    "..GWWWWWWG..",
    "..GWWWWWWG..",
    "..GGGGGGGG..",
    "............",
};

inline const char *const kIronChestIdle[] = {
    ".WWWWWWWWWW.",
    "WWWWWWWWWWWW",
    "WWWWWWWWWWWW",
    "WWWGGGGGGWWW",
    "WWWGGGGGGWWW",
    "WWWWWWWWWWWW",
    "WWWWWWWWWWWW",
    "WWWWWWWWWWWW",
    "WWWWWWWWWWWW",
    "WWWWWWWWWWWW",
    "WWWWWWWWWWWW",
    "wWWWWWWWWWWw",
    "wWWWWWWWWWWw",
    "wWWGGGGGGWWw",
    "wWWGGGGGGWWw",
    "wWWWWWWWWWWw",
    "wwwwwwwwwwww",
    "............",
};

inline const char *const kIronLegsIdle[] = {
    "..WWWWWWWW..",
    "..WwwwwwwW..",
    "..WwWWWWwW..",
    "..WwWWWWwW..",
    "..WwWGGWwW..",
    "..WwWGGWwW..",
    "..WwwwwwwW..",
    "..WwwwwwwW..",
    "...WWWWWW...",
    "...WWWWWW...",
};

} // namespace sprites
