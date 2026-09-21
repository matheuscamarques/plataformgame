#pragma once
#include <cmath>

#include <SFML/Graphics/Texture.hpp>

#include "assets/Sprites/SpriteSet.h"
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
                                                   support::AimDir aimDir,
                                                   bool attackingThrow,
                                                   int walkFrame) {
    using support::SpriteFrameId;
    using support::AimDir;
    if (hurt) return SpriteFrameId::PlayerHurt;
    // Corpo reflete o input atual (aimDir), mesmo em Recovery: se o
    // jogador virar pra cima no meio do golpe, o braço ergue.
    if (attackingMelee) {
        switch (aimDir) {
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
                                          const sprites::SpriteSet &sp,
                                          const sf::Texture *meleeTex = nullptr) {
    using support::SpriteFrameId;
    switch (id) {
        case SpriteFrameId::PlayerIdle: return &sp.playerIdle;
        case SpriteFrameId::PlayerWalkA: return &sp.playerWalkA;
        case SpriteFrameId::PlayerWalkB: return &sp.playerWalkB;
        case SpriteFrameId::PlayerJump: return &sp.playerJump;
        case SpriteFrameId::PlayerThrow: return &sp.playerThrow;
        case SpriteFrameId::PlayerPunch:
            return meleeTex ? meleeTex : &sp.playerPunch;
        case SpriteFrameId::PlayerPunchUp: return &sp.playerPunchUp;
        case SpriteFrameId::PlayerPunchDown: return &sp.playerPunchDown;
        case SpriteFrameId::PlayerHurt: return &sp.playerHurt;
        case SpriteFrameId::PlayerDeath: return &sp.playerDeath;
        case SpriteFrameId::SlimeIdle: return &sp.slimeIdle;
        case SpriteFrameId::SlimeSquash: return &sp.slimeSquash;
        case SpriteFrameId::DwarfIdle: return &sp.dwarfIdle;
        case SpriteFrameId::DwarfWalkA: return &sp.dwarfWalkA;
        case SpriteFrameId::DwarfWalkB: return &sp.dwarfWalkB;
        case SpriteFrameId::DwarfThrow: return &sp.dwarfThrow;
        case SpriteFrameId::DwarfMelee: return &sp.dwarfMelee;
        case SpriteFrameId::None:
        case SpriteFrameId::COUNT:
        default: return &sp.playerIdle;
    }
}

} // namespace game
