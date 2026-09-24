/**
 * @file src/world/TileRenderer.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Constrói batch de quads dos tiles estáticos do chunk.
 * @details Implementa rebuild em coords locais e isTileKind para colide, água e lava, chamado por ChunkManager e World antes do desenho.
 */

#include "world/TileRenderer.h"
#include "world/Chunk.h"
#include "core/Config.h"
#include "core/EntityKind.h"
#include "core/VecSfml.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace support {

bool TileRenderer::isTileKind(int kind) {
    return kind == core::kIdColide || kind == core::kIdWater ||
           kind == core::kIdLava;
}

void TileRenderer::rebuild(Chunk& c) {
    c.tileVerts.clear();
    c.tileVerts.setPrimitiveType(sf::Quads);
    // Entidades guardam coords de MUNDO; o tileLayer é 800×800 local ao
    // chunk (view default). Sem o desconto da origem, vértices fora de
    // [0,800) clippam e só o chunk (0,0) desenharia.
    const float ox = c.cx * Chunk::W * core::kBlockSize;
    const float oy = c.cy * Chunk::H * core::kBlockSize;
    for (const auto& slot : c.entities) {
        Entity* e = slot.get();
        if (!e || !isTileKind(e->getName())) continue;
        const core::Vec2f wp = core::fromSf(e->getPosition());
        const core::Vec2f p{wp.x - ox, wp.y - oy};
        const core::Vec2f s = core::fromSf(e->getSize());
        const sf::Color col = e->getFillColor();
        c.tileVerts.append(sf::Vertex(core::toSf(p), col));
        c.tileVerts.append(sf::Vertex(core::toSf(core::Vec2f{p.x + s.x, p.y}), col));
        c.tileVerts.append(sf::Vertex(core::toSf(core::Vec2f{p.x + s.x, p.y + s.y}), col));
        c.tileVerts.append(sf::Vertex({p.x, p.y + s.y}, col));
    }
    c.tileDirty = false;
}

void TileRenderer::upload(Chunk& c) {
    constexpr unsigned PX = Chunk::W * core::kBlockSize; // 800
    constexpr unsigned PY = Chunk::H * core::kBlockSize;
    if (c.tileLayer.getSize().x != PX || c.tileLayer.getSize().y != PY)
        c.tileLayer.create(PX, PY);
    c.tileLayer.clear(sf::Color::Transparent);
    c.tileLayer.draw(c.tileVerts);
    c.tileLayer.display();
}

void TileRenderer::drawLayer(const Chunk& c, sf::RenderTarget& target) {
    if (c.tileLayer.getSize().x == 0) return;
    sf::Sprite spr(c.tileLayer.getTexture());
    spr.setPosition(c.cx * Chunk::W * core::kBlockSize,
                    c.cy * Chunk::H * core::kBlockSize);
    target.draw(spr);
}

} // namespace support
