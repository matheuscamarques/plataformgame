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
#include "support/Combat/Facing.h"

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
};

inline const char* const kPlayerIdleFeet[] = {
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};


// Idle: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerIdleArms[] = {
    "............",
    "............",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerIdleParts[] = {
    { kPlayerIdleHead, 12, 12, 0, 0 },
    { kPlayerIdleTorso, 12, 16, 0, 12 },
    { kPlayerIdleLegs, 12, 6, 0, 28 },
    { kPlayerIdleFeet, 12, 6, 0, 34 },
    { kPlayerIdleArms, 12, 16, 0, 12 },
};

// ---- Fase D, onda 1: Idle em 5 direções (S/SE/E/NE/N) ----
// Mesma grade 12x40 e mesmos chars→parte (F/E cabeça, G/H braços,
// C torso, L/B pernas); só a silhueta muda. S/N simétricos.

inline const char* const kPlayerIdleSHead[] = {
    "....KKKK....",
    "...KKKKKK...",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFK..",
    "..KFFEEFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
    "....FFFF....",
};

inline const char* const kPlayerIdleSTorso[] = {
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
    "..CCKKKKCC..",
    "..CCKKKKCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerIdleSLegs[] = {
    "..CC....CC..",
    "..CC....CC..",
    "..CC....CC..",
    "..CC....CC..",
    "..LL....BB..",
    "..LL....BB..",
};

inline const char* const kPlayerIdleSFeet[] = {
    "..LL....BB..",
    "..LL....BB..",
    ".LL......BB.",
    ".LL......BB.",
    "LL........BB",
    "LL........BB",
};

inline const char* const kPlayerIdleSArms[] = {
    "............",
    "............",
    "............",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerIdleSParts[] = {
    { kPlayerIdleSHead, 12, 12, 0, 0 },
    { kPlayerIdleSTorso, 12, 16, 0, 12 },
    { kPlayerIdleSLegs, 12, 6, 0, 28 },
    { kPlayerIdleSFeet, 12, 6, 0, 34 },
    { kPlayerIdleSArms, 12, 16, 0, 12 },
};

inline const char* const kPlayerIdleNHead[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "...KKKKKK...",
    "...FFFFFF...",
    "...FFFFFF...",
    "....FFFF....",
};

inline const char* const kPlayerIdleNTorso[] = {
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
    ".....KK.....",
    ".....KK.....",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerIdleNLegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "....LLBB....",
    "....LLBB....",
};

inline const char* const kPlayerIdleNFeet[] = {
    "....LLBB....",
    "....LLBB....",
    "....LLBB....",
    "....LLBB....",
    "............",
    "............",
};

inline const char* const kPlayerIdleNArms[] = {
    "............",
    "............",
    "............",
    "G..........H",
    "G..........H",
    "G..........H",
    "G..........H",
    "G..........H",
    "G..........H",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerIdleNParts[] = {
    { kPlayerIdleNHead, 12, 12, 0, 0 },
    { kPlayerIdleNTorso, 12, 16, 0, 12 },
    { kPlayerIdleNLegs, 12, 6, 0, 28 },
    { kPlayerIdleNFeet, 12, 6, 0, 34 },
    { kPlayerIdleNArms, 12, 16, 0, 12 },
};

inline const char* const kPlayerIdleSEHead[] = {
    "....KKKK....",
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFFFFFFKE.",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFKE.",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
};

inline const char* const kPlayerIdleSETorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GKCCCCCCCH.",
    ".GKCCCCCCCH.",
    ".GKCCCCCCCH.",
    ".GKCCCCCCCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerIdleSELegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC...CC..",
    "...CC...CC..",
    "...LL...BB..",
    "...LL...BB..",
};

inline const char* const kPlayerIdleSEFeet[] = {
    "...LL...BB..",
    "...LL...BB..",
    "...LL...BB..",
    "...LL...BB..",
    "............",
    "............",
};

inline const char* const kPlayerIdleSEArms[] = {
    "............",
    "............",
    ".G..........",
    ".G..........",
    ".G........H.",
    ".G........H.",
    ".........HH.",
    "..........H.",
    "..........H.",
    "..........H.",
    "..........H.",
    "..........H.",
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerIdleSEParts[] = {
    { kPlayerIdleSEHead, 12, 12, 0, 0 },
    { kPlayerIdleSETorso, 12, 16, 0, 12 },
    { kPlayerIdleSELegs, 12, 6, 0, 28 },
    { kPlayerIdleSEFeet, 12, 6, 0, 34 },
    { kPlayerIdleSEArms, 12, 16, 0, 12 },
};

inline const char* const kPlayerIdleNEHead[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KKKKKKKK..",
    "..KFFFFFFK..",
    "..KFEFFEFK..",
    "..KFEFFEFK..",
    "..KFFFFFFKE.",
    "..KFFFFFFK..",
    "..KFFFFFFK..",
    "...FFFFFF...",
    "...FFFFFF...",
    "....FFFF....",
};

inline const char* const kPlayerIdleNETorso[] = {
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".GCCCCCCCCH.",
    ".GCCCCCCCCH.",
    ".GCCCCCCKCH.",
    ".GCCCCCCKCH.",
    ".GCCCCCCKCH.",
    ".GCCCCCCKCH.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CCCCCC...",
};

inline const char* const kPlayerIdleNELegs[] = {
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    "...CC..CC...",
    ".......BB...",
    ".......BB...",
};

inline const char* const kPlayerIdleNEFeet[] = {
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    ".......BB...",
    ".......BB...",
};

inline const char* const kPlayerIdleNEArms[] = {
    "..........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G..........",
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

inline constexpr assets::Part kPlayerIdleNEParts[] = {
    { kPlayerIdleNEHead, 12, 12, 0, 0 },
    { kPlayerIdleNETorso, 12, 16, 0, 12 },
    { kPlayerIdleNELegs, 12, 6, 0, 28 },
    { kPlayerIdleNEFeet, 12, 6, 0, 34 },
    { kPlayerIdleNEArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerWalkAFeet[] = {
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};


// WalkA: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerWalkAArms[] = {
    "............",
    "............",
    ".G..........",
    ".G..........",
    ".G..........",
    ".G..........",
    ".G..........",
    ".G..........",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerWalkAParts[] = {
    { kPlayerWalkAHead, 12, 12, 0, 0 },
    { kPlayerWalkATorso, 12, 16, 0, 12 },
    { kPlayerWalkALegs, 12, 6, 0, 28 },
    { kPlayerWalkAFeet, 12, 6, 0, 34 },
    { kPlayerWalkAArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerWalkBFeet[] = {
    ".LL......BB.",
    ".LL......BB.",
    ".LL......BB.",
    ".LL......BB.",
    "............",
    "............",
};


// WalkB: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerWalkBArms[] = {
    "............",
    "............",
    "..........H.",
    "..........H.",
    "..........H.",
    "..........H.",
    "..........H.",
    "..........H.",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerWalkBParts[] = {
    { kPlayerWalkBHead, 12, 12, 0, 0 },
    { kPlayerWalkBTorso, 12, 16, 0, 12 },
    { kPlayerWalkBLegs, 12, 6, 0, 28 },
    { kPlayerWalkBFeet, 12, 6, 0, 34 },
    { kPlayerWalkBArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerJumpFeet[] = {
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};


// Jump: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerJumpArms[] = {
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
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerJumpParts[] = {
    { kPlayerJumpHead, 12, 12, 0, 0 },
    { kPlayerJumpTorso, 12, 16, 0, 12 },
    { kPlayerJumpLegs, 12, 6, 0, 28 },
    { kPlayerJumpFeet, 12, 6, 0, 34 },
    { kPlayerJumpArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerThrowFeet[] = {
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};


// Throw: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerThrowArms[] = {
    "............",
    "............",
    ".G..........",
    ".G..........",
    ".G..........",
    ".G..........",
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

inline constexpr assets::Part kPlayerThrowParts[] = {
    { kPlayerThrowHead, 12, 12, 0, 0 },
    { kPlayerThrowTorso, 12, 16, 0, 12 },
    { kPlayerThrowLegs, 12, 6, 0, 28 },
    { kPlayerThrowFeet, 12, 6, 0, 34 },
    { kPlayerThrowArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerPunchFeet[] = {
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};


// Punch: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerPunchArms[] = {
    "............",
    "............",
    "..........H.",
    "..........H.",
    "..........HH",
    "..........HH",
    "..........H.",
    "..........H.",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerPunchParts[] = {
    { kPlayerPunchHead, 12, 12, 0, 0 },
    { kPlayerPunchTorso, 12, 16, 0, 12 },
    { kPlayerPunchLegs, 12, 6, 0, 28 },
    { kPlayerPunchFeet, 12, 6, 0, 34 },
    { kPlayerPunchArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerPunchUpFeet[] = {
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};


// PunchUp: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerPunchUpArms[] = {
    ".G........H.",
    ".G........H.",
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
    "............",
    "............",
};

inline constexpr assets::Part kPlayerPunchUpParts[] = {
    { kPlayerPunchUpHead, 12, 12, 0, 0 },
    { kPlayerPunchUpTorso, 12, 16, 0, 12 },
    { kPlayerPunchUpLegs, 12, 6, 0, 28 },
    { kPlayerPunchUpFeet, 12, 6, 0, 34 },
    { kPlayerPunchUpArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerPunchDownFeet[] = {
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "...LL..BB...",
    "............",
    "............",
};


// PunchDown: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerPunchDownArms[] = {
    "............",
    "............",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    ".G........H.",
    "..G......H..",
    "..G......H..",
    "..G......H..",
    "..G......H..",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerPunchDownParts[] = {
    { kPlayerPunchDownHead, 12, 12, 0, 0 },
    { kPlayerPunchDownTorso, 12, 16, 0, 12 },
    { kPlayerPunchDownLegs, 12, 6, 0, 28 },
    { kPlayerPunchDownFeet, 12, 6, 0, 34 },
    { kPlayerPunchDownArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerHurtFeet[] = {
    "............",
    "............",
    "............",
    "............",
    "............",
    "............",
};


// Hurt: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerHurtArms[] = {
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
    "............",
    "............",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerHurtParts[] = {
    { kPlayerHurtHead, 12, 12, 0, 0 },
    { kPlayerHurtTorso, 12, 16, 0, 12 },
    { kPlayerHurtLegs, 12, 6, 0, 28 },
    { kPlayerHurtFeet, 12, 6, 0, 34 },
    { kPlayerHurtArms, 12, 16, 0, 12 },
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
};

inline const char* const kPlayerDeathFeet[] = {
    ".LL......BB.",
    ".LL......BB.",
    "LL........BB",
    "LL........BB",
    "............",
    "............",
};


// Death: Head(0,0) Torso(0,12) Legs(0,28) — 12+16+12=40.
inline const char* const kPlayerDeathArms[] = {
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
    ".G........H.",
    ".G........H.",
    "............",
    "............",
};

inline constexpr assets::Part kPlayerDeathParts[] = {
    { kPlayerDeathHead, 12, 12, 0, 0 },
    { kPlayerDeathTorso, 12, 16, 0, 12 },
    { kPlayerDeathLegs, 12, 6, 0, 28 },
    { kPlayerDeathFeet, 12, 6, 0, 34 },
    { kPlayerDeathArms, 12, 16, 0, 12 },
};

// Poses na ordem dos frames (espelha textureForFrame do Renderer).
enum class PlayerPose : uint8_t {
    Idle, WalkA, WalkB, Jump, Throw,
    Punch, PunchUp, PunchDown, Hurt, Death,
    COUNT
};

// Texturas no Renderer andam em ordem head/torso/ARMS/legs/feet, mas
// Parts[] em head/torso/LEGS/feet/arms (7a125e1 inseriu arms no meio
// das texturas sem remapear): índice da PARTE p/ índice da TEXTURA.
// Origem e skip de botas DEVEM usar isto, nunca o índice cru.
inline int partIndexForTex(int texIdx) {
    constexpr int kMap[5] = {0, 1, 4, 2, 3};
    return (texIdx >= 0 && texIdx < 5) ? kMap[texIdx] : 0;
}

inline constexpr int kPlayerPoseCount = 10;

inline const assets::Part* poseParts(PlayerPose p) {
    switch (p) {
        case PlayerPose::Idle:      return kPlayerIdleParts;
        case PlayerPose::WalkA:     return kPlayerWalkAParts;
        case PlayerPose::WalkB:     return kPlayerWalkBParts;
        case PlayerPose::Jump:      return kPlayerJumpParts;
        case PlayerPose::Throw:     return kPlayerThrowParts;
        case PlayerPose::Punch:     return kPlayerPunchParts;
        case PlayerPose::PunchUp:   return kPlayerPunchUpParts;
        case PlayerPose::PunchDown: return kPlayerPunchDownParts;
        case PlayerPose::Hurt:      return kPlayerHurtParts;
        case PlayerPose::Death:     return kPlayerDeathParts;
        default:                    return kPlayerIdleParts;
    }
}

// Direções com arte (Fase D, onda 1: Idle/Walk; resto cai em E).
// Ordem canônica p/ caches e texturas [pose][dir].
inline constexpr int kArtDirCount = 5;

inline int artDirIndex(support::Facing d) {
    switch (d) {
        case support::Facing::S:  return 0;
        case support::Facing::SE: return 1;
        case support::Facing::E:  return 2;
        case support::Facing::NE: return 3;
        case support::Facing::N:  return 4;
        default:                  return 2; // NW/W/SW: resolve p/ base antes
    }
}

// Inversa (loops de build/cache): 0..4 → S/SE/E/NE/N.
inline support::Facing artDirForIndex(int i) {
    switch (i) {
        case 0: return support::Facing::S;
        case 1: return support::Facing::SE;
        case 2: return support::Facing::E;
        case 3: return support::Facing::NE;
        case 4: return support::Facing::N;
        default: return support::Facing::E;
    }
}

// Partes por (pose, direção de arte). Onda 1 cobre Idle; resto cai
// em side-view (E) até sua onda.
inline const assets::Part* posePartsFor(PlayerPose p, support::Facing d) {
    if (p == PlayerPose::Idle) {
        switch (d) {
            case support::Facing::S:  return kPlayerIdleSParts;
            case support::Facing::SE: return kPlayerIdleSEParts;
            case support::Facing::NE: return kPlayerIdleNEParts;
            case support::Facing::N:  return kPlayerIdleNParts;
            default:                  return kPlayerIdleParts;
        }
    }
    return poseParts(p);
}

} // namespace sprites