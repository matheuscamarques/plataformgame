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

const ComposedFrame& composedFor(sprites::PlayerPose pose,
                                 support::Facing artDir) {
    static ComposedFrame cache[sprites::kPlayerPoseCount]
                              [sprites::kArtDirCount];
    static bool built[sprites::kPlayerPoseCount][sprites::kArtDirCount] = {};
    const int pi = static_cast<int>(pose);
    const int di = sprites::artDirIndex(artDir);
    if (!built[pi][di]) {
        const Part* pp = sprites::posePartsFor(
            static_cast<sprites::PlayerPose>(pi),
            sprites::artDirForIndex(di));
        ComposedFrame& c = cache[pi][di];
        // 5 partes: head/torso/legs/feet + arms. Arms é APENAS hitbox
        // (G/H no corpo) — o desenho é 100% procedural em Renderer.
        c.text = compose(pp, 5, sprites::kPlayerW, sprites::kPlayerH);
        c.rows.reserve(c.text.size());
        for (const auto& s : c.text) c.rows.push_back(s.c_str());
        built[pi][di] = true;
    }
    return cache[pi][di];
}

const char* const* composedRows(support::SpriteFrameId id) {
    return composedFor(game::poseForFrameId(id), support::Facing::E)
        .rows.data();
}

} // namespace

SpriteFrameData playerFrameData(sprites::PlayerPose pose,
                                 support::Facing artDir) {
    const ComposedFrame& c = composedFor(pose, artDir);
    return {c.rows.data(), sprites::kPlayerW, sprites::kPlayerH,
            sprites::kPlayerPalette, sprites::kPlayerPaletteCount};
}

SpriteFrameData playerAirAttackFrameData(sprites::PlayerPose pose) {
    const assets::Part* ap = sprites::posePartsForAir(pose);
    if (!ap)
        return playerFrameData(pose, support::Facing::E);
    static ComposedFrame cache[sprites::kPlayerPoseCount];
    static bool built[sprites::kPlayerPoseCount] = {};
    const int pi = static_cast<int>(pose);
    if (pi < 0 || pi >= sprites::kPlayerPoseCount)
        return playerFrameData(pose, support::Facing::E);
    if (!built[pi]) {
        ComposedFrame& c = cache[pi];
        c.text = compose(ap, 5, sprites::kPlayerW, sprites::kPlayerH);
        c.rows.reserve(c.text.size());
        for (const auto& s : c.text) c.rows.push_back(s.c_str());
        built[pi] = true;
    }
    const ComposedFrame& c = cache[pi];
    return {c.rows.data(), sprites::kPlayerW, sprites::kPlayerH,
            sprites::kPlayerPalette, sprites::kPlayerPaletteCount};
}

// Fonte única dos frames estáticos (item 2 da faxina): mesmos
// rows/dimensões/paleta do switch antigo, sem repetir a tripla.
// _LIT para dims literais (14x18/14x12, sem constante nomeada).
#define STATIC_FRAME(Id, Rows, W, H, Pal)                                \
    {support::SpriteFrameId::Id, sprites::Rows, sprites::W, sprites::H,   \
     sprites::Pal, sprites::Pal##Count}
#define STATIC_FRAME_LIT(Id, Rows, W, H, Pal)                            \
    {support::SpriteFrameId::Id, sprites::Rows, (W), (H), sprites::Pal,   \
     sprites::Pal##Count}

const StaticFrameEntry kStaticFrames[] = {
    STATIC_FRAME(SlimeIdle, kSlimeIdle, kSlimeW, kSlimeH, kSlimePal),
    STATIC_FRAME(SlimeSquash, kSlimeSquash, kSlimeW, kSlimeH, kSlimePal),
    STATIC_FRAME(DwarfIdle, kDwarfIdle, kDwarfW, kDwarfH, kDwarfPal),
    STATIC_FRAME(DwarfWalkA, kDwarfWalkA, kDwarfW, kDwarfH, kDwarfPal),
    STATIC_FRAME(DwarfWalkB, kDwarfWalkB, kDwarfW, kDwarfH, kDwarfPal),
    STATIC_FRAME(DwarfWalkC, kDwarfWalkC, kDwarfW, kDwarfH, kDwarfPal),
    STATIC_FRAME(DwarfWalkD, kDwarfWalkD, kDwarfW, kDwarfH, kDwarfPal),
    STATIC_FRAME(DwarfThrow, kDwarfThrow, kDwarfW, kDwarfH, kDwarfPal),
    STATIC_FRAME(DwarfMelee, kDwarfMelee, kDwarfW, kDwarfH, kDwarfPal),
    STATIC_FRAME(SkeletonIdle, kSkeletonIdle, kSkeletonW, kSkeletonH,
                 kSkeletonPal),
    STATIC_FRAME(SkeletonWalkA, kSkeletonWalkA, kSkeletonW, kSkeletonH,
                 kSkeletonPal),
    STATIC_FRAME(SkeletonWalkB, kSkeletonWalkB, kSkeletonW, kSkeletonH,
                 kSkeletonPal),
    STATIC_FRAME(SkeletonMelee, kSkeletonMelee, kSkeletonW, kSkeletonH,
                 kSkeletonPal),
    STATIC_FRAME_LIT(HollowIdle, kHollowIdle, 14, 18, kHollowPal),
    STATIC_FRAME_LIT(HollowWalkB, kHollowWalkB, 14, 18, kHollowPal),
    STATIC_FRAME_LIT(RatIdle, kRatIdle, 14, 12, kRatPal),
    STATIC_FRAME_LIT(RatSquash, kRatSquash, 14, 12, kRatPal),
    STATIC_FRAME_LIT(BurstIdle, kBurstIdle, 14, 18, kBurstPal),
    STATIC_FRAME_LIT(BurstWalkB, kBurstWalkB, 14, 18, kBurstPal),
    STATIC_FRAME_LIT(ImpIdle, kImpIdle, 14, 18, kImpPal),
    STATIC_FRAME_LIT(ImpWalkB, kImpWalkB, 14, 18, kImpPal),
    STATIC_FRAME_LIT(ElementalIdle, kElementalIdle, 14, 18, kElementalPal),
    STATIC_FRAME_LIT(ElementalWalkB, kElementalWalkB, 14, 18, kElementalPal),
    STATIC_FRAME_LIT(UndeadIdle, kUndeadIdle, 14, 18, kUndeadPal),
    STATIC_FRAME_LIT(UndeadWalkB, kUndeadWalkB, 14, 18, kUndeadPal),
    STATIC_FRAME_LIT(HarpyIdle, kHarpyIdle, 14, 12, kHarpyPal),
    STATIC_FRAME_LIT(HarpyWalkA, kHarpyWalkA, 14, 12, kHarpyPal),
    STATIC_FRAME_LIT(HarpyWalkB, kHarpyWalkB, 14, 12, kHarpyPal),
    STATIC_FRAME_LIT(EyeIdle, kEyeIdle, 14, 12, kEyePal),
    STATIC_FRAME_LIT(EyeWalkB, kEyeWalkB, 14, 12, kEyePal),
    STATIC_FRAME(InsectIdle, kInsectIdle, kInsectW, kInsectH, kInsectPal),
    STATIC_FRAME(SerpentIdle, kSerpentIdle, kSerpentW, kSerpentH,
                 kSerpentPal),
    STATIC_FRAME(SpecterIdle, kSpecterIdle, kSpecterW, kSpecterH,
                 kSpecterPal),
    STATIC_FRAME(ConstructIdle, kConstructIdle, kConstructW, kConstructH,
                 kConstructPal),
    STATIC_FRAME(PureElementalIdle, kPureElementalIdle, kPureElementalW,
                 kPureElementalH, kPureElementalPal),
};

#undef STATIC_FRAME
#undef STATIC_FRAME_LIT

const StaticFrameEntry* staticFrameTable(int* outCount) {
    if (outCount)
        *outCount = static_cast<int>(sizeof(kStaticFrames) /
                                    sizeof(kStaticFrames[0]));
    return kStaticFrames;
}

SpriteFrameData frameData(support::SpriteFrameId id) {
    using support::SpriteFrameId;
    switch (id) {
        case SpriteFrameId::PlayerIdle:
        case SpriteFrameId::PlayerWalkA:
        case SpriteFrameId::PlayerWalkB:
        case SpriteFrameId::PlayerWalkC:
        case SpriteFrameId::PlayerWalkD:
        case SpriteFrameId::PlayerJump:
        case SpriteFrameId::PlayerThrow:
        case SpriteFrameId::PlayerPunch:
        case SpriteFrameId::PlayerPunchUp:
        case SpriteFrameId::PlayerPunchDown:
        case SpriteFrameId::PlayerHurt:
        case SpriteFrameId::PlayerDeath:
            return {composedRows(id), sprites::kPlayerW, sprites::kPlayerH,
                    sprites::kPlayerPalette, sprites::kPlayerPaletteCount};
        default:
            break;
    }
    int n = 0;
    const StaticFrameEntry* table = staticFrameTable(&n);
    for (int i = 0; i < n; ++i) {
        if (table[i].id == id) {
            return {table[i].rows, table[i].w, table[i].h, table[i].pal,
                    table[i].palCount};
        }
    }
    return {}; // None/COUNT/desconhecido
}

} // namespace assets
