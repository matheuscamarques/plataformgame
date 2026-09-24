/**
 * @file src/render/Render2D.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementação SFML do RenderBackend (fundação, borda).
 * @details Render2D nasce por frame com o RenderTarget (barato: só guarda ref) e converte SpriteData/handles/cmds p/ SFML; único lugar novo que desenha mundo via backend, incluído por Renderer.
 */

#pragma once

#include <unordered_map>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "render/RenderBackend.h"

namespace render {

// Backend 2D: construído por frame com o alvo (a view ativa é do caller,
// como hoje — beginFrame não troca view, só guarda a câmera p/ debug).
class Render2D : public RenderBackend {
public:
    explicit Render2D(sf::RenderTarget &target);

    void beginFrame(const Camera &cam) override;
    void endFrame() override;
    SpriteHandle createSprite(const core::SpriteData &s) override;
    void destroySprite(SpriteHandle h) override;
    void drawSprite(const SpriteDrawCmd &cmd) override;
    void drawRect(core::Vec2f pos, core::Vec2f size, uint32_t color) override;
    void drawCircle(core::Vec2f center, float radius,
                    uint32_t color) override;

private:
    static sf::Color toColor(uint32_t rgba);

    sf::RenderTarget &target_;
    Camera cam_{};
    uint32_t nextId_ = 1;
    std::unordered_map<uint32_t, sf::Texture> textures_;
};

} // namespace render
