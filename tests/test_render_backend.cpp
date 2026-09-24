/**
 * @file tests/test_render_backend.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava a interface RenderBackend (Fase 3).
 * @details Usa NullBackend (dublê que registra cmds; Render2D real exige GL e não roda headless) e cobre begin/end, ciclo de handles e registro de draws, roda com make test que compila em build/tests/test_render_backend.
 */

#include <cassert>
#include <cstdio>
#include <vector>

#include "render/RenderBackend.h"

namespace {

// Dublê de teste: registra chamadas sem desenhar nada.
class NullBackend : public render::RenderBackend {
public:
    void beginFrame(const render::Camera &) override { ++begins_; }
    void endFrame() override { ++ends_; }
    render::SpriteHandle createSprite(const core::SpriteData &) override {
        return {next_++};
    }
    void destroySprite(render::SpriteHandle h) override {
        destroyed_.push_back(h.id);
    }
    void drawSprite(const render::SpriteDrawCmd &c) override {
        sprites_.push_back(c);
    }
    void drawRect(core::Vec2f, core::Vec2f, uint32_t color) override {
        rects_.push_back(color);
    }
    void drawCircle(core::Vec2f, float, uint32_t color) override {
        circles_.push_back(color);
    }

    int begins_ = 0, ends_ = 0;
    uint32_t next_ = 1;
    std::vector<render::SpriteDrawCmd> sprites_;
    std::vector<uint32_t> rects_, circles_, destroyed_;
};

} // namespace

int main() {
    NullBackend b;
    render::Camera cam;
    cam.position = {100.f, 200.f};
    b.beginFrame(cam);

    { // HandleLifecycle (ids únicos, destroy registra)
        static const core::SpritePalEntry kPal[] = {
            {'.', 0, core::BodyPartId::None}};
        static const char *const kRows[] = {".."};
        const auto data = core::toSpriteData(kRows, 2, 1, kPal, 1);
        const auto h1 = b.createSprite(data);
        const auto h2 = b.createSprite(data);
        assert(h1.valid() && h2.valid() && h1.id != h2.id);
        b.destroySprite(h1);
        assert(b.destroyed_.size() == 1u && b.destroyed_[0] == h1.id);
    }
    { // DrawsRecorded (sprite + rect + círculo com dados)
        render::SpriteDrawCmd cmd;
        cmd.handle = {7};
        cmd.position = {10.f, 20.f};
        cmd.origin = {6.f, 40.f};
        cmd.scale = {2.5f, 2.5f};
        cmd.facing = -1;
        b.drawSprite(cmd);
        b.drawRect({0.f, 0.f}, {60.f, 6.f}, core::rgba(255, 0, 0));
        b.drawCircle({5.f, 5.f}, 3.f, core::rgba(0, 255, 0));
        assert(b.sprites_.size() == 1u);
        assert(b.sprites_[0].handle.id == 7);
        assert(b.sprites_[0].origin == core::Vec2f(6.f, 40.f));
        assert(b.sprites_[0].facing == -1);
        assert(b.rects_.size() == 1u && b.circles_.size() == 1u);
    }
    b.endFrame();
    assert(b.begins_ == 1 && b.ends_ == 1);

    std::printf("render_backend test OK\n");
    return 0;
}
