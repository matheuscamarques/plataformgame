#pragma once
#include <cmath>

#include <SFML/Graphics/Texture.hpp>

#include "assets/Sprites.h"
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
                                                  bool attackingThrow,
                                                  int walkFrame) {
    using support::SpriteFrameId;
    if (hurt) return SpriteFrameId::PlayerHurt;
    if (attackingMelee) return SpriteFrameId::PlayerPunch;
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
