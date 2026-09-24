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
inline constexpr int kHelmW = 24;
inline constexpr int kHelmH = 10;
inline constexpr int kChestW = 24;
inline constexpr int kChestH = 16;
inline constexpr int kLegsW = 24;
inline constexpr int kLegsH = 12;

inline constexpr int kBootsW = 24;
inline constexpr int kBootsH = 6;

inline const char *const kIronBootsIdle[] = {
    "....WWWWWWWWWWWWWWWW....",
    "....WWWWWWWWWWWWWWWW....",
    "....WWwwwwwwwwwwwwWW....",
    "....WWwwwwwwwwwwwwWW....",
    "....WWWWWWWWWWWWWWWW....",
    "....WWWWWWWWWWWWWWWW....",
};

inline constexpr int kGloveW = 8;
inline constexpr int kGloveH = 6;

inline const char *const kIronGlovesIdle[] = {
    "..WWWW..",
    "..WWWW..",
    "WWWWWWww",
    "WWWWWWww",
    "..wwWW..",
    "..wwWW..",
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
    "GGGGGGGGGGGGGGGGGGGGGGGG",
    "GGGGGGGGGGGGGGGGGGGGGGGG",
    "GGwwwwwwwwwwwwwwwwwwwwGG",
    "GGwwwwwwwwwwwwwwwwwwwwGG",
    "GGwwwwwwwwwwwwwwwwwwwwGG",
    "GGwwwwwwwwwwwwwwwwwwwwGG",
    "GGww....EEEE....EEEEwwGG",
    "GGww....EEEE....EEEEwwGG",
    "GGww................wwGG",
    "GGww................wwGG",
};

inline const char *const kIronChestIdle[] = {
    "wwwwwwwwwwwwwwwwwwwwwwww",
    "wwwwwwwwwwwwwwwwwwwwwwww",
    "wwWWWWWWWWWWWWWWWWWWWWww",
    "wwWWWWWWWWWWWWWWWWWWWWww",
    "wwWWWWWWWWWWWWWWWWWWWWww",
    "wwWWWWWWWWWWWWWWWWWWWWww",
    "wwWWWWGGGGGGGGGGGGWWWWww",
    "wwWWWWGGGGGGGGGGGGWWWWww",
    "wwWWWWGGGGGGGGGGGGWWWWww",
    "wwWWWWGGGGGGGGGGGGWWWWww",
    "wwWWWWWWWWWWWWWWWWWWWWww",
    "wwWWWWWWWWWWWWWWWWWWWWww",
    "wwwwwwwwwwwwwwwwwwwwwwww",
    "wwwwwwwwwwwwwwwwwwwwwwww",
    "..wwwwwwwwwwwwwwwwwwww..",
    "..wwwwwwwwwwwwwwwwwwww..",
};

inline const char *const kIronLegsIdle[] = {
    "....WWWWWWWWWWWWWWWW....",
    "....WWWWWWWWWWWWWWWW....",
    "....WWwwwwwwwwwwwwWW....",
    "....WWwwwwwwwwwwwwWW....",
    "....WWwwWWWWWWWWwwWW....",
    "....WWwwWWWWWWWWwwWW....",
    "....WWwwWWGGGGWWwwWW....",
    "....WWwwWWGGGGWWwwWW....",
    "....WWwwwwwwwwwwwwWW....",
    "....WWwwwwwwwwwwwwWW....",
    "......WWWWWWWWWWWW......",
    "......WWWWWWWWWWWW......",
};

} // namespace sprites
