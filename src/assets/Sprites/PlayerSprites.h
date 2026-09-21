#pragma once

#include "core/sprite_from_ascii.h"

namespace sprites {

// Player 12x20, slime 14x12, dwarf 14x18. Top compartilhado via macros;
// teste trava widths (linha errada = sprite deslocada).

inline constexpr int kPlayerW = 12;
inline constexpr int kPlayerH = 20;

inline const core::PaletteEntry kPlayerPal[] = {
    {'.', {0, 0, 0, 0}, core::BodyPartId::None},
    {'K', {30, 20, 20}, core::BodyPartId::None},
    {'F', {230, 180, 140}, core::BodyPartId::Head},
    {'H', {230, 180, 140}, core::BodyPartId::ArmR},
    {'E', {20, 15, 15}, core::BodyPartId::Head},
    {'C', {60, 90, 160}, core::BodyPartId::Torso},
    {'B', {50, 35, 30}, core::BodyPartId::LegR},
    {'W', {190, 190, 200}, core::BodyPartId::Weapon},
    {'T', {220, 60, 50}, core::BodyPartId::Weapon},
    {'t', {90, 30, 25}, core::BodyPartId::Weapon},
};
inline constexpr std::size_t kPlayerPalCount = 10;

inline const char *const kPlayerIdle[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    ".HCCCCCCCCH.",
    ".HCCCCCCCCH.",
    ".HCCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "............",
};

inline const char *const kPlayerWalkA[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    ".HCCCCCCCC..",
    ".HCCCCCCCC..",
    ".HCCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "............",
};

inline const char *const kPlayerWalkB[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "..CC....CC..",
    "..CC....CC..",
    ".BB......BB.",
    ".BB......BB.",
    ".BB......BB.",
    "............",
};
inline const char *const kPlayerJump[] = {
    ".H........H.",
    ".H.KKKKKK.H.",
    ".HKKKKKKKKH.",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "..BB....BB..",
    "............",
    "............",
    "............",
    "............",
};

inline const char *const kPlayerThrow[] = {
    "....KKKK..TT",
    "..KKKKKKKKtT",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    ".HCCCCCCCC..",
    ".HCCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "............",
    "............",
    "............",
};

inline const char *const kPlayerPunch[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    "..CCCCCCCCH.",
    "..CCCCCCCCHH",
    "..CCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...BB..BB...",
    "...BB..BB...",
    "...BB..BB...",
    "............",
};

inline const char *const kPlayerPunchUp[] = {
    ".HH......HH.",     // 0  mãos nas laterais
    ".HH......HH.",     // 1
    ".H..KKKK..H.",     // 2  cabeça + braços nas laterais
    ".HKKKKKKKKH.",     // 3
    ".HKFFFFFFKH.",     // 4
    ".HKFEFFEFKH.",     // 5
    ".HKFFFFFFKH.",     // 6
    "...FFFFFF...",     // 7  queixo (braços terminam em 6)
    "..CCCCCCCC..",     // 8  torso (+2 vs idle)
    "..CCCCCCCC..",     // 9
    "..CCCCCCCC..",     // 10
    "..CCCCCCCC..",     // 11
    "..CCCCCCCC..",     // 12
    "...CCCCCC...",     // 13
    "...CC..CC...",     // 14 pernas (mesma row)
    "...CC..CC...",     // 15
    "...BB..BB...",     // 16 botas (mesma row)
    "...BB..BB...",     // 17
    "...BB..BB...",     // 18
    "............",     // 19
};

inline const char *const kPlayerPunchDown[] = {
    "....KKKK....",     // 0  cabeça (igual idle)
    "..KKKKKKKK..",     // 1
    "..KFFFFFFK..",     // 2
    "..KFEFFEFK..",     // 3
    "..KFFFFFFK..",     // 4
    "...FFFFFF...",     // 5
    "..CCCCCCCC..",     // 6  ombros (igual idle)
    ".HCCCCCCCCH.",     // 7  braços descem
    ".HCCCCCCCCH.",     // 8
    ".HCCCCCCCCH.",     // 9
    ".HCCCCCCCCH.",     // 10
    "..HCCCCCCH..",     // 11 estreitando
    "..HCCCCCCH..",     // 12
    "...CCCCCC...",     // 13 cintura (igual idle)
    "...CC..CC...",     // 14 PERNAS (igual idle)
    "...CC..CC...",     // 15
    "...BB..BB...",     // 16
    "...BB..BB...",     // 17
    "...BB..BB...",     // 18
    "............",     // 19
};

inline const char *const kPlayerHurt[] = {
    ".H........H.",
    ".H.KKKKKK.H.",
    ".HKKKKKKKKH.",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "..CC....CC..",
    "..BB....BB..",
    "............",
    "............",
    "............",
};

inline const char *const kPlayerDeath[] = {
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "....KKKK....",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".HCCCCCCCCH.",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "..CC....CC..",
    "..CC....CC..",
    ".BB......BB.",
    "BB........BB",
    "............",
};

} // namespace sprites
