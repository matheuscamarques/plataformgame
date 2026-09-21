#pragma once

#include "core/sprite_from_ascii.h"

namespace sprites {
inline constexpr int kSlimeW = 14;
inline constexpr int kSlimeH = 12;

inline const core::PaletteEntry kSlimePal[] = {
    {'.', {0, 0, 0, 0}, core::BodyPartId::None},
    {'O', {30, 70, 40}, core::BodyPartId::Torso},
    {'G', {80, 180, 90}, core::BodyPartId::Torso},
    {'L', {150, 230, 140}, core::BodyPartId::Head},
    {'E', {20, 25, 20}, core::BodyPartId::Head},
};
inline constexpr std::size_t kSlimePalCount = 5;

inline const char *const kSlimeIdle[] = {
    "....OOOOOO....",
    "..OOLLLLLLOO..",
    ".OLLLLLLLLLLO.",
    "OLLGEGGGGEGLLO",
    "OLLGEGGGGEGLLO",
    "OLLGGGGGGGGLLO",
    "OLLGGGGGGGGLLO",
    "OLLGGGGGGGGLLO",
    "OLLGGGGGGGGLLO",
    "OLLGGGGGGGGLLO",
    ".OOGGGGGGGGOO.",
    "..OOOOOOOOOO..",
};
inline const char *const kSlimeSquash[] = {
    "..............",
    "..............",
    "..OOOOOOOOOO..",
    ".OOLLLLLLLLOO.",
    "OLLGEGGGGEGLLO",
    "OLLGEGGGGEGLLO",
    "OLLGGGGGGGGLLO",
    "OLLGGGGGGGGLLO",
    "OLLGGGGGGGGLLO",
    "OLLGGGGGGGGLLO",
    ".OOGGGGGGGGOO.",
    "..OOOOOOOOOO..",
};

inline constexpr int kDwarfW = 14;
inline constexpr int kDwarfH = 18;

inline const core::PaletteEntry kDwarfPal[] = {
    {'.', {0, 0, 0, 0}, core::BodyPartId::None},
    {'K', {20, 15, 10}, core::BodyPartId::None},
    {'F', {230, 180, 140}, core::BodyPartId::Head},
    {'H', {230, 180, 140}, core::BodyPartId::ArmR},
    {'E', {20, 20, 20}, core::BodyPartId::Head},
    {'R', {140, 90, 60}, core::BodyPartId::Head},
    {'C', {120, 130, 150}, core::BodyPartId::Head},
    {'T', {90, 60, 40}, core::BodyPartId::Torso},
    {'B', {50, 35, 30}, core::BodyPartId::LegR},
    {'W', {180, 180, 190}, core::BodyPartId::Weapon},
    {'D', {220, 60, 50}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kDwarfPalCount = 11;

#define DWARF_TOP_IDLE       \
    "....CCCCCC....",       \
    "...CCCCCCCC...",       \
    "...CFFFFFFC...",       \
    "...FEFFFFEF...",       \
    "....FFFFFF....",       \
    "...RRRRRRRR...",       \
    "..RRRRRRRRRR..",       \
    "..RRRRRRRRRR..",       \
    ".HRRRRRRRRRRH.",       \
    ".TTTRRRRRRTTT.",       \
    ".TTTTTTTTTTTT.",       \
    ".TTTTTTTTTTTT.",       \
    ".TTTTTTTTTTTT.",       \
    "..TTTTTTTTTT.."

#define DWARF_LEGS_IDLE     \
    "..TTT....TTT..",       \
    "..BBB....BBB..",       \
    "..BBB....BBB..",       \
    "..BBB....BBB.."

#define DWARF_LEGS_WALK     \
    "..TTT.....TTT.",       \
    "..BBB.....BBB.",       \
    "..BBB.....BBB.",       \
    "..BBB.....BBB."

inline const char *const kDwarfIdle[] = {DWARF_TOP_IDLE, DWARF_LEGS_IDLE};
inline const char *const kDwarfWalkA[] = {DWARF_TOP_IDLE, DWARF_LEGS_IDLE};
inline const char *const kDwarfWalkB[] = {DWARF_TOP_IDLE, DWARF_LEGS_WALK};
inline const char *const kDwarfThrow[] = {
    "....CCCCCC....",
    "...CCCCCCCC...",
    "...CFFFFFFC...",
    "...FEFFFFEF...",
    "....FFFFFF....",
    "...RRRRRRRR...",
    "..RRRRRRRRRR..",
    "..RRRRRRRRRR.D",
    ".HRRRRRRRRRRHD",
    ".TTTRRRRRRTTT.",
    ".TTTTTTTTTTTT.",
    ".TTTTTTTTTTTT.",
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};
inline const char *const kDwarfMelee[] = {
    ".........WWWW.",
    "..........W...",
    "....CCCCCCW...",
    "...CCCCCCCC...",
    "...CFFFFFFC...",
    "...FEFFFFEF...",
    "....FFFFFF....",
    "...RRRRRRRR...",
    "..RRRRRRRRRR..",
    "..RRRRRRRRRR..",
    ".HRRRRRRRRRRH.",
    ".TTTRRRRRRTTT.",
    ".TTTTTTTTTTTT.",
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};

} // namespace sprites
