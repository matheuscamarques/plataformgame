#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include "support/Combat/Body.h"

namespace game {

// Peça de equipamento ancorada numa parte do Body.
// REGRA: spriteOffsetPx está em pixels do SPRITE do player.
// Quem multiplica por escala/facing é pieceDrawPos, nunca o chamador.
struct PieceDraw {
    const sf::Texture *tex = nullptr;
    sf::Vector2f originPx; // pivô dentro do sprite
    support::BodyPartId anchor = support::BodyPartId::None;
    sf::Vector2f spriteOffsetPx;
};

// Posição final da peça (pura, testável sem GL).
inline sf::Vector2f pieceDrawPos(const PieceDraw &pd,
                                 const support::Body &body,
                                 int facing, float worldScale) {
    const support::PartState *part = body.find(pd.anchor);
    float ax = 0.f, ay = 0.f;
    if (part) {
        ax = part->worldBox.left + part->worldBox.width * 0.5f;
        ay = part->worldBox.top + part->worldBox.height * 0.5f;
    }
    return {ax + pd.spriteOffsetPx.x * static_cast<float>(facing) * worldScale,
            ay + pd.spriteOffsetPx.y * worldScale};
}

} // namespace game
