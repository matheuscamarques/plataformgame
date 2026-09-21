#pragma once
#include <SFML/System/Vector2.hpp>

namespace game {

// Posição de overlay na grade do sprite do player (12x20).
// spriteLeft/Top = canto superior esquerdo do sprite no mundo.
// Espelha X quando facing<0. Pura, testável sem GL.
// REGRA: posições em pixels do SPRITE (rows do ASCII), nunca world.
inline sf::Vector2f equipSpritePos(float spriteLeft, float spriteTop, float s,
                                   int facing, float spriteX, float spriteY,
                                   int spriteW = 12) {
    // facing=-1: setScale(-s,s) com origin (0,0) desenha o sprite da
    // posição para a ESQUERDA — logo a posição ancora o canto DIREITO.
    // Por isso sx = spriteW - spriteX (sem subtrair texW): com origin no
    // canto, a conta fecha sem correção extra.
    float sx = (facing >= 0) ? spriteX : (spriteW - spriteX);
    return {spriteLeft + sx * s, spriteTop + spriteY * s};
}

} // namespace game
