/**
 * @file src/assets/Sprites/EnemySprites.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define sprites ASCII do slime e do anão inimigo.
 * @details Contém dimensões, paletas com BodyPartId e matrizes de caracteres 14x12 e 14x18, consumidas por SpriteSet build para gerar texturas.
 */

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
    ".........WWWW.",     // 0  pickaxe blade
    "....CCCCCCW...",     // 1  capacete topo + cabo
    "...CCCCCCCC...",     // 2  capacete
    "...CFFFFFFC...",     // 3  rosto
    "...FEFFFFEF...",     // 4  olhos
    "....FFFFFF....",     // 5  queixo
    "...RRRRRRRR...",     // 6  barba topo (comprimida)
    "..RRRRRRRRRR..",     // 7  barba
    ".HRRRRRRRRRRH.",     // 8  braços (mesma row do idle)
    ".TTTRRRRRRTTT.",     // 9  torso topo + barba
    ".TTTTTTTTTTTT.",     // 10 torso
    ".TTTTTTTTTTTT.",     // 11
    ".TTTTTTTTTTTT.",     // 12
    "..TTTTTTTTTT..",     // 13
    "..TTT....TTT..",     // 14 pernas (mesma row do idle)
    "..BBB....BBB..",     // 15 botas
    "..BBB....BBB..",     // 16
    "..BBB....BBB..",     // 17
};


// ---- Esqueleto (reflexo do player 12x40, paleta osso) ----
inline constexpr int kSkeletonW = 12;
inline constexpr int kSkeletonH = 40;

inline const core::PaletteEntry kSkeletonPal[] = {
    {'.', {0, 0, 0, 0}, core::BodyPartId::None},
    {'S', {200, 195, 170}, core::BodyPartId::Head},
    {'E', {10, 10, 15}, core::BodyPartId::Head},
    {'R', {210, 205, 185}, core::BodyPartId::Torso},
    {'G', {195, 190, 170}, core::BodyPartId::ArmL},
    {'H', {195, 190, 170}, core::BodyPartId::ArmR},
    {'B', {185, 175, 155}, core::BodyPartId::LegR},
    {'L', {185, 175, 155}, core::BodyPartId::LegL},
};
inline constexpr std::size_t kSkeletonPalCount = 8;

inline const char *const kSkeletonIdle[] = {
    "....SSSS....",
    "....SSSS....",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSESSESS..",
    "..SSESSESS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "...SSSSSS...",
    "...SSSSSS...",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    ".GRRRRRRRRH.",
    ".GRRRRRRRRH.",
    ".GRRRRRRRRH.",
    ".GRRRRRRRRH.",
    ".GRRRRRRRRH.",
    ".GRRRRRRRRH.",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "...RRRRRR...",
    "...RRRRRR...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "............",
    "............",
};

inline const char *const kSkeletonWalkA[] = {
    "....SSSS....",
    "....SSSS....",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSESSESS..",
    "..SSESSESS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "...SSSSSS...",
    "...SSSSSS...",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    ".GRRRRRRRR..",
    ".GRRRRRRRR..",
    ".GRRRRRRRR..",
    ".GRRRRRRRR..",
    ".GRRRRRRRR..",
    ".GRRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "...RRRRRR...",
    "...RRRRRR...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "............",
    "............",
};

inline const char *const kSkeletonWalkB[] = {
    "....SSSS....",
    "....SSSS....",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSESSESS..",
    "..SSESSESS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "...SSSSSS...",
    "...SSSSSS...",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRRH.",
    "..RRRRRRRRH.",
    "..RRRRRRRRH.",
    "..RRRRRRRRH.",
    "..RRRRRRRRH.",
    "..RRRRRRRRH.",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "...RRRRRR...",
    "...RRRRRR...",
    "..BB....BB..",
    "..BB....BB..",
    "..BB....BB..",
    "..BB....BB..",
    ".BB......BB.",
    ".BB......BB.",
    ".BB......BB.",
    ".BB......BB.",
    ".BB......BB.",
    ".BB......BB.",
    "............",
    "............",
};

inline const char *const kSkeletonMelee[] = {
    "....SSSS....",
    "....SSSS....",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "..SSESSESS..",
    "..SSESSESS..",
    "..SSSSSSSS..",
    "..SSSSSSSS..",
    "...SSSSSS...",
    "...SSSSSS...",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRRH.",
    "..RRRRRRRRH.",
    "..RRRRRRRRHH",
    "..RRRRRRRRHH",
    "..RRRRRRRRH.",
    "..RRRRRRRRH.",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "..RRRRRRRR..",
    "...RRRRRR...",
    "...RRRRRR...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "............",
    "............",
};

// ---- Pack voadores + dados (gerado por script, fonte: dwarf/slime) ----
inline constexpr int kPackW = 14;
inline constexpr int kPackH = 18;
inline constexpr int kPackSmallH = 12;

inline const core::PaletteEntry kHollowPal[] = {
    {'.', {0, 0, 0, 0}},
    {'K', {20, 15, 10}, core::BodyPartId::None},
    {'F', {170, 180, 165}, core::BodyPartId::Head},
    {'H', {170, 180, 165}, core::BodyPartId::ArmR},
    {'E', {15, 15, 15}, core::BodyPartId::Head},
    {'R', {100, 100, 105}, core::BodyPartId::Head},
    {'C', {90, 95, 105}, core::BodyPartId::Head},
    {'T', {120, 70, 50}, core::BodyPartId::Torso},
    {'B', {45, 35, 30}, core::BodyPartId::LegR},
    {'W', {180, 180, 190}, core::BodyPartId::Weapon},
    {'D', {220, 60, 50}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kHollowPalCount = 11;

inline const char *const kHollowIdle[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};

inline const char *const kHollowWalkB[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT.....TTT.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
};

inline const core::PaletteEntry kBurstPal[] = {
    {'.', {0, 0, 0, 0}},
    {'K', {20, 25, 15}, core::BodyPartId::None},
    {'F', {140, 200, 120}, core::BodyPartId::Head},
    {'H', {140, 200, 120}, core::BodyPartId::ArmR},
    {'E', {10, 10, 10}, core::BodyPartId::Head},
    {'R', {40, 90, 40}, core::BodyPartId::Head},
    {'C', {90, 110, 90}, core::BodyPartId::Head},
    {'T', {50, 120, 60}, core::BodyPartId::Torso},
    {'B', {30, 60, 35}, core::BodyPartId::LegR},
    {'W', {180, 180, 190}, core::BodyPartId::Weapon},
    {'D', {220, 60, 50}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kBurstPalCount = 11;

inline const char *const kBurstIdle[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};

inline const char *const kBurstWalkB[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT.....TTT.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
};

inline const core::PaletteEntry kImpPal[] = {
    {'.', {0, 0, 0, 0}},
    {'K', {25, 10, 10}, core::BodyPartId::None},
    {'F', {200, 120, 100}, core::BodyPartId::Head},
    {'H', {200, 120, 100}, core::BodyPartId::ArmR},
    {'E', {230, 200, 50}, core::BodyPartId::Head},
    {'R', {120, 40, 40}, core::BodyPartId::Head},
    {'C', {80, 40, 40}, core::BodyPartId::Head},
    {'T', {140, 50, 50}, core::BodyPartId::Torso},
    {'B', {50, 25, 25}, core::BodyPartId::LegR},
    {'W', {180, 180, 190}, core::BodyPartId::Weapon},
    {'D', {220, 60, 50}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kImpPalCount = 11;

inline const char *const kImpIdle[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};

inline const char *const kImpWalkB[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT.....TTT.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
};

inline const core::PaletteEntry kElementalPal[] = {
    {'.', {0, 0, 0, 0}},
    {'K', {30, 15, 10}, core::BodyPartId::None},
    {'F', {255, 180, 100}, core::BodyPartId::Head},
    {'H', {255, 180, 100}, core::BodyPartId::ArmR},
    {'E', {255, 255, 220}, core::BodyPartId::Head},
    {'R', {230, 120, 40}, core::BodyPartId::Head},
    {'C', {200, 80, 30}, core::BodyPartId::Head},
    {'T', {220, 100, 40}, core::BodyPartId::Torso},
    {'B', {90, 30, 20}, core::BodyPartId::LegR},
    {'W', {255, 230, 150}, core::BodyPartId::Weapon},
    {'D', {255, 120, 40}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kElementalPalCount = 11;

inline const char *const kElementalIdle[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};

inline const char *const kElementalWalkB[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT.....TTT.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
};

inline const core::PaletteEntry kUndeadPal[] = {
    {'.', {0, 0, 0, 0}},
    {'K', {20, 18, 15}, core::BodyPartId::None},
    {'F', {210, 205, 190}, core::BodyPartId::Head},
    {'H', {210, 205, 190}, core::BodyPartId::ArmR},
    {'E', {10, 10, 15}, core::BodyPartId::Head},
    {'R', {190, 185, 170}, core::BodyPartId::Head},
    {'C', {130, 90, 60}, core::BodyPartId::Head},
    {'T', {110, 110, 115}, core::BodyPartId::Torso},
    {'B', {45, 40, 35}, core::BodyPartId::LegR},
    {'W', {180, 180, 190}, core::BodyPartId::Weapon},
    {'D', {220, 60, 50}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kUndeadPalCount = 11;

inline const char *const kUndeadIdle[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};

inline const char *const kUndeadWalkB[] = {
    "....CCCCCC....",
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
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT.....TTT.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
    "..BBB.....BBB.",
};

inline const core::PaletteEntry kRatPal[] = {
    {'.', {0, 0, 0, 0}},
    {'O', {60, 40, 25}, core::BodyPartId::Torso},
    {'G', {130, 90, 50}, core::BodyPartId::Torso},
    {'L', {200, 160, 110}, core::BodyPartId::Head},
    {'E', {180, 30, 30}, core::BodyPartId::Head},
};
inline constexpr std::size_t kRatPalCount = 5;

inline const char *const kRatIdle[] = {
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

inline const char *const kRatSquash[] = {
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

inline const core::PaletteEntry kHarpyPal[] = {
    {'.', {0, 0, 0, 0}},
    {'B', {150, 110, 70}, core::BodyPartId::Torso},
    {'E', {20, 15, 15}, core::BodyPartId::Head},
    {'W', {200, 200, 210}, core::BodyPartId::None},
};
inline constexpr std::size_t kHarpyPalCount = 4;

inline const char *const kHarpyIdle[] = {
    "..............",
    "..............",
    ".....BB.......",
    "....BBBB......",
    "....BEEB......",
    "....BBBB......",
    ".....BB.......",
    "...BB..BB.....",
    "..WW....WW....",
    "..WW....WW....",
    "..............",
    "..............",
};

inline const char *const kHarpyWalkA[] = {
    "..WW....WW....",
    ".WWWB..BWWW...",
    ".WWBBBBBBWW...",
    "..WBBEEBBW....",
    "...BBBBBB.....",
    "....BBBB......",
    "...BB..BB.....",
    "..............",
    "..............",
    "..............",
    "..............",
    "..............",
};

inline const char *const kHarpyWalkB[] = {
    "..............",
    "..............",
    "..............",
    "..............",
    "...BBBBBB.....",
    "....BEEB......",
    "....BBBB......",
    "...BB..BB.....",
    "..WW....WW....",
    ".WWB....BWW...",
    ".WWB....BWW...",
    "..W......W....",
};

inline const core::PaletteEntry kEyePal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {230, 230, 235}, core::BodyPartId::Torso},
    {'E', {180, 60, 60}, core::BodyPartId::Head},
    {'K', {20, 10, 10}, core::BodyPartId::Head},
};
inline constexpr std::size_t kEyePalCount = 4;

inline const char *const kEyeIdle[] = {
    "..............",
    "....SSSS......",
    "..SSSSSSSS....",
    ".SSSEEEESSS...",
    ".SSEEEEEESS...",
    ".SSEEKKEESS...",
    ".SSEEEEEESS...",
    ".SSSEEEESSS...",
    "..SSSSSSSS....",
    "....SSSS......",
    "..............",
    "..............",
};

inline const char *const kEyeWalkB[] = {
    "..............",
    "....SSSS......",
    "..SSSSSSSS....",
    ".SSSEEEESSS...",
    ".SSEEEEEESS...",
    ".SSEEEKKEES...",
    ".SSEEEEEESS...",
    ".SSSEEEESSS...",
    "..SSSSSSSS....",
    "....SSSS......",
    "..............",
    "..............",
};

} // namespace sprites
