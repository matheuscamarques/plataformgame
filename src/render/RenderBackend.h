/**
 * @file src/render/RenderBackend.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Interface abstrata de render 2D (fundação, sem SFML).
 * @details Declara SpriteHandle, Camera, SpriteDrawCmd e RenderBackend com sprites/retos/círculos; implementação em Render2D, incluído por quem desenha mundo sem conhecer SFML.
 */

#pragma once

#include <cstdint>

#include "core/SpriteData.h"
#include "core/Vec.h"

namespace render {

// Alça opaca de sprite (id > 0 válido; 0 = nulo).
// INVARIANTE: o handle só vale no backend que o criou — se o backend
// morre, os handles viram pó e drawSprite os ignora em silêncio.
// Por isso o backend dono de texturas é persistente, nunca local.
struct SpriteHandle {
    uint32_t id = 0;
    bool valid() const { return id != 0; }
};

// Câmera world-space (posição = canto superior-esquerdo, como Camera).
struct Camera {
    core::Vec2f position{0.f, 0.f};
    core::Vec2f viewSize{800.f, 600.f};
    float zoom = 1.f;
};

// Um desenho de sprite (origem = pivô em pixels do sprite).
struct SpriteDrawCmd {
    SpriteHandle handle;
    core::Vec2f position{0.f, 0.f};
    core::Vec2f origin{0.f, 0.f};
    core::Vec2f scale{1.f, 1.f};
    int facing = 1; // -1 espelha em X
    float rotation = 0.f;
    uint8_t alpha = 255;
};

// Backend de render: sprites + formas sólidas world-space.
// Sem texto (UI fica SFML), sem blend modes (luz fica no Render2D).
// begin/end delimitam o frame; handles vivem entre frames.
class RenderBackend {
public:
    virtual ~RenderBackend() = default;
    virtual void beginFrame(const Camera &cam) = 0;
    virtual void endFrame() = 0;
    virtual SpriteHandle createSprite(const core::SpriteData &s) = 0;
    virtual void destroySprite(SpriteHandle h) = 0;
    virtual void drawSprite(const SpriteDrawCmd &cmd) = 0;
    virtual void drawRect(core::Vec2f pos, core::Vec2f size,
                          uint32_t color) = 0;
    virtual void drawCircle(core::Vec2f center, float radius,
                            uint32_t color) = 0;
};

} // namespace render
