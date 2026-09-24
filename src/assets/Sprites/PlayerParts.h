/**
 * @file src/assets/Sprites/PlayerParts.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Partes do sprite do player 12x40 (cabeca/torso/pernas por frame).
 * @details Fase A da migracao p/ composicao: fatiamento mecanico dos frames
 * monolíticos (Head 0-11, Torso 12-27, Legs 28-39). Fonte temporaria —
 * a Fase E remove os monolíticos e estas viram a fonte unica.
 */

#pragma once

#include "assets/SpriteComposer.h"

namespace sprites {

inline const char* const kPlayerIdleHead[] = {
    "....KKKK....",
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
};

inline const char* const kPlayerIdleTorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerIdleLegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};

// Idle: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerIdleParts[] = {
    { kPlayerIdleHead, 12, 12, 0, 0 },
    { kPlayerIdleTorso, 12, 16, 0, 12 },
    { kPlayerIdleLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerWalkAHead[] = {
    "....KKKK....",
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
};

inline const char* const kPlayerWalkATorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerWalkALegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};

// WalkA: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerWalkAParts[] = {
    { kPlayerWalkAHead, 12, 12, 0, 0 },
    { kPlayerWalkATorso, 12, 16, 0, 12 },
    { kPlayerWalkALegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerWalkBHead[] = {
    "....KKKK....",
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
};

inline const char* const kPlayerWalkBTorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerWalkBLegs[] = {
    "..CC....CC..",
    "..CC....CC..",
    "..CC....CC..",
    "..CC....CC..",
    ".LL......BB.",
    ".LL......BB.",
    ".LL......BB.",
    ".LL......BB.",
    ".LL......BB.",
    ".LL......BB.",
    "............",
    "............",
};

// WalkB: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerWalkBParts[] = {
    { kPlayerWalkBHead, 12, 12, 0, 0 },
    { kPlayerWalkBTorso, 12, 16, 0, 12 },
    { kPlayerWalkBLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerJumpHead[] = {
    ".G........H.",
    ".G........H.",
    ".G.KKKKKK.H.",
    ".G.KKKKKK.H.",
    ".GKKKKKKKKH.",
    ".GKKKKKKKKH.",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
};

inline const char* const kPlayerJumpTorso[] = {
    "...FFFFFF...",
    "...FFFFFF...",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerJumpLegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "..LL....BB..",
    "..LL....BB..",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

// Jump: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerJumpParts[] = {
    { kPlayerJumpHead, 12, 12, 0, 0 },
    { kPlayerJumpTorso, 12, 16, 0, 12 },
    { kPlayerJumpLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerThrowHead[] = {
    "....KKKK..TT",
    "....KKKK..TT",
    "..KKKKKKKKtT",
    "..KKKKKKKKtT",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
};

inline const char* const kPlayerThrowTorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    ".GCCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
};

inline const char* const kPlayerThrowLegs[] = {
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

// Throw: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerThrowParts[] = {
    { kPlayerThrowHead, 12, 12, 0, 0 },
    { kPlayerThrowTorso, 12, 16, 0, 12 },
    { kPlayerThrowLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerPunchHead[] = {
    "....KKKK....",
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
};

inline const char* const kPlayerPunchTorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCCHH",
    "..CCCCCCCCHH",
    "..CCCCCCCCH.",
    "..CCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerPunchLegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};

// Punch: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerPunchParts[] = {
    { kPlayerPunchHead, 12, 12, 0, 0 },
    { kPlayerPunchTorso, 12, 16, 0, 12 },
    { kPlayerPunchLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerPunchUpHead[] = {
    ".GG......HH.",
    ".GG......HH.",
    ".GG......HH.",
    ".GG......HH.",
    ".G..KKKK..H.",
    ".G..KKKK..H.",
    ".GKKKKKKKKH.",
    ".GKKKKKKKKH.",
    ".GKFFFFFFKH.",
    ".GKFFFFFFKH.",
    ".GKFEFFEFKH.",
    ".GKFEFFEFKH.",
};

inline const char* const kPlayerPunchUpTorso[] = {
    ".GKFFFFFFKH.",
    ".GKFFFFFFKH.",
    "...FFFFFF...",
    "...FFFFFF...",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerPunchUpLegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};

// PunchUp: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerPunchUpParts[] = {
    { kPlayerPunchUpHead, 12, 12, 0, 0 },
    { kPlayerPunchUpTorso, 12, 16, 0, 12 },
    { kPlayerPunchUpLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerPunchDownHead[] = {
    "....KKKK....",
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
};

inline const char* const kPlayerPunchDownTorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    "..GCCCCCCH..",
    "..GCCCCCCH..",
    "..GCCCCCCH..",
    "..GCCCCCCH..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerPunchDownLegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};

// PunchDown: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerPunchDownParts[] = {
    { kPlayerPunchDownHead, 12, 12, 0, 0 },
    { kPlayerPunchDownTorso, 12, 16, 0, 12 },
    { kPlayerPunchDownLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerHurtHead[] = {
    ".G........H.",
    ".G........H.",
    ".G.KKKKKK.H.",
    ".G.KKKKKK.H.",
    ".GKKKKKKKKH.",
    ".GKKKKKKKKH.",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
};

inline const char* const kPlayerHurtTorso[] = {
    "...FFFFFF...",
    "...FFFFFF...",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerHurtLegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "..CC....CC..",
    "..CC....CC..",
    "..LL....BB..",
    "..LL....BB..",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

// Hurt: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerHurtParts[] = {
    { kPlayerHurtHead, 12, 12, 0, 0 },
    { kPlayerHurtTorso, 12, 16, 0, 12 },
    { kPlayerHurtLegs, 12, 12, 0, 28 },
};

inline const char* const kPlayerDeathHead[] = {
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

inline const char* const kPlayerDeathTorso[] = {
    "....KKKK....",
    "....KKKK....",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
};

inline const char* const kPlayerDeathLegs[] = {
    "...CCCCCC...",
    "...CCCCCC...",
    "..CC....CC..",
    "..CC....CC..",
    "..CC....CC..",
    "..CC....CC..",
    ".LL......BB.",
    ".LL......BB.",
    "LL........BB",
    "LL........BB",
    "............",
    "............",
};

// Death: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline constexpr assets::Part kPlayerDeathParts[] = {
    { kPlayerDeathHead, 12, 12, 0, 0 },
    { kPlayerDeathTorso, 12, 16, 0, 12 },
    { kPlayerDeathLegs, 12, 12, 0, 28 },
};

} // namespace sprites