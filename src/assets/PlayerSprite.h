/**
 * @file src/assets/PlayerSprite.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Decide qual sprite do jogador exibir conforme estado atual.
 * @details Funções inline resolvePlayerSprite e textureForFrame escolhem id do frame por chão, velocidade, dano e ataque, chamadas por Game tick e Renderer sem precisar de GL.
 */

#pragma once
#include <cmath>

#include <SFML/Graphics/Texture.hpp>

#include "assets/Sprites/SpriteSet.h"
#include "assets/Sprites/PlayerParts.h"
#include "support/Combat/AimDir.h"
#include "support/Combat/Facing.h"
#include "support/Combat/SpriteFrame.h"

class Player;

// Ponto único de decisão do sprite do player (resolve) + mapa id→
// textura (textureForFrame, sem lógica). Tick preenche currentFrameId
// via resolve; render desenha via textureForFrame. Sem GL na decisão:
// testável headless. meleeTex null = fallback pro soco.
namespace game {

inline support::SpriteFrameId resolvePlayerSprite(bool onGround, float vx,
                                                   bool hurt,
                                                   bool attackingMelee,
                                                   support::AimDir attackAim,
                                                   bool attackingThrow,
                                                   int walkFrame) {
    using support::SpriteFrameId;
    using support::AimDir;
    if (hurt) return SpriteFrameId::PlayerHurt;
    // Corpo reflete a direção congelada do golpe (attackAim), não o
    // input vivo: sprite e hitbox leem o mesmo snapshot.
    if (attackingMelee) {
        switch (attackAim) {
            case AimDir::N:
            case AimDir::NE:
            case AimDir::NW: return SpriteFrameId::PlayerPunchUp;
            case AimDir::S:
            case AimDir::SE:
            case AimDir::SW: return SpriteFrameId::PlayerPunchDown;
            default: return SpriteFrameId::PlayerPunch;
        }
    }
    if (attackingThrow) return SpriteFrameId::PlayerThrow;
    if (!onGround) return SpriteFrameId::PlayerJump;
    if (std::fabs(vx) > 5.f) {
        return (walkFrame % 2 == 0) ? SpriteFrameId::PlayerWalkA
                                    : SpriteFrameId::PlayerWalkB;
    }
    return SpriteFrameId::PlayerIdle;
}

inline const sf::Texture *textureForFrame(support::SpriteFrameId id,
                                          const sprites::SpriteSet &sp) {
    using support::SpriteFrameId;
    // Fase E2: só inimigos têm textura única (player desenha partes).
    // meleeTex morreu com os monolíticos (App não assigna mais).
    switch (id) {
        case SpriteFrameId::SlimeIdle: return &sp.slimeIdle;
        case SpriteFrameId::SlimeSquash: return &sp.slimeSquash;
        case SpriteFrameId::DwarfIdle: return &sp.dwarfIdle;
        case SpriteFrameId::DwarfWalkA: return &sp.dwarfWalkA;
        case SpriteFrameId::DwarfWalkB: return &sp.dwarfWalkB;
        case SpriteFrameId::DwarfWalkC: return &sp.dwarfWalkC;
        case SpriteFrameId::DwarfWalkD: return &sp.dwarfWalkD;
        case SpriteFrameId::DwarfThrow: return &sp.dwarfThrow;
        case SpriteFrameId::DwarfMelee: return &sp.dwarfMelee;
        case SpriteFrameId::SkeletonIdle: return &sp.skeletonIdle;
        case SpriteFrameId::SkeletonWalkA: return &sp.skeletonWalkA;
        case SpriteFrameId::SkeletonWalkB: return &sp.skeletonWalkB;
        case SpriteFrameId::SkeletonMelee: return &sp.skeletonMelee;
        case SpriteFrameId::HollowIdle: return &sp.hollowIdle;
        case SpriteFrameId::HollowWalkB: return &sp.hollowWalkB;
        case SpriteFrameId::RatIdle: return &sp.ratIdle;
        case SpriteFrameId::RatSquash: return &sp.ratSquash;
        case SpriteFrameId::BurstIdle: return &sp.burstIdle;
        case SpriteFrameId::BurstWalkB: return &sp.burstWalkB;
        case SpriteFrameId::ImpIdle: return &sp.impIdle;
        case SpriteFrameId::ImpWalkB: return &sp.impWalkB;
        case SpriteFrameId::ElementalIdle: return &sp.elementalIdle;
        case SpriteFrameId::ElementalWalkB: return &sp.elementalWalkB;
        case SpriteFrameId::UndeadIdle: return &sp.undeadIdle;
        case SpriteFrameId::UndeadWalkB: return &sp.undeadWalkB;
        case SpriteFrameId::HarpyIdle: return &sp.harpyIdle;
        case SpriteFrameId::HarpyWalkA: return &sp.harpyWalkA;
        case SpriteFrameId::HarpyWalkB: return &sp.harpyWalkB;
        case SpriteFrameId::EyeIdle: return &sp.eyeIdle;
        case SpriteFrameId::EyeWalkB: return &sp.eyeWalkB;
        case SpriteFrameId::InsectIdle: return &sp.insectIdle;
        case SpriteFrameId::SerpentIdle: return &sp.serpentIdle;
        case SpriteFrameId::SpecterIdle: return &sp.specterIdle;
        case SpriteFrameId::ConstructIdle: return &sp.constructIdle;
        case SpriteFrameId::PureElementalIdle: return &sp.pureElementalIdle;
        case SpriteFrameId::None:
        case SpriteFrameId::COUNT:
        default: return &sp.slimeIdle;
    }
}

// Fase D: frame → pose das partes (espelha textureForFrame p/ player).
// Slime/anão/None caem em Idle (drawPlayerSprite só chama p/ player).
// Headless-testável (puro, sem GL).
inline sprites::PlayerPose poseForFrameId(support::SpriteFrameId id) {
    using support::SpriteFrameId;
    using sprites::PlayerPose;
    switch (id) {
        case SpriteFrameId::PlayerIdle:      return PlayerPose::Idle;
        case SpriteFrameId::PlayerWalkA:     return PlayerPose::WalkA;
        case SpriteFrameId::PlayerWalkB:     return PlayerPose::WalkB;
        case SpriteFrameId::PlayerJump:      return PlayerPose::Jump;
        case SpriteFrameId::PlayerThrow:     return PlayerPose::Throw;
        case SpriteFrameId::PlayerPunch:     return PlayerPose::Punch;
        case SpriteFrameId::PlayerPunchUp:   return PlayerPose::PunchUp;
        case SpriteFrameId::PlayerPunchDown: return PlayerPose::PunchDown;
        case SpriteFrameId::PlayerHurt:      return PlayerPose::Hurt;
        case SpriteFrameId::PlayerDeath:     return PlayerPose::Death;
        default:                             return PlayerPose::Idle;
    }
}

// Fase D (infra, sem arte nova): pose dirigida por facing. artDir é a
// direção COM arte desenhada (hoje sempre E: só existe side-view);
// mirror espelha NW/W/SW. Quando poses N/NE/S chegarem, artDirFor
// cresce e o Renderer escolhe a textura por (pose, artDir).
struct DirectedPose {
    sprites::PlayerPose pose; // a mesma (futuro: variante direcional)
    support::Facing artDir;   // direção com arte (= E hoje)
    bool mirror = false;      // flip horizontal
};

inline support::Facing artDirFor(sprites::PlayerPose pose,
                                 support::Facing f) {
    // Ondas 1 (Idle/Walk) e 2a (Punch) têm 5 dirs; resto cai em E.
    // baseDir dobra NW→NE, W→E, SW→SE (S/SE/E/NE/N com arte).
    switch (pose) {
        case sprites::PlayerPose::Idle:
        case sprites::PlayerPose::WalkA:
        case sprites::PlayerPose::WalkB:
        case sprites::PlayerPose::Punch: return support::baseDir(f);
        default:                         return support::Facing::E;
    }
}

inline DirectedPose directedPose(sprites::PlayerPose pose,
                                 support::Facing f) {
    return {pose, artDirFor(pose, f), support::isMirrored(f)};
}

// Marcha via clip (fecha a Fase C): o resolve arbitra combate vs
// marcha; o clip decide o frame. Fora da zona, passa direto.
inline support::SpriteFrameId applyLocoFrame(
    support::SpriteFrameId resolved, support::SpriteFrameId loco) {
    using support::SpriteFrameId;
    return (resolved == SpriteFrameId::PlayerWalkA ||
            resolved == SpriteFrameId::PlayerWalkB ||
            resolved == SpriteFrameId::PlayerIdle)
               ? loco
               : resolved;
}

// Juice procedural (Fase 5, testável): respiração, bounce e squash.
// Unidades em sprite-px (Renderer multiplica por s); Y+ = baixo.
inline float idleBobY(int tickCount, float amplitude = 1.f) {
    constexpr float kPi = 3.14159265f;
    return std::sin(static_cast<float>(tickCount) * 2.f * kPi / 30.f) *
           amplitude; // 1Hz a 30 ticks/s
}

// Marcha: frame de contato (índice ímpar do clip) afunda 1.5px.
inline float walkBobY(int clipFrame) {
    return (clipFrame % 2 == 1) ? 1.5f : 0.f;
}

struct Squash {
    float kx = 1.f, ky = 1.f;
};

// Pouso: 0.12s comprimindo 18% (1→0); fora disso, identidade.
inline Squash landSquash(float landAnimT) {
    constexpr float kDur = 0.12f; // == Player::kLandAnimDur
    if (landAnimT <= 0.f) return {};
    const float t = landAnimT >= kDur ? 1.f : landAnimT / kDur;
    return {1.f + 0.18f * t, 1.f - 0.18f * t};
}

// Queda rápida: estica (largo -10%, alto +10%).
inline Squash fallStretch(float vy) {
    if (vy > 12.f) return {0.9f, 1.1f};
    return {};
}

} // namespace game
