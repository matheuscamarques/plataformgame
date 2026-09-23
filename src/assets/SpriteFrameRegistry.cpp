/**
 * @file src/assets/SpriteFrameRegistry.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Mapeia cada SpriteFrameId para seus dados ASCII e paleta.
 * @details Implementa frameData com switch que retorna rows, dimensões e paleta de Sprites, usado pelo Renderer e BodyDump via SpriteFrameRegistry.h.
 */

#include "SpriteFrameRegistry.h"

#include "assets/Sprites/SpriteSet.h"

namespace assets {

SpriteFrameData frameData(support::SpriteFrameId id) {
    using support::SpriteFrameId;
    switch (id) {
        case SpriteFrameId::PlayerIdle:
            return {sprites::kPlayerIdle, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerWalkA:
            return {sprites::kPlayerWalkA, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerWalkB:
            return {sprites::kPlayerWalkB, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerJump:
            return {sprites::kPlayerJump, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerThrow:
            return {sprites::kPlayerThrow, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerPunch:
            return {sprites::kPlayerPunch, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerPunchUp:
            return {sprites::kPlayerPunchUp, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerPunchDown:
            return {sprites::kPlayerPunchDown, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerHurt:
            return {sprites::kPlayerHurt, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::PlayerDeath:
            return {sprites::kPlayerDeath, sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPal, sprites::kPlayerPalCount};
        case SpriteFrameId::SlimeIdle:
            return {sprites::kSlimeIdle, sprites::kSlimeW, sprites::kSlimeH,
                    sprites::kSlimePal, sprites::kSlimePalCount};
        case SpriteFrameId::SlimeSquash:
            return {sprites::kSlimeSquash, sprites::kSlimeW, sprites::kSlimeH,
                    sprites::kSlimePal, sprites::kSlimePalCount};
        case SpriteFrameId::DwarfIdle:
            return {sprites::kDwarfIdle, sprites::kDwarfW, sprites::kDwarfH,
                    sprites::kDwarfPal, sprites::kDwarfPalCount};
        case SpriteFrameId::DwarfWalkA:
            return {sprites::kDwarfWalkA, sprites::kDwarfW, sprites::kDwarfH,
                    sprites::kDwarfPal, sprites::kDwarfPalCount};
        case SpriteFrameId::DwarfWalkB:
            return {sprites::kDwarfWalkB, sprites::kDwarfW, sprites::kDwarfH,
                    sprites::kDwarfPal, sprites::kDwarfPalCount};
        case SpriteFrameId::DwarfThrow:
            return {sprites::kDwarfThrow, sprites::kDwarfW, sprites::kDwarfH,
                    sprites::kDwarfPal, sprites::kDwarfPalCount};
        case SpriteFrameId::DwarfMelee:
            return {sprites::kDwarfMelee, sprites::kDwarfW, sprites::kDwarfH,
                    sprites::kDwarfPal, sprites::kDwarfPalCount};
        case SpriteFrameId::None:
        case SpriteFrameId::COUNT:
        default:
            return {};
    }
}

} // namespace assets
