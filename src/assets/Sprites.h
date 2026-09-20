#pragma once
#include <cstddef>

#include "core/Material.h"
#include "core/sprite_from_ascii.h"

namespace sprites {

// Player 12x20, slime 14x12, dwarf 14x18. Top compartilhado via macros;
// teste trava widths (linha errada = sprite deslocada).

inline constexpr int kPlayerW = 12;
inline constexpr int kPlayerH = 20;

inline const core::PaletteEntry kPlayerPal[] = {
    {'.', {0, 0, 0, 0}, core::BodyPartId::None},
    {'K', {30, 20, 20}, core::BodyPartId::None},
    {'S', {230, 180, 140}, core::BodyPartId::ArmR},
    {'E', {20, 15, 15}, core::BodyPartId::Head},
    {'C', {60, 90, 160}, core::BodyPartId::Torso},
    {'B', {50, 35, 30}, core::BodyPartId::LegR},
    {'W', {190, 190, 200}, core::BodyPartId::Weapon},
    {'T', {220, 60, 50}, core::BodyPartId::Weapon},
    {'t', {90, 30, 25}, core::BodyPartId::Weapon},
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
    {'S', {230, 180, 140}, core::BodyPartId::ArmR},
    {'E', {20, 20, 20}, core::BodyPartId::Head},
    {'H', {140, 90, 60}, core::BodyPartId::Head},
    {'C', {120, 130, 150}, core::BodyPartId::Head},
    {'T', {90, 60, 40}, core::BodyPartId::Torso},
    {'B', {50, 35, 30}, core::BodyPartId::LegR},
    {'W', {180, 180, 190}, core::BodyPartId::Weapon},
    {'D', {220, 60, 50}, core::BodyPartId::Weapon},
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

// ============================================================
// EQUIPAMENTO — forma compartilhada, 1 textura por material.
// Espada 8x20 (idle/windup), 16x8 (swing); elmo 12x5; peitoral 12x8;
// perneiras 12x6. Paleta de 5 entradas (., W, w, G, E) por material.
inline constexpr int kSwordW = 8;
inline constexpr int kSwordH = 20;
inline constexpr int kSwordSwingW = 16;
inline constexpr int kSwordSwingH = 8;
inline constexpr int kHelmW = 12;
inline constexpr int kHelmH = 5;
inline constexpr int kChestW = 12;
inline constexpr int kChestH = 8;
inline constexpr int kLegsW = 12;
inline constexpr int kLegsH = 6;

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
    "GGGGGGGGGGGG",
    "GwwwwwwwwwwG",
    "GwwwwwwwwwwG",
    "Gw..EE..EEwG",
    "Gw........wG",
};

inline const char *const kIronChestIdle[] = {
    "wwwwwwwwwwww",
    "wWWWWWWWWWWw",
    "wWWWWWWWWWWw",
    "wWWGGGGGGWWw",
    "wWWGGGGGGWWw",
    "wWWWWWWWWWWw",
    "wwwwwwwwwwww",
    ".wwwwwwwwww.",
};

inline const char *const kIronLegsIdle[] = {
    "..wwww.wwww.",
    "..wWWw.wWWw.",
    "..wWWw.wWWw.",
    "..wWWw.wWWw.",
    "..wGGw.wGGw.",
    "..wwww.wwww.",
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

    // Equipment — 1 textura por (peça × material).
    static constexpr int kMats = static_cast<int>(core::MaterialId::COUNT);
    sf::Texture swordIdle[kMats];
    sf::Texture swordWindup[kMats];
    sf::Texture swordSwing[kMats];
    sf::Texture helm[kMats];
    sf::Texture chest[kMats];
    sf::Texture legs[kMats];
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

    // Paleta de equipamento por material: 5 entradas fixas (., W, w, G, E).
    for (int m = 0; m < SpriteSet::kMats; ++m) {
        const auto &c = core::materialColors(static_cast<core::MaterialId>(m));
        core::PaletteEntry pal[5] = {
            {'.', {0, 0, 0, 0}},
            {'W', c.main},
            {'w', c.dark},
            {'G', c.accent},
            {'E', {20, 15, 15}},
        };
        s.swordIdle[m] = core::makeSprite(kIronSwordIdle, kSwordW, kSwordH, pal, 5);
        s.swordWindup[m] = core::makeSprite(kIronSwordWindup, kSwordW, kSwordH, pal, 5);
        s.swordSwing[m] = core::makeSprite(kIronSwordSwing, kSwordSwingW, kSwordSwingH, pal, 5);
        s.helm[m] = core::makeSprite(kIronHelmIdle, kHelmW, kHelmH, pal, 5);
        s.chest[m] = core::makeSprite(kIronChestIdle, kChestW, kChestH, pal, 5);
        s.legs[m] = core::makeSprite(kIronLegsIdle, kLegsW, kLegsH, pal, 5);
    }
    return s;
}

} // namespace sprites
