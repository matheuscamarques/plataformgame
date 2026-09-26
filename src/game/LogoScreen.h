/**
 * @file src/game/LogoScreen.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Splash animado WEB-ENGENHARIA (trilhas PCB + pássaro).
 * @details Recria a animação do site (drawPath, dataPulse, birdSwoop, fadeIn) com primitivas do RenderBackend + sf::Text; tempo único, sem estado global, incluído por App para o boot.
 */

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "core/Vec.h"
#include "render/RenderBackend.h"

namespace game {
namespace logo {

// Matemática pura da animação (testável sem GL).
inline float clamp01(float v) {
    return v < 0.f ? 0.f : (v > 1.f ? 1.f : v);
}
inline float easeOutCubic(float t) {
    t = clamp01(t);
    const float u = 1.f - t;
    return 1.f - u * u * u;
}
// Progresso 0..1 de um efeito com delay e duração.
inline float progress(float elapsed, float delay, float dur) {
    if (dur <= 0.f) return elapsed >= delay ? 1.f : 0.f;
    return clamp01((elapsed - delay) / dur);
}
// Ponto na polilinha na fração t (0..1 por comprimento de arco).
inline core::Vec2f polylinePointAt(const std::vector<core::Vec2f> &pts,
                                   float t) {
    if (pts.empty()) return {0.f, 0.f};
    if (pts.size() == 1 || t <= 0.f) return pts.front();
    if (t >= 1.f) return pts.back();
    float total = 0.f;
    for (std::size_t i = 1; i < pts.size(); ++i) {
        const float dx = pts[i].x - pts[i - 1].x;
        const float dy = pts[i].y - pts[i - 1].y;
        total += std::sqrt(dx * dx + dy * dy);
    }
    if (total <= 0.f) return pts.front();
    float want = t * total;
    for (std::size_t i = 1; i < pts.size(); ++i) {
        const float dx = pts[i].x - pts[i - 1].x;
        const float dy = pts[i].y - pts[i - 1].y;
        const float len = std::sqrt(dx * dx + dy * dy);
        if (want <= len)
            return {pts[i - 1].x + (len > 0.f ? dx * want / len : 0.f),
                    pts[i - 1].y + (len > 0.f ? dy * want / len : 0.f)};
        want -= len;
    }
    return pts.back();
}

// Duração total da vinheta (texto pronto + pássaro pousado).
inline constexpr float kDuration = 6.2f;

struct Trace {
    std::vector<core::Vec2f> pts;
    float delay = 0.f;       // desenho (drawPath 1.5s)
    float pulseDelay = 3.f;  // dataPulse infinito a partir daqui
};

struct Node {
    core::Vec2f pos;
    float radius = 4.f;
    bool solid = true;
    float delay = 1.5f; // fadeIn 0.5s
};

// Tabelas fiéis ao SVG (viewBox 800x600).
std::vector<Trace> traces();
std::vector<Node> nodes();
std::vector<core::Vec2f> mainPath();
std::vector<core::Vec2f> birdBody();
std::vector<core::Vec2f> birdBeak();

// Cores do CSS (:root).
inline constexpr uint32_t kBase = 0x062A1BFFu;
inline constexpr uint32_t kTrace = 0xA5C7A9FFu;
inline constexpr uint32_t kPulse = 0xE2F2E4FFu;
inline constexpr uint32_t kBird = 0x62866DFFu;
inline constexpr uint32_t kBg = 0xFFFFFFFFu;
inline constexpr uint32_t kBgDot = 0xE5EBE5FFu;
inline constexpr uint32_t kText = 0x000000FFu;

// Troca o byte alpha de uma cor RGBA.
inline uint32_t withAlpha(uint32_t c, uint8_t a) {
    return (c & 0xFFFFFF00u) | a;
}

class LogoScreen {
public:
    LogoScreen() = default;

    void update(float dt) { elapsed_ += dt; }
    float elapsed() const { return elapsed_; }
    bool isFinished() const { return elapsed_ >= kDuration; }

    // Desenha no alvo (vectores via backend, texto via SFML direto,
    // como a UI). Design 800x600 centralizado com fit.
    void render(render::RenderBackend &backend, sf::RenderTarget &target,
                const sf::Font &font);

private:
    float elapsed_ = 0.f;
};

} // namespace logo
} // namespace game
