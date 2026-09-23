/**
 * @file src/assets/Sprites/PlayerSprites.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define sprites ASCII e paleta do jogador 12x20.
 * @details Contém largura, altura, paleta com partes do corpo e frames idle, walk, jump, throw, punch, hurt e death, usados por SpriteSet e testes de largura.
 */

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

inline const char *const kPlayerIdle[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "..CCCCCCCC..",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
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
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
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
    ".LL......BB.",
    ".LL......BB.",
    ".LL......BB.",
    "............",
};
inline const char *const kPlayerJump[] = {
    ".G........H.",
    ".G.KKKKKK.H.",
    ".GKKKKKKKKH.",
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
    "..LL....BB..",
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
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
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
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
};

inline const char *const kPlayerPunchUp[] = {
    ".GG......HH.",     // 0  mãos nas laterais
    ".GG......HH.",     // 1
    ".G..KKKK..H.",     // 2  cabeça + braços nas laterais
    ".GKKKKKKKKH.",     // 3
    ".GKFFFFFFKH.",     // 4
    ".GKFEFFEFKH.",     // 5
    ".GKFFFFFFKH.",     // 6
    "...FFFFFF...",     // 7  queixo (braços terminam em 6)
    "..CCCCCCCC..",     // 8  torso (+2 vs idle)
    "..CCCCCCCC..",     // 9
    "..CCCCCCCC..",     // 10
    "..CCCCCCCC..",     // 11
    "..CCCCCCCC..",     // 12
    "...CCCCCC...",     // 13
    "...CC..CC...",     // 14 pernas (mesma row)
    "...CC..CC...",     // 15
    "...LL..BB...",     // 16 botas (mesma row)
    "...LL..BB...",     // 17
    "...LL..BB...",     // 18
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
    ".GCCCCCCCCH.",     // 7  braços descem
    ".GCCCCCCCCH.",     // 8
    ".GCCCCCCCCH.",     // 9
    ".GCCCCCCCCH.",     // 10
    "..GCCCCCCH..",     // 11 estreitando
    "..GCCCCCCH..",     // 12
    "...CCCCCC...",     // 13 cintura (igual idle)
    "...CC..CC...",     // 14 PERNAS (igual idle)
    "...CC..CC...",     // 15
    "...LL..BB...",     // 16
    "...LL..BB...",     // 17
    "...LL..BB...",     // 18
    "............",     // 19
};

inline const char *const kPlayerHurt[] = {
    ".G........H.",
    ".G.KKKKKK.H.",
    ".GKKKKKKKKH.",
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
    "..LL....BB..",
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
    ".GCCCCCCCCH.",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "..CC....CC..",
    "..CC....CC..",
    ".LL......BB.",
    "LL........BB",
    "............",
};

} // namespace sprites
