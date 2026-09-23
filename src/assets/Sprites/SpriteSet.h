/**
 * @file src/assets/Sprites/SpriteSet.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Agrega todas as texturas SFML do jogo e as constrói.
 * @details Define struct SpriteSet com texturas de jogador, inimigos, equipamentos por material e TNT mais função build que converte ASCII em texturas no boot via Game.
 */

#pragma once
#include <cstddef>

#include "core/Material.h"
#include "core/sprite_from_ascii.h"

#include "assets/Sprites/PlayerSprites.h"
#include "assets/Sprites/EnemySprites.h"
#include "assets/Sprites/EquipSprites.h"
#include "assets/Sprites/ThrowableSprites.h"

namespace sprites {
struct SpriteSet {
    sf::Texture playerIdle;
    sf::Texture playerWalkA;
    sf::Texture playerWalkB;
    sf::Texture playerJump;
    sf::Texture playerThrow;
    sf::Texture playerPunch;
    sf::Texture playerPunchUp;
    sf::Texture playerPunchDown;
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
    sf::Texture axeIdle[kMats];
    sf::Texture helm[kMats];
    sf::Texture chest[kMats];
    sf::Texture legs[kMats];
    sf::Texture boots[kMats];
    sf::Texture gloves[kMats];

    // Throwables — 1 textura por frame (fresh/burning/critical), sem material.
    sf::Texture tnt[3];
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
    s.playerPunchUp = P(kPlayerPunchUp);
    s.playerPunchDown = P(kPlayerPunchDown);
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
        s.axeIdle[m] = core::makeSprite(kIronAxeIdle, kSwordW, kSwordH, pal, 5);
        s.helm[m] = core::makeSprite(kIronHelmIdle, kHelmW, kHelmH, pal, 5);
        s.chest[m] = core::makeSprite(kIronChestIdle, kChestW, kChestH, pal, 5);
        s.legs[m] = core::makeSprite(kIronLegsIdle, kLegsW, kLegsH, pal, 5);
        s.boots[m] = core::makeSprite(kIronBootsIdle, kBootsW, kBootsH, pal, 5);
        s.gloves[m] = core::makeSprite(kIronGlovesIdle, kGloveW, kGloveH, pal, 5);
    }
    s.tnt[0] = core::makeSprite(kTntFresh, kTntW, kTntH, kTntPal, kTntPalCount);
    s.tnt[1] = core::makeSprite(kTntBurning, kTntW, kTntH, kTntPal, kTntPalCount);
    s.tnt[2] = core::makeSprite(kTntCritical, kTntW, kTntH, kTntPal, kTntPalCount);
    return s;
}

} // namespace sprites
