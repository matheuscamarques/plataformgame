/**
 * @file tests/test_logoscreen.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless da vinheta (matemática + dados, sem GL).
 * @details Cobre clamp01/easeOutCubic, progress com delay, polylinePointAt por arco, contagem de trilhas/nós e drawLine no NullBackend, roda com make test que compila em build/tests/test_logoscreen.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "game/LogoScreen.h"
#include "render/RenderBackend.h"

namespace {

// Dublê mínimo com drawLine (espelha NullBackend do test_render_backend).
class NullLogo : public render::RenderBackend {
public:
    void beginFrame(const render::Camera &) override {}
    void endFrame() override {}
    render::SpriteHandle createSprite(const core::SpriteData &) override {
        return {};
    }
    void destroySprite(render::SpriteHandle) override {}
    void drawSprite(const render::SpriteDrawCmd &) override {}
    void drawRect(core::Vec2f, core::Vec2f, uint32_t) override {}
    void drawCircle(core::Vec2f, float, uint32_t) override {}
    void drawLine(core::Vec2f a, core::Vec2f b, float t,
                  uint32_t) override {
        lines_ += 1;
        lastLen_ = std::sqrt((b.x - a.x) * (b.x - a.x) +
                             (b.y - a.y) * (b.y - a.y));
    }
    void drawPolygon(const std::vector<core::Vec2f> &pts,
                     uint32_t) override {
        polys_ += 1;
        lastPolyN_ = pts.size();
    }
    int lines_ = 0;
    float lastLen_ = 0.f;
    int polys_ = 0;
    std::size_t lastPolyN_ = 0;
};

} // namespace

int main() {
    using namespace game::logo;

    { // MathHelpers (clamp, easing, progresso com delay)
        assert(clamp01(-1.f) == 0.f && clamp01(2.f) == 1.f);
        assert(easeOutCubic(0.f) == 0.f && easeOutCubic(1.f) == 1.f);
        assert(easeOutCubic(0.5f) > 0.5f); // saída rápida
        assert(progress(0.1f, 0.2f, 1.5f) == 0.f); // antes do delay
        assert(progress(1.0f, 0.2f, 1.5f) > 0.f &&
               progress(1.0f, 0.2f, 1.5f) < 1.f);
        assert(progress(9.f, 0.2f, 1.5f) == 1.f);
    }
    { // PolylinePointAt (arco por comprimento, não por vértice)
        const std::vector<core::Vec2f> pts = {{0.f, 0.f}, {10.f, 0.f}};
        const auto a = polylinePointAt(pts, 0.f);
        const auto b = polylinePointAt(pts, 0.5f);
        const auto c = polylinePointAt(pts, 1.f);
        assert(a.x == 0.f && b.x == 5.f && c.x == 10.f);
        assert(polylinePointAt({}, 0.5f).x == 0.f); // vazio: sem crash
    }
    { // DataCounts (fiéis ao SVG: 13 trilhas, 15 nós)
        assert(traces().size() == 13u);
        assert(nodes().size() == 15u);
        assert(mainPath().size() > 20u); // com arcos amostrados
        assert(birdBody().size() > 40u); // 4 Béziers amostradas
        assert(birdBeak().size() == 9u);
        assert(traces().front().delay == 0.2f);
    }
    { // DrawLineRecords (backend recebe linhas)
        NullLogo b;
        b.drawLine({0.f, 0.f}, {3.f, 4.f}, 2.f, 0xFFFFFFFFu);
        assert(b.lines_ == 1 && b.lastLen_ == 5.f);
        b.drawLine({1.f, 1.f}, {1.f, 1.f}, 2.f, 0); // zero: sem crash
        assert(b.lines_ == 2); // conta, Render2D filtra no GL
    }
    { // VertexTPrecomputed (0 no início, 1 no fim, monotônico)
        auto trs = traces();
        for (const auto &tr : trs) {
            assert(tr.vertexT.size() == tr.pts.size());
            assert(tr.vertexT.front() == 0.f);
            assert(tr.vertexT.back() == 1.f);
            for (std::size_t i = 1; i < tr.vertexT.size(); ++i)
                assert(tr.vertexT[i] >= tr.vertexT[i - 1]);
        }
    }
    { // DrawPolygonRecords (silhueta do pássaro chega ao backend)
        NullLogo b;
        b.drawPolygon({{0.f, 0.f}, {10.f, 0.f}, {5.f, 8.f}}, 0xFFFFFFFFu);
        assert(b.polys_ == 1 && b.lastPolyN_ == 3u);
        assert(birdBody().size() > 40u); // contorno fechado amostrado
    }
    { // FinishWindow (6.2s; antes disso não termina)
        LogoScreen l;
        assert(!l.isFinished());
        l.update(6.0f);
        assert(!l.isFinished());
        l.update(0.3f);
        assert(l.isFinished());
    }

    std::printf("logoscreen test OK\n");
    return 0;
}
