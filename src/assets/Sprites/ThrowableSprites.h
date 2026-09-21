#pragma once

#include "core/sprite_from_ascii.h"

namespace sprites {
// ============================================================
// THROWABLES — TNT animada em 3 frames (6x8), 1 textura por frame
// (sem material). Frame por fuse: >0.66 fresh, 0.33-0.66 burning,
// <0.33 critical. Todas as rows têm 6 chars (travado em teste).
inline constexpr int kTntW = 6;
inline constexpr int kTntH = 8;

inline const core::PaletteEntry kTntPal[] = {
    {'.', {0, 0, 0, 0},   core::BodyPartId::None},
    {'T', {220, 60, 50},  core::BodyPartId::None},
    {'t', {90, 30, 25},   core::BodyPartId::None},
    {'F', {240, 200, 80}, core::BodyPartId::None},
    {'s', {255, 255, 220}, core::BodyPartId::None},
};
inline constexpr std::size_t kTntPalCount = 5;

inline const char *const kTntFresh[] = {
    "...s..",
    "...F..",
    "...F..",
    "..TTT.",
    ".TTTTT",
    ".ttttt",
    ".TTTTT",
    ".TTTTT",
};

inline const char *const kTntBurning[] = {
    "......",
    "...s..",
    "...F..",
    "..TTT.",
    ".TTTTT",
    ".ttttt",
    ".TTTTT",
    ".TTTTT",
};

inline const char *const kTntCritical[] = {
    "......",
    "......",
    "..s...",
    "..TTT.",
    ".TTTTT",
    ".ttttt",
    ".TTTTT",
    ".TTTTT",
};

} // namespace sprites
