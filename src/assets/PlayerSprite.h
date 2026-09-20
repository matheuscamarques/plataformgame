#pragma once
#include <cmath>

#include <SFML/Graphics/Texture.hpp>

#include "assets/Sprites.h"

class Player;

// Seleção de frame do player. Prioridade: hurt > melee > throw > jump.
// Pura (sem Player completo): testável headless — compara endereços,
// sem build() de textura. meleeTex null = fallback pro soco.
namespace game {

inline const sf::Texture *pickPlayerFrame(bool onGround, float vx,
                                          bool hurt,
                                          bool attackingMelee,
                                          const sf::Texture *meleeTex,
                                          bool attackingThrow,
                                          const sprites::SpriteSet &sp,
                                          int walkFrame) {
    if (hurt) return &sp.playerHurt;
    if (attackingMelee) return meleeTex ? meleeTex : &sp.playerPunch;
    if (attackingThrow) return &sp.playerThrow;
    if (!onGround) return &sp.playerJump;
    if (std::fabs(vx) > 5.f) {
        return (walkFrame % 2 == 0) ? &sp.playerWalkA : &sp.playerWalkB;
    }
    return &sp.playerIdle;
}

} // namespace game
