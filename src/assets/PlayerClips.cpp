/**
 * @file src/assets/PlayerClips.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define os clips de ataque do player (Fase C).
 * @details Único TU com as tabelas; header só declara. Frame 0 =
 * Windup, 1 = Active (Hitbox viva + Sfx de swoosh na borda), 2 =
 * Recovery.
 */

#include "assets/PlayerClips.h"

namespace game {

namespace {

using support::AnimKeyframe;
using support::SpriteFrameId;

constexpr AnimKeyframe kSideFrames[3] = {
    {SpriteFrameId::PlayerPunch, 0.f, 0},
    {SpriteFrameId::PlayerPunch, 0.f, support::AnimEvent::Hitbox | support::AnimEvent::Sfx},
    {SpriteFrameId::PlayerPunch, 0.f, 0},
};
constexpr AnimKeyframe kUpFrames[3] = {
    {SpriteFrameId::PlayerPunchUp, 0.f, 0},
    {SpriteFrameId::PlayerPunchUp, 0.f, support::AnimEvent::Hitbox | support::AnimEvent::Sfx},
    {SpriteFrameId::PlayerPunchUp, 0.f, 0},
};
constexpr AnimKeyframe kDownFrames[3] = {
    {SpriteFrameId::PlayerPunchDown, 0.f, 0},
    {SpriteFrameId::PlayerPunchDown, 0.f, support::AnimEvent::Hitbox | support::AnimEvent::Sfx},
    {SpriteFrameId::PlayerPunchDown, 0.f, 0},
};

} // namespace

const support::AnimClip &attackClipSide() {
    static constexpr support::AnimClip k{"attack_side", kSideFrames, 3,
                                         false};
    return k;
}

const support::AnimClip &attackClipUp() {
    static constexpr support::AnimClip k{"attack_up", kUpFrames, 3, false};
    return k;
}

const support::AnimClip &attackClipDown() {
    static constexpr support::AnimClip k{"attack_down", kDownFrames, 3,
                                         false};
    return k;
}

const support::AnimClip &attackClipFor(support::AimDir aim) {
    using support::AimDir;
    switch (aim) {
        case AimDir::N:
        case AimDir::NE:
        case AimDir::NW: return attackClipUp();
        case AimDir::S:
        case AimDir::SE:
        case AimDir::SW: return attackClipDown();
        default: return attackClipSide();
    }
}

namespace {

constexpr AnimKeyframe kIdleFrames[1] = {
    {SpriteFrameId::PlayerIdle, 0.25f, 0},
};
// A/C = contact (pé bate: poeira), B/D = passing (pé no ar).
constexpr AnimKeyframe kWalkFrames[4] = {
    {SpriteFrameId::PlayerWalkA, 0.10f, support::AnimEvent::Step},
    {SpriteFrameId::PlayerWalkB, 0.10f, 0},
    {SpriteFrameId::PlayerWalkC, 0.10f, support::AnimEvent::Step},
    {SpriteFrameId::PlayerWalkD, 0.10f, 0},
};

} // namespace

const support::AnimClip &idleClip() {
    static constexpr support::AnimClip k{"idle", kIdleFrames, 1, true};
    return k;
}

const support::AnimClip &walkClip() {
    static constexpr support::AnimClip k{"walk", kWalkFrames, 4, true};
    return k;
}

} // namespace game
