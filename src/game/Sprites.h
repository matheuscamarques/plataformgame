#pragma once
#include <array>
#include <cstddef>

#include "../core/sprite_from_ascii.h"

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
};
inline constexpr std::size_t kPlayerPalCount = 6;

#define PLAYER_TOP_NEUTRAL   \
    "....KKKK....",          \
    "..KKKKKKKK..",          \
    "..KSSSSSSK..",          \
    "..KSESSESK..",          \
    "..KSSSSSSK..",          \
    "...SSSSSS...",          \
    "....SSSS....",          \
    "...CCCCCC...",          \
    "..CCCCCCCC..",          \
    ".SCCCCCCCCS.",          \
    "..CCCCCCCC..",          \
    "..CCCCCCCC..",          \
    "..CCCCCCCC..",          \
    "...CCCCCC..."

#define PLAYER_LEGS_IDLE    \
    "...CC..CC...",         \
    "...CC..CC...",         \
    "...BB..BB...",         \
    "...BB..BB...",         \
    "...BB..BB...",         \
    "............"

#define PLAYER_LEGS_WALK_R  \
    "..CC...CC...",         \
    ".CC.....CC..",         \
    "BB.......BB.",         \
    "BB.......BB.",         \
    "............",         \
    "............"

#define PLAYER_LEGS_WALK_L  \
    "...CC...CC..",         \
    "..CC.....CC.",         \
    ".BB.......BB",         \
    ".BB.......BB",         \
    "............",         \
    "............"

inline const char *const kPlayerIdle[] = {PLAYER_TOP_NEUTRAL, PLAYER_LEGS_IDLE};
inline const char *const kPlayerWalk0[] = {PLAYER_TOP_NEUTRAL, PLAYER_LEGS_IDLE};
inline const char *const kPlayerWalk1[] = {PLAYER_TOP_NEUTRAL, PLAYER_LEGS_WALK_R};
inline const char *const kPlayerWalk2[] = {PLAYER_TOP_NEUTRAL, PLAYER_LEGS_IDLE};
inline const char *const kPlayerWalk3[] = {PLAYER_TOP_NEUTRAL, PLAYER_LEGS_WALK_L};
inline const char *const kPlayerJump[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    "..KSSSSSSK..",
    "..KSESSESK..",
    "..KSSSSSSK..",
    "...SSSSSS...",
    "....SSSS....",
    ".SCCCCCCCCS.",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "..CCCCCCCC..",
    "...CCCCCC...",
    "...CC..CC...",
    "...CC..CC...",
    "...BB..BB...",
    "...BB..BB...",
    "....BBBB....",
    "............",
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
    "...........D..",
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
    std::array<sf::Texture, 4> playerWalk;
    sf::Texture playerIdle;
    sf::Texture playerJump;
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
    s.playerIdle = core::makeSprite(kPlayerIdle, kPlayerW, kPlayerH,
                                    kPlayerPal, kPlayerPalCount);
    s.playerWalk[0] = core::makeSprite(kPlayerWalk0, kPlayerW, kPlayerH,
                                       kPlayerPal, kPlayerPalCount);
    s.playerWalk[1] = core::makeSprite(kPlayerWalk1, kPlayerW, kPlayerH,
                                       kPlayerPal, kPlayerPalCount);
    s.playerWalk[2] = core::makeSprite(kPlayerWalk2, kPlayerW, kPlayerH,
                                       kPlayerPal, kPlayerPalCount);
    s.playerWalk[3] = core::makeSprite(kPlayerWalk3, kPlayerW, kPlayerH,
                                       kPlayerPal, kPlayerPalCount);
    s.playerJump = core::makeSprite(kPlayerJump, kPlayerW, kPlayerH,
                                    kPlayerPal, kPlayerPalCount);
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
