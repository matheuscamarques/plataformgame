#pragma once
#include <cmath>

#include <SFML/Graphics/Texture.hpp>

#include "Sprites.h"

class Player;

// Seleção de frame do player. Prioridade: melee > throw > jump > walk.
// Pura (sem Player completo): testável headless — compara endereços,
// sem build() de textura.
namespace game {

inline const sf::Texture *pickPlayerFrame(bool onGround, float vx,
                                          bool hurt,
                                          bool attackingMelee,
                                          bool attackingThrow,
                                          const sprites::SpriteSet &sp,
                                          int walkFrame) {
    if (hurt) return &sp.playerHurt;
    if (attackingMelee) return &sp.playerMelee;
    if (attackingThrow) return &sp.playerThrow;
    if (!onGround) return &sp.playerJump;
    if (std::fabs(vx) > 5.f) {
        return (walkFrame % 2 == 0) ? &sp.playerWalkA : &sp.playerWalkB;
    }
    return &sp.playerIdle;
}

} // namespace game
