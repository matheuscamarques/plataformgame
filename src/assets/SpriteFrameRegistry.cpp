/**
 * @file src/assets/SpriteFrameRegistry.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Mapeia cada SpriteFrameId para seus dados ASCII e paleta.
 * @details Implementa frameData com switch que retorna rows, dimensões e paleta de Sprites, usado pelo Renderer e BodyDump via SpriteFrameRegistry.h.
 */

#include "SpriteFrameRegistry.h"

#include <string>
#include <vector>

#include "assets/PlayerSprite.h"
#include "assets/SpriteComposer.h"
#include "assets/Sprites/PlayerParts.h"
#include "assets/Sprites/SpriteSet.h"

namespace assets {

namespace {

// Cache composto por pose (Fase E1): frameData do player serve daqui,
// byte-idêntico ao monolítico (test_sprite_compose prova a igualdade).
// vector<string> é o dono; rows aponta p/ c_str estáveis (pós-fill).
struct ComposedFrame {
    std::vector<std::string> text;
    std::vector<const char*> rows;
};

const ComposedFrame& composedFor(sprites::PlayerPose pose) {
    static ComposedFrame cache[sprites::kPlayerPoseCount];
    static bool built = false;
    if (!built) {
        for (int i = 0; i < sprites::kPlayerPoseCount; ++i) {
            const Part* pp = sprites::poseParts(
                static_cast<sprites::PlayerPose>(i));
            ComposedFrame& c = cache[i];
            c.text = compose(pp, 4, sprites::kPlayerW, sprites::kPlayerH);
            c.rows.reserve(c.text.size());
            for (const auto& s : c.text) c.rows.push_back(s.c_str());
        }
        built = true;
    }
    return cache[static_cast<int>(pose)];
}

const char* const* composedRows(support::SpriteFrameId id) {
    return composedFor(game::poseForFrameId(id)).rows.data();
}

} // namespace

SpriteFrameData frameData(support::SpriteFrameId id) {
    using support::SpriteFrameId;
    switch (id) {
        case SpriteFrameId::PlayerIdle:
        case SpriteFrameId::PlayerWalkA:
        case SpriteFrameId::PlayerWalkB:
        case SpriteFrameId::PlayerJump:
        case SpriteFrameId::PlayerThrow:
        case SpriteFrameId::PlayerPunch:
        case SpriteFrameId::PlayerPunchUp:
        case SpriteFrameId::PlayerPunchDown:
        case SpriteFrameId::PlayerHurt:
        case SpriteFrameId::PlayerDeath:
            return {composedRows(id), sprites::kPlayerW, sprites::kPlayerH,
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
        case SpriteFrameId::SkeletonIdle:
            return {sprites::kSkeletonIdle, sprites::kSkeletonW,
                    sprites::kSkeletonH, sprites::kSkeletonPal,
                    sprites::kSkeletonPalCount};
        case SpriteFrameId::SkeletonWalkA:
            return {sprites::kSkeletonWalkA, sprites::kSkeletonW,
                    sprites::kSkeletonH, sprites::kSkeletonPal,
                    sprites::kSkeletonPalCount};
        case SpriteFrameId::SkeletonWalkB:
            return {sprites::kSkeletonWalkB, sprites::kSkeletonW,
                    sprites::kSkeletonH, sprites::kSkeletonPal,
                    sprites::kSkeletonPalCount};
        case SpriteFrameId::SkeletonMelee:
            return {sprites::kSkeletonMelee, sprites::kSkeletonW,
                    sprites::kSkeletonH, sprites::kSkeletonPal,
                    sprites::kSkeletonPalCount};
        case SpriteFrameId::None:
        case SpriteFrameId::COUNT:
        default:
            return {};
    }
}

} // namespace assets
