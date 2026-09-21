#pragma once
#include <SFML/System/Vector2.hpp>

namespace game {

// Posição de overlay na grade do sprite do player (12x20).
// spriteLeft/Top = canto superior esquerdo do sprite no mundo.
// Espelha X quando facing<0. Pura, testável sem GL.
// REGRA: posições em pixels do SPRITE (rows do ASCII), nunca world.
inline sf::Vector2f equipSpritePos(float spriteLeft, float spriteTop, float s,
                                   int facing, float spriteX, float spriteY,
                                   int texW, int spriteW = 12) {
    float sx = (facing >= 0) ? spriteX : (spriteW - spriteX - texW);
    return {spriteLeft + sx * s, spriteTop + spriteY * s};
}

} // namespace game
