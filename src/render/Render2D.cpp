/**
 * @file src/render/Render2D.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa Render2D sobre SFML (texturas, sprites, formas).
 * @details Converte SpriteData p/ sf::Texture (nearest, sem smooth) e cmds p/ sf::Sprite com origem/escala/flip/alpha; drawRect/drawCircle são sólidos sem outline, usado por Renderer via RenderBackend.
 */

#include "render/Render2D.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace render {

Render2D::Render2D(sf::RenderTarget &target) : target_(target) {}

void Render2D::beginFrame(const Camera &cam) { cam_ = cam; }

void Render2D::endFrame() {}

sf::Color Render2D::toColor(uint32_t rgba) {
    return {(uint8_t)(rgba >> 24), (uint8_t)(rgba >> 16),
            (uint8_t)(rgba >> 8), (uint8_t)(rgba)};
}

SpriteHandle Render2D::createSprite(const core::SpriteData &s) {
    if (s.width <= 0 || s.height <= 0 || s.indices.empty() ||
        s.pal.empty())
        return {};
    sf::Image img;
    img.create(static_cast<unsigned>(s.width),
               static_cast<unsigned>(s.height), sf::Color::Transparent);
    for (int y = 0; y < s.height; ++y) {
        for (int x = 0; x < s.width; ++x) {
            const std::size_t i =
                s.indices[static_cast<std::size_t>(y * s.width + x)];
            if (i >= s.pal.size()) continue;
            const uint32_t c = s.pal[i].color;
            if (c == 0) continue; // transparente
            img.setPixel(static_cast<unsigned>(x), static_cast<unsigned>(y),
                         toColor(c));
        }
    }
    sf::Texture t;
    if (!t.loadFromImage(img)) return {};
    t.setSmooth(false);
    const uint32_t id = nextId_++;
    textures_.emplace(id, std::move(t));
    return {id};
}

void Render2D::destroySprite(SpriteHandle h) {
    if (h.valid()) textures_.erase(h.id);
}

void Render2D::drawSprite(const SpriteDrawCmd &cmd) {
    auto it = textures_.find(cmd.handle.id);
    if (it == textures_.end()) return; // handle morto: sem crash
    sf::Sprite spr(it->second);
    spr.setOrigin(cmd.origin.x, cmd.origin.y);
    spr.setPosition(cmd.position.x, cmd.position.y);
    spr.setScale(cmd.scale.x * static_cast<float>(cmd.facing), cmd.scale.y);
    if (cmd.rotation != 0.f) spr.setRotation(cmd.rotation);
    sf::Color c((uint8_t)(cmd.color >> 24), (uint8_t)(cmd.color >> 16),
                (uint8_t)(cmd.color >> 8), (uint8_t)(cmd.color));
    if (cmd.alpha != 255)
        c.a = static_cast<uint8_t>(c.a * cmd.alpha / 255u);
    spr.setColor(c);
    target_.draw(spr);
}

void Render2D::drawRect(core::Vec2f pos, core::Vec2f size, uint32_t color) {
    sf::RectangleShape r({size.x, size.y});
    r.setPosition(pos.x, pos.y);
    r.setFillColor(toColor(color));
    target_.draw(r);
}

void Render2D::drawCircle(core::Vec2f center, float radius, uint32_t color) {
    sf::CircleShape c(radius);
    c.setOrigin(radius, radius);
    c.setPosition(center.x, center.y);
    c.setFillColor(toColor(color));
    target_.draw(c);
}

} // namespace render
