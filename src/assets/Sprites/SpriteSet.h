/**
 * @file src/assets/Sprites/SpriteSet.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Agrega todas as texturas SFML do jogo e as constrói.
 * @details Define struct SpriteSet com texturas de jogador (partes),
 * inimigos, equipamentos por material e TNT mais função build que
 * converte ASCII em texturas no boot via Game. Fase E3: frames
 * monolíticos do player mortos (fonte única: partes).
 */

#pragma once
#include <cstddef>

#include "core/Material.h"
#include "core/sprite_from_ascii.h"

#include "assets/Sprites/PlayerSprites.h"
#include "assets/Sprites/PlayerParts.h"
#include "assets/Sprites/EnemySprites.h"
#include "assets/Sprites/EquipSprites.h"
#include "assets/Sprites/ThrowableSprites.h"

namespace sprites {
struct SpriteSet {
    // Partes do player (fonte única): 10 poses × head/torso/legs/feet/arms.
    // Arms = overlay transparente (só pele G/H) sobre o torso.
    struct PlayerPartsTex {
        sf::Texture head;
        sf::Texture torso;
        sf::Texture legs;
        sf::Texture feet;
        sf::Texture arms;
    };
    PlayerPartsTex playerParts[kPlayerPoseCount];
    sf::Texture slimeIdle;
    sf::Texture slimeSquash;
    sf::Texture dwarfIdle;
    sf::Texture dwarfWalkA;
    sf::Texture dwarfWalkB;
    sf::Texture dwarfThrow;
    sf::Texture dwarfMelee;
    sf::Texture skeletonIdle;
    sf::Texture skeletonWalkA;
    sf::Texture skeletonWalkB;
    sf::Texture skeletonMelee;
    sf::Texture hollowIdle;
    sf::Texture hollowWalkB;
    sf::Texture ratIdle;
    sf::Texture ratSquash;
    sf::Texture burstIdle;
    sf::Texture burstWalkB;
    sf::Texture impIdle;
    sf::Texture impWalkB;
    sf::Texture elementalIdle;
    sf::Texture elementalWalkB;
    sf::Texture undeadIdle;
    sf::Texture undeadWalkB;
    sf::Texture harpyIdle;
    sf::Texture harpyWalkA;
    sf::Texture harpyWalkB;
    sf::Texture eyeIdle;
    sf::Texture eyeWalkB;

    // Equipment — 1 textura por (peça × material).
    static constexpr int kMats = static_cast<int>(core::MaterialId::COUNT);
    sf::Texture swordIdle[kMats];
    sf::Texture swordWindup[kMats];
    sf::Texture swordSwing[kMats];
    sf::Texture axeIdle[kMats];
    sf::Texture staffIdle[kMats];
    sf::Texture bellIdle[kMats];
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
    // Partes: mesma arte dos frames, fatiada (compose() prova igualdade).
    for (int i = 0; i < kPlayerPoseCount; ++i) {
        const assets::Part* pp = poseParts(static_cast<PlayerPose>(i));
        s.playerParts[i].head = core::makeSprite(
            pp[0].rows, pp[0].w, pp[0].h, kPlayerPal, kPlayerPalCount);
        s.playerParts[i].torso = core::makeSprite(
            pp[1].rows, pp[1].w, pp[1].h, kPlayerPal, kPlayerPalCount);
        s.playerParts[i].legs = core::makeSprite(
            pp[2].rows, pp[2].w, pp[2].h, kPlayerPal, kPlayerPalCount);
        s.playerParts[i].feet = core::makeSprite(
            pp[3].rows, pp[3].w, pp[3].h, kPlayerPal, kPlayerPalCount);
        s.playerParts[i].arms = core::makeSprite(
            pp[4].rows, pp[4].w, pp[4].h, kPlayerPal, kPlayerPalCount);
    }
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
    s.skeletonIdle = core::makeSprite(kSkeletonIdle, kSkeletonW, kSkeletonH,
                                      kSkeletonPal, kSkeletonPalCount);
    s.skeletonWalkA = core::makeSprite(kSkeletonWalkA, kSkeletonW, kSkeletonH,
                                       kSkeletonPal, kSkeletonPalCount);
    s.skeletonWalkB = core::makeSprite(kSkeletonWalkB, kSkeletonW, kSkeletonH,
                                       kSkeletonPal, kSkeletonPalCount);
    s.skeletonMelee = core::makeSprite(kSkeletonMelee, kSkeletonW, kSkeletonH,
                                       kSkeletonPal, kSkeletonPalCount);
    s.hollowIdle = core::makeSprite(kHollowIdle, 14, 18,
                                       kHollowPal, kHollowPalCount);
    s.hollowWalkB = core::makeSprite(kHollowWalkB, 14, 18,
                                       kHollowPal, kHollowPalCount);
    s.ratIdle = core::makeSprite(kRatIdle, 14, 12,
                                       kRatPal, kRatPalCount);
    s.ratSquash = core::makeSprite(kRatSquash, 14, 12,
                                       kRatPal, kRatPalCount);
    s.burstIdle = core::makeSprite(kBurstIdle, 14, 18,
                                       kBurstPal, kBurstPalCount);
    s.burstWalkB = core::makeSprite(kBurstWalkB, 14, 18,
                                       kBurstPal, kBurstPalCount);
    s.impIdle = core::makeSprite(kImpIdle, 14, 18,
                                       kImpPal, kImpPalCount);
    s.impWalkB = core::makeSprite(kImpWalkB, 14, 18,
                                       kImpPal, kImpPalCount);
    s.elementalIdle = core::makeSprite(kElementalIdle, 14, 18,
                                       kElementalPal, kElementalPalCount);
    s.elementalWalkB = core::makeSprite(kElementalWalkB, 14, 18,
                                       kElementalPal, kElementalPalCount);
    s.undeadIdle = core::makeSprite(kUndeadIdle, 14, 18,
                                       kUndeadPal, kUndeadPalCount);
    s.undeadWalkB = core::makeSprite(kUndeadWalkB, 14, 18,
                                       kUndeadPal, kUndeadPalCount);
    s.harpyIdle = core::makeSprite(kHarpyIdle, 14, 12,
                                       kHarpyPal, kHarpyPalCount);
    s.harpyWalkA = core::makeSprite(kHarpyWalkA, 14, 12,
                                       kHarpyPal, kHarpyPalCount);
    s.harpyWalkB = core::makeSprite(kHarpyWalkB, 14, 12,
                                       kHarpyPal, kHarpyPalCount);
    s.eyeIdle = core::makeSprite(kEyeIdle, 14, 12,
                                       kEyePal, kEyePalCount);
    s.eyeWalkB = core::makeSprite(kEyeWalkB, 14, 12,
                                       kEyePal, kEyePalCount);

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
        s.staffIdle[m] = core::makeSprite(kStaffIdle, kStaffW, kStaffH, pal, 5);
        s.bellIdle[m] = core::makeSprite(kBellIdle, kStaffW, kStaffH, pal, 5);
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
