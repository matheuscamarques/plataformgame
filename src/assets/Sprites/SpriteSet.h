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
#include <array>
#include <cstddef>

#include "core/Material.h"
#include "core/TarotSprites.h"
#include "core/sprite_from_ascii.h"

#include "assets/SpriteFrameRegistry.h"
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
    sf::Texture dwarfWalkC;
    sf::Texture dwarfWalkD;
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
    sf::Texture insectIdle;
    sf::Texture serpentIdle;
    sf::Texture specterIdle;
    sf::Texture constructIdle;
    sf::Texture pureElementalIdle;

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

    // Tarô — 38 texturas únicas indexadas por uniqueIndexOf()
    // (22 Maiores + 16 Menores; 40 Menores compartilham por rank).
    std::array<sf::Texture, core::tarot_sprites::kUniqueSpriteCount>
        tarotTex;
};

// Frame de inimigo → membro de textura (item 3 da faxina): o loop do
// build() lê rows/dims/pal da tabela do registry (fonte única).
struct EnemyTexTarget {
    support::SpriteFrameId id;
    sf::Texture SpriteSet::* tex;
};

inline const EnemyTexTarget* enemyTexTargets(int* outCount) {
    static const EnemyTexTarget kTargets[] = {
        {support::SpriteFrameId::SlimeIdle, &SpriteSet::slimeIdle},
        {support::SpriteFrameId::SlimeSquash, &SpriteSet::slimeSquash},
        {support::SpriteFrameId::DwarfIdle, &SpriteSet::dwarfIdle},
        {support::SpriteFrameId::DwarfWalkA, &SpriteSet::dwarfWalkA},
        {support::SpriteFrameId::DwarfWalkB, &SpriteSet::dwarfWalkB},
        {support::SpriteFrameId::DwarfWalkC, &SpriteSet::dwarfWalkC},
        {support::SpriteFrameId::DwarfWalkD, &SpriteSet::dwarfWalkD},
        {support::SpriteFrameId::DwarfThrow, &SpriteSet::dwarfThrow},
        {support::SpriteFrameId::DwarfMelee, &SpriteSet::dwarfMelee},
        {support::SpriteFrameId::SkeletonIdle, &SpriteSet::skeletonIdle},
        {support::SpriteFrameId::SkeletonWalkA, &SpriteSet::skeletonWalkA},
        {support::SpriteFrameId::SkeletonWalkB, &SpriteSet::skeletonWalkB},
        {support::SpriteFrameId::SkeletonMelee, &SpriteSet::skeletonMelee},
        {support::SpriteFrameId::HollowIdle, &SpriteSet::hollowIdle},
        {support::SpriteFrameId::HollowWalkB, &SpriteSet::hollowWalkB},
        {support::SpriteFrameId::RatIdle, &SpriteSet::ratIdle},
        {support::SpriteFrameId::RatSquash, &SpriteSet::ratSquash},
        {support::SpriteFrameId::BurstIdle, &SpriteSet::burstIdle},
        {support::SpriteFrameId::BurstWalkB, &SpriteSet::burstWalkB},
        {support::SpriteFrameId::ImpIdle, &SpriteSet::impIdle},
        {support::SpriteFrameId::ImpWalkB, &SpriteSet::impWalkB},
        {support::SpriteFrameId::ElementalIdle, &SpriteSet::elementalIdle},
        {support::SpriteFrameId::ElementalWalkB, &SpriteSet::elementalWalkB},
        {support::SpriteFrameId::UndeadIdle, &SpriteSet::undeadIdle},
        {support::SpriteFrameId::UndeadWalkB, &SpriteSet::undeadWalkB},
        {support::SpriteFrameId::HarpyIdle, &SpriteSet::harpyIdle},
        {support::SpriteFrameId::HarpyWalkA, &SpriteSet::harpyWalkA},
        {support::SpriteFrameId::HarpyWalkB, &SpriteSet::harpyWalkB},
        {support::SpriteFrameId::EyeIdle, &SpriteSet::eyeIdle},
        {support::SpriteFrameId::EyeWalkB, &SpriteSet::eyeWalkB},
        {support::SpriteFrameId::InsectIdle, &SpriteSet::insectIdle},
        {support::SpriteFrameId::SerpentIdle, &SpriteSet::serpentIdle},
        {support::SpriteFrameId::SpecterIdle, &SpriteSet::specterIdle},
        {support::SpriteFrameId::ConstructIdle, &SpriteSet::constructIdle},
        {support::SpriteFrameId::PureElementalIdle,
         &SpriteSet::pureElementalIdle},
    };
    if (outCount)
        *outCount = static_cast<int>(sizeof(kTargets) /
                                    sizeof(kTargets[0]));
    return kTargets;
}

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
    // Inimigos: rows/dims/pal vêm da tabela do registry (fonte única);
    // aqui só o destino (membro). Id sem frame = textura vazia, sem crash.
    {
        int nn = 0, mm = 0;
        const assets::StaticFrameEntry* ft = assets::staticFrameTable(&nn);
        const EnemyTexTarget* tt = enemyTexTargets(&mm);
        for (int i = 0; i < mm; ++i) {
            const assets::StaticFrameEntry* e = nullptr;
            for (int j = 0; j < nn; ++j) {
                if (ft[j].id == tt[i].id) {
                    e = &ft[j];
                    break;
                }
            }
            if (!e) continue;
            s.*(tt[i].tex) =
                core::makeSprite(e->rows, e->w, e->h, e->pal, e->palCount);
        }
    }

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
        // Catalisadores: paleta fixa (não material), igual em todo m.
        s.staffIdle[m] = core::makeSprite(kStaffIdle, kStaffW, kStaffH,
                                          kStaffFixedPal, kStaffFixedPalCount);
        s.bellIdle[m] = core::makeSprite(kBellIdle, kStaffW, kStaffH,
                                         kBellFixedPal, kBellFixedPalCount);
        s.helm[m] = core::makeSprite(kIronHelmIdle, kHelmW, kHelmH, pal, 5);
        s.chest[m] = core::makeSprite(kIronChestIdle, kChestW, kChestH, pal, 5);
        s.legs[m] = core::makeSprite(kIronLegsIdle, kLegsW, kLegsH, pal, 5);
        s.boots[m] = core::makeSprite(kIronBootsIdle, kBootsW, kBootsH, pal, 5);
        s.gloves[m] = core::makeSprite(kIronGlovesIdle, kGloveW, kGloveH, pal, 5);
    }
    s.tnt[0] = core::makeSprite(kTntFresh, kTntW, kTntH, kTntPal, kTntPalCount);
    s.tnt[1] = core::makeSprite(kTntBurning, kTntW, kTntH, kTntPal, kTntPalCount);
    s.tnt[2] = core::makeSprite(kTntCritical, kTntW, kTntH, kTntPal, kTntPalCount);
    // Tarô: 38 texturas (makeSprite aceita palCount 24, sem teto).
    for (int u = 0; u < core::tarot_sprites::kUniqueSpriteCount; ++u) {
        const auto ref = core::tarot_sprites::uniqueSprite(u);
        const auto pal = core::toLegacyPal(ref.pal, ref.palCount);
        s.tarotTex[u] = core::makeSprite(ref.rows, ref.w, ref.h,
                                         pal.data(), pal.size());
    }
    return s;
}

} // namespace sprites
