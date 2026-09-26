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

} // namespace game
