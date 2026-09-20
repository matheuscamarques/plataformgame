#pragma once
#include <cstddef>

#include "core/sprite_from_ascii.h"

namespace sprites {

// Player 12x20, slime 14x12, dwarf 14x18. Top compartilhado via macros;
// teste trava widths (linha errada = sprite deslocada).

inline constexpr int kPlayerW = 12;
inline constexpr int kPlayerH = 20;

inline const core::PaletteEntry kPlayerPal[] = {
    {'.', {0, 0, 0, 0}},
    {'K', {30, 20, 20}},
    {'S', {230, 180, 140}},
    {'E', {20, 15, 15}},
    {'C', {60, 90, 160}},
    {'B', {50, 35, 30}},
    {'W', {190, 190, 200}},
    {'T', {220, 60, 50}},
    {'t', {90, 30, 25}},
};
inline constexpr std::size_t kPlayerPalCount = 9;

inline const char *const kPlayerIdle[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
    "..CCCCCCCC..",
    ".SCCCCCCCCS.",
    ".SCCCCCCCCS.",
    ".SCCCCCCCCS.",
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
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
    "..CCCCCCCC..",
    ".SCCCCCCCC..",
    ".SCCCCCCCC..",
    ".SCCCCCCCC..",
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
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
    "..CCCCCCCC..",
    "..CCCCCCCCS.",
    "..CCCCCCCCS.",
    "..CCCCCCCCS.",
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
    ".S........S.",
    ".S.KKKKKK.S.",
    ".SKKKKKKKKS.",
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
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
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
    "..CCCCCCCC..",
    ".SCCCCCCCC..",
    ".SCCCCCCCC..",
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
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
    "..CCCCCCCC..",
    "..CCCCCCCCS.",
    "..CCCCCCCCSS",
    "..CCCCCCCCS.",
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

inline const char *const kPlayerHurt[] = {
    ".S........S.",
    ".S.KKKKKK.S.",
    ".SKKKKKKKKS.",
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
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
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    ".SCCCCCCCCS.",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "..CC....CC..",
    "..CC....CC..",
    ".BB......BB.",
    "BB........BB",
    "............",
};

inline constexpr int kSlimeW = 14;
inline constexpr int kSlimeH = 12;

inline const core::PaletteEntry kSlimePal[] = {
    {'.', {0, 0, 0, 0}},
    {'O', {30, 70, 40}},
    {'G', {80, 180, 90}},
    {'L', {150, 230, 140}},
    {'E', {20, 25, 20}},
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
    {'.', {0, 0, 0, 0}},
    {'K', {20, 15, 10}},
    {'S', {230, 180, 140}},
    {'E', {20, 20, 20}},
    {'H', {140, 90, 60}},
    {'C', {120, 130, 150}},
    {'T', {90, 60, 40}},
    {'B', {50, 35, 30}},
    {'W', {180, 180, 190}},
    {'D', {220, 60, 50}},
};
inline constexpr std::size_t kDwarfPalCount = 10;

#define DWARF_TOP_IDLE       \
    "....CCCCCC....",       \
    "...CCCCCCCC...",       \
    "...CSSSSSSC...",       \
    "...SESSSSES...",       \
    "....SSSSSS....",       \
    "...HHHHHHHH...",       \
    "..HHHHHHHHHH..",       \
    "..HHHHHHHHHH..",       \
    ".SHHHHHHHHHHS.",       \
    ".TTTHHHHHHTTT.",       \
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
    "...CSSSSSSC...",
    "...SESSSSES...",
    "....SSSSSS....",
    "...HHHHHHHH...",
    "..HHHHHHHHHH..",
    "..HHHHHHHHHH.D",
    ".SHHHHHHHHHHSD",
    ".TTTHHHHHHTTT.",
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
    "...CSSSSSSC...",
    "...SESSSSES...",
    "....SSSSSS....",
    "...HHHHHHHH...",
    "..HHHHHHHHHH..",
    "..HHHHHHHHHH..",
    ".SHHHHHHHHHHS.",
    ".TTTHHHHHHTTT.",
    ".TTTTTTTTTTTT.",
    ".TTTTTTTTTTTT.",
    "..TTTTTTTTTT..",
    "..TTT....TTT..",
    "..BBB....BBB..",
    "..BBB....BBB..",
};

struct SpriteSet {
    sf::Texture playerIdle;
    sf::Texture playerWalkA;
    sf::Texture playerWalkB;
    sf::Texture playerJump;
    sf::Texture playerThrow;
    sf::Texture playerPunch;
    sf::Texture playerHurt;
    sf::Texture playerDeath;
    sf::Texture slimeIdle;
    sf::Texture slimeSquash;
    sf::Texture dwarfIdle;
    sf::Texture dwarfWalkA;
    sf::Texture dwarfWalkB;
    sf::Texture dwarfThrow;
    sf::Texture dwarfMelee;
};

// Roda 1x no boot (precisa de contexto GL — nunca em teste headless).
inline SpriteSet build() {
    SpriteSet s;
    auto P = [](const char *const *rows) {
        return core::makeSprite(rows, kPlayerW, kPlayerH,
                                kPlayerPal, kPlayerPalCount);
    };
    s.playerIdle = P(kPlayerIdle);
    s.playerWalkA = P(kPlayerWalkA);
    s.playerWalkB = P(kPlayerWalkB);
    s.playerJump = P(kPlayerJump);
    s.playerThrow = P(kPlayerThrow);
    s.playerPunch = P(kPlayerPunch);
    s.playerHurt = P(kPlayerHurt);
    s.playerDeath = P(kPlayerDeath);
    s.slimeIdle = core::makeSprite(kSlimeIdle, kSlimeW, kSlimeH,
                                   kSlimePal, kSlimePalCount);
    s.slimeSquash = core::makeSprite(kSlimeSquash, kSlimeW, kSlimeH,
                                     kSlimePal, kSlimePalCount);
    s.dwarfIdle = core::makeSprite(kDwarfIdle, kDwarfW, kDwarfH,
                                   kDwarfPal, kDwarfPalCount);
    s.dwarfWalkA = core::makeSprite(kDwarfWalkA, kDwarfW, kDwarfH,
                                    kDwarfPal, kDwarfPalCount);
    s.dwarfWalkB = core::makeSprite(kDwarfWalkB, kDwarfW, kDwarfH,
                                    kDwarfPal, kDwarfPalCount);
    s.dwarfThrow = core::makeSprite(kDwarfThrow, kDwarfW, kDwarfH,
                                    kDwarfPal, kDwarfPalCount);
    s.dwarfMelee = core::makeSprite(kDwarfMelee, kDwarfW, kDwarfH,
                                    kDwarfPal, kDwarfPalCount);
    return s;
}

} // namespace sprites
