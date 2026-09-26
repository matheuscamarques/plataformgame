/**
 * @file src/game/LogoScreen.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa a vinheta WEB-ENGENHARIA (dados + desenho).
 * @details Trilhas do SVG como polilinhas, main-path com arcos amostrados, pássaro por Bézier amostrada, texto com fade; usado por App no boot antes do jogo.
 */

#include "game/LogoScreen.h"

#include <cmath>
#include <initializer_list>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace game {
namespace logo {

namespace {

using V = core::Vec2f;
std::vector<V> pts(std::initializer_list<V> l) { return l; }

// Amostra arco circular (centro, raio, ângulos em radianos).
std::vector<V> arc(V c, float r, float a0, float a1, int n) {
    std::vector<V> out;
    for (int i = 0; i <= n; ++i) {
        const float a = a0 + (a1 - a0) * i / n;
        out.push_back({c.x + r * std::cos(a), c.y + r * std::sin(a)});
    }
    return out;
}

// Bézier cúbica amostrada.
std::vector<V> cubic(V p0, V p1, V p2, V p3, int n) {
    std::vector<V> out;
    for (int i = 0; i <= n; ++i) {
        const float t = static_cast<float>(i) / n;
        const float u = 1.f - t;
        out.push_back(
            {u * u * u * p0.x + 3 * u * u * t * p1.x + 3 * u * t * t * p2.x +
                 t * t * t * p3.x,
             u * u * u * p0.y + 3 * u * u * t * p1.y + 3 * u * t * t * p2.y +
                 t * t * t * p3.y});
    }
    return out;
}

void append(std::vector<V> &dst, const std::vector<V> &src) {
    for (std::size_t i = dst.empty() ? 0 : 1; i < src.size(); ++i)
        dst.push_back(src[i]);
}

constexpr float kPi = 3.14159265f;

} // namespace

std::vector<Trace> traces() {
    std::vector<Trace> out = {
        {{{183, 425}, {290, 425}, {300, 435}, {330, 435}, {340, 425}, {485, 425}, {615, 425}}, 0.2f, 3.0f},
        {{{485, 425}, {485, 440}}, 0.4f, 3.2f},
        {{{130, 350}, {160, 350}, {170, 340}, {170, 290}, {180, 280}, {205, 280}, {215, 290}, {215, 425}}, 0.5f, 3.3f},
        {{{235, 380}, {235, 345}, {245, 335}, {255, 345}, {255, 380}}, 0.6f, 3.5f},
        {{{285, 425}, {285, 150}}, 0.7f, 3.7f},
        {{{285, 190}, {275, 180}, {260, 180}}, 0.9f, 3.9f},
        {{{315, 380}, {315, 245}, {325, 235}, {335, 245}, {335, 380}}, 0.8f, 3.9f},
        {{{365, 425}, {365, 200}}, 0.9f, 4.1f},
        {{{365, 240}, {375, 230}, {390, 230}}, 1.0f, 4.2f},
        {{{395, 380}, {395, 305}, {405, 295}, {415, 305}, {415, 380}}, 1.0f, 4.3f},
        {{{445, 425}, {445, 280}}, 1.1f, 4.5f},
        {{{475, 380}, {475, 245}, {485, 235}, {495, 245}, {495, 380}}, 1.2f, 4.7f},
        {{{525, 425}, {525, 320}, {535, 310}, {575, 310}}, 1.3f, 4.9f},
    };
    for (auto &tr : out) computeVertexT(tr);
    return out;
}

void computeVertexT(Trace &tr) {
    const std::size_t n = tr.pts.size();
    tr.vertexT.assign(n, 0.f);
    if (n < 2) return;
    float total = 0.f;
    std::vector<float> cum(n, 0.f);
    for (std::size_t i = 1; i < n; ++i) {
        const float dx = tr.pts[i].x - tr.pts[i - 1].x;
        const float dy = tr.pts[i].y - tr.pts[i - 1].y;
        total += std::sqrt(dx * dx + dy * dy);
        cum[i] = total;
    }
    if (total <= 0.f) return;
    for (std::size_t i = 0; i < n; ++i) tr.vertexT[i] = cum[i] / total;
}

std::vector<Node> nodes() {
    const Node list[] = {
        {{173, 425}, 8.f, false, 1.5f}, {{625, 425}, 8.f, false, 1.6f},
        {{485, 450}, 8.f, false, 1.6f}, {{130, 350}, 6.f, true, 1.7f},
        {{215, 425}, 4.f, true, 1.8f},  {{285, 150}, 6.f, true, 1.8f},
        {{285, 425}, 4.f, true, 1.9f},  {{260, 180}, 4.f, true, 1.9f},
        {{365, 200}, 5.f, true, 1.9f},  {{365, 425}, 4.f, true, 2.0f},
        {{390, 230}, 4.f, true, 2.0f},  {{445, 280}, 5.f, true, 2.0f},
        {{445, 425}, 4.f, true, 2.1f},  {{575, 310}, 6.f, true, 2.1f},
        {{525, 425}, 4.f, true, 2.2f},
    };
    return {std::begin(list), std::end(list)};
}

std::vector<V> mainPath() {
    std::vector<V> out = pts({{170, 380}, {220, 380}, {220, 310}});
    append(out, arc({245, 310}, 25.f, kPi, 2 * kPi, 32));
    append(out, pts({{270, 380}, {300, 380}, {300, 180}}));
    append(out, arc({325, 180}, 25.f, kPi, 2 * kPi, 32));
    append(out, pts({{350, 380}, {380, 380}, {380, 250}}));
    append(out, arc({405, 250}, 25.f, kPi, 2 * kPi, 32));
    append(out, pts({{430, 380}, {460, 380}, {460, 180}}));
    append(out, arc({485, 180}, 25.f, kPi, 2 * kPi, 32));
    append(out, pts({{510, 380}, {590, 380}}));
    return out;
}

std::vector<V> birdBody() {
    std::vector<V> out;
    append(out, cubic({440, 265}, {380, 265}, {330, 200}, {290, 120}, 14));
    append(out, cubic({290, 120}, {350, 140}, {400, 210}, {440, 235}, 14));
    append(out, cubic({440, 235}, {480, 210}, {540, 140}, {600, 120}, 14));
    append(out, cubic({600, 120}, {550, 200}, {490, 265}, {440, 265}, 14));
    return out;
}

std::vector<V> birdBeak() {
    std::vector<V> out;
    // Quadrática M420,258 Q440,285 485,255 como duas cúbicas degeneradas.
    auto q = [&](V p0, V p1, V p2, int n) {
        for (int i = 0; i <= n; ++i) {
            const float t = static_cast<float>(i) / n;
            const float u = 1.f - t;
            out.push_back({u * u * p0.x + 2 * u * t * p1.x + t * t * p2.x,
                           u * u * p0.y + 2 * u * t * p1.y + t * t * p2.y});
        }
    };
    q({420, 258}, {440, 285}, {485, 255}, 8);
    // Curva 2 (de volta): Q 440 265 420 258 — fecha o bico.
    // O append pula o 1º ponto (duplicado com o fim da curva 1).
    std::vector<V> back;
    {
        auto qb = [&](V p0, V p1, V p2, int n) {
            for (int i = 0; i <= n; ++i) {
                const float t = static_cast<float>(i) / n;
                const float u = 1.f - t;
                back.push_back(
                    {u * u * p0.x + 2 * u * t * p1.x + t * t * p2.x,
                     u * u * p0.y + 2 * u * t * p1.y + t * t * p2.y});
            }
        };
        qb({485, 255}, {440, 265}, {420, 258}, 8);
    }
    for (std::size_t i = 1; i < back.size(); ++i) out.push_back(back[i]);
    return out;
}

namespace {

// Desenha polilinha parcial [t0, t1]. Círculos APENAS em junções
// angulares (Δθ > 15°) — em arco denso amostrado, zero círculos
// (fim do "colar de contas"). Steps adaptativos ao tamanho.
void drawPartial(render::RenderBackend &b,
                 const std::vector<core::Vec2f> &design,
                 float t0, float t1, float width, uint32_t color,
                 float scale, float ox, float oy) {
    if (t1 <= t0 || design.size() < 2) return;
    auto map = [&](core::Vec2f p) {
        return core::Vec2f{ox + p.x * scale, oy + p.y * scale};
    };
    const int steps = std::max(24, std::min(128,
        static_cast<int>(design.size()) * 3));
    core::Vec2f prev = map(polylinePointAt(design, t0));
    for (int i = 1; i <= steps; ++i) {
        const float t = t0 + (t1 - t0) * i / steps;
        const core::Vec2f cur = map(polylinePointAt(design, t));
        b.drawLine(prev, cur, width * scale, color);
        prev = cur;
    }
    for (std::size_t i = 1; i + 1 < design.size(); ++i) {
        const float vt = static_cast<float>(i) /
                         static_cast<float>(design.size() - 1);
        if (vt < t0 || vt > t1) continue;
        const core::Vec2f a{design[i].x - design[i - 1].x,
                            design[i].y - design[i - 1].y};
        const core::Vec2f c{design[i + 1].x - design[i].x,
                            design[i + 1].y - design[i].y};
        const float la = std::sqrt(a.x * a.x + a.y * a.y);
        const float lc = std::sqrt(c.x * c.x + c.y * c.y);
        if (la < 0.01f || lc < 0.01f) continue;
        const float dot =
            std::max(-1.f, std::min(1.f, (a.x * c.x + a.y * c.y) / (la * lc)));
        if (std::acos(dot) > 0.26f) // > 15°: junção real
            b.drawCircle(map(design[i]), width * 0.5f * scale, color);
    }
}

// Versão Trace: juntas por vertexT pré-computado (13 trilhas).
void drawPartialTrace(render::RenderBackend &b, const Trace &tr,
                      float t0, float t1, float width, uint32_t color,
                      float scale, float ox, float oy) {
    if (t1 <= t0 || tr.pts.size() < 2) return;
    auto map = [&](core::Vec2f p) {
        return core::Vec2f{ox + p.x * scale, oy + p.y * scale};
    };
    const int steps = 24;
    core::Vec2f prev = map(polylinePointAt(tr.pts, t0));
    for (int i = 1; i <= steps; ++i) {
        const float t = t0 + (t1 - t0) * i / steps;
        const core::Vec2f cur = map(polylinePointAt(tr.pts, t));
        b.drawLine(prev, cur, width * scale, color);
        prev = cur;
    }
    for (std::size_t i = 0; i < tr.pts.size() && i < tr.vertexT.size();
         ++i) {
        const float vt = tr.vertexT[i];
        if (vt >= t0 && vt <= t1)
            b.drawCircle(map(tr.pts[i]), width * 0.5f * scale, color);
    }
}

} // namespace

void LogoScreen::render(render::RenderBackend &backend,
                        sf::RenderTarget &target, const sf::Font &font) {
    const sf::Vector2u size = target.getSize();
    if (size.x == 0 || size.y == 0) return;
    const float W = static_cast<float>(size.x);
    const float H = static_cast<float>(size.y);
    const float s = std::min(W / 800.f, H / 600.f);
    const float ox = (W - 800.f * s) * 0.5f;
    const float oy = (H - 600.f * s) * 0.5f;
    const float e = elapsed_;

    render::Camera cam;
    cam.viewSize = {W, H};
    backend.beginFrame(cam);
    // Fundo branco.
    backend.drawRect({0.f, 0.f}, {W, H}, kBg);

    // Pontos de fundo (fade 2s @0.5s).
    {
        const float a = progress(e, 0.5f, 2.f);
        if (a > 0.f) {
            const uint8_t al = static_cast<uint8_t>(255.f * a);
            const V dots[] = {{610, 300}, {500, 80}, {410, 180},
                              {310, 140}, {210, 120}, {260, 240},
                              {580, 220}};
            const float rs[] = {5, 7, 4, 6, 4, 3, 3};
            for (int i = 0; i < 7; ++i)
                backend.drawCircle({ox + dots[i].x * s, oy + dots[i].y * s},
                                   rs[i] * s,
                                   withAlpha(kBgDot, al));
        }
    }

    // Trilhas: desenho 1.5s + pulso infinito (glow + núcleo).
    static const std::vector<Trace> trs = traces();
    for (const auto &tr : trs) {
        const float p = progress(e, tr.delay, 1.5f);
        if (p > 0.f)
            drawPartialTrace(backend, tr, 0.f, p, 4.f, kTrace, s, ox, oy);
        if (e > tr.pulseDelay && p >= 1.f) {
            const float cyc =
                std::fmod(e - tr.pulseDelay, 2.5f) / 2.5f; // 0..1
            const float head = cyc;
            const float tail = std::max(0.f, head - 0.06f);
            float alpha = 1.f;
            if (cyc < 0.1f) alpha = cyc / 0.1f;
            else if (cyc > 0.8f) alpha = (1.f - cyc) / 0.2f;
            if (alpha > 0.f && head > tail) {
                const uint8_t al =
                    static_cast<uint8_t>(255.f * alpha);
                drawPartialTrace(backend, tr, tail, head, 8.f,
                                 withAlpha(kPulse, al / 4), s, ox, oy);
                drawPartialTrace(backend, tr, tail, head, 4.f,
                                 withAlpha(kPulse, al), s, ox, oy);
            }
        }
    }

    // Estrutura principal (2.5s): contorno + corpo + veia central.
    {
        const float p = progress(e, 0.f, 2.5f);
        if (p > 0.f) {
            static const std::vector<V> mp = mainPath();
            drawPartial(backend, mp, 0.f, p, 20.f, kBaseDark, s, ox, oy);
            drawPartial(backend, mp, 0.f, p, 14.f, kBase,     s, ox, oy);
            drawPartial(backend, mp, 0.f, p,  6.f, kBaseMid,  s, ox, oy);
        }
    }

    // Nós (fade 0.5s; sólidos pulsam 3s com glow).
    static const std::vector<Node> nds = nodes();
    for (const auto &n : nds) {
        const float f = progress(e, n.delay, 0.5f);
        if (f <= 0.f) continue;
        const uint8_t al = static_cast<uint8_t>(255.f * f);
        if (n.solid) {
            const float sc =
                1.f + 0.1f * std::sin(6.2831853f * e / 3.f - 1.5707963f);
            const float opa = 0.9f + 0.1f * std::sin(6.2831853f * e / 3.f -
                                                    1.5707963f);
            const uint8_t oa = static_cast<uint8_t>(255.f * f * opa);
            backend.drawCircle({ox + n.pos.x * s, oy + n.pos.y * s},
                               n.radius * 2.f * s,
                               withAlpha(kPulse, oa / 4));
            backend.drawCircle({ox + n.pos.x * s, oy + n.pos.y * s},
                               n.radius * sc * s,
                               withAlpha(kTrace, oa));
        } else {
            backend.drawCircle({ox + n.pos.x * s, oy + n.pos.y * s},
                               n.radius * s, withAlpha(kTrace, al));
            backend.drawCircle({ox + n.pos.x * s, oy + n.pos.y * s},
                               std::max(0.f, (n.radius - 4.f)) * s,
                               withAlpha(kBg, al));
        }
    }

    // Pássaro (swoop 3.5s @1.5s + float após 5s).
    {
        static const std::vector<V> body = birdBody();
        static const std::vector<V> beak = birdBeak();
        const float t = (e - 1.5f) / 3.5f;
        if (t > 0.f) {
            const float tc = clamp01(t);
            // Keys: (0,-500,0) (0.4,+50,3.5) (0.75,-15,0.9) (1,0,1).
            float dy = 0.f, sc = 1.f;
            if (tc < 0.4f) {
                const float u = easeOutCubic(tc / 0.4f);
                dy = -500.f + 550.f * u;
                sc = 3.5f * u;
            } else if (tc < 0.75f) {
                const float u = easeOutCubic((tc - 0.4f) / 0.35f);
                dy = 50.f - 65.f * u;
                sc = 3.5f - 2.6f * u;
            } else {
                const float u = easeOutCubic((tc - 0.75f) / 0.25f);
                dy = -15.f + 15.f * u;
                sc = 0.9f + 0.1f * u;
            }
            float fy = 0.f;
            if (e > 5.f) fy = -8.f * std::sin((e - 5.f) * 1.5f);
            const float alpha =
                tc >= 1.f ? 1.f : clamp01(tc / 0.15f);
            if (sc > 0.01f && alpha > 0.f) {
                const uint8_t al = static_cast<uint8_t>(255.f * alpha);
                // Transforma pontos: escala no eixo (445,200) + voo.
                auto xf = [&](V q) {
                    return core::Vec2f{
                        ox + (445.f + (q.x - 445.f) * sc) * s,
                        oy + (200.f + (q.y - 200.f) * sc + dy + fy) * s};
                };
                std::vector<V> tb, tk;
                for (auto q : body) tb.push_back(xf(q));
                for (auto q : beak) tk.push_back(xf(q));
                // Silhueta sólida (fan), não wireframe.
                backend.drawPolygon(tb, withAlpha(kBird, al));
                backend.drawPolygon(tk, withAlpha(kBird, al));
            }
        }
    }
    backend.endFrame();

    // Texto (fade 1.2s @3.5s, sobe 10px). SFML direto, como a UI.
    {
        const float a = progress(e, 3.5f, 1.2f);
        if (a > 0.f) {
            sf::Text tx;
            tx.setFont(font);
            tx.setString("WEB-ENGENHARIA");
            tx.setCharacterSize(static_cast<unsigned>(28.f * s));
            // SFML: letter-spacing é FATOR (não px como no SVG).
            // Alvo SVG: 258px + 18px×14 = 510px → fator 510/258.
            tx.setLetterSpacing(1.98f);
            tx.setFillColor(sf::Color(0, 0, 0,
                                      static_cast<uint8_t>(255.f * a)));
            const float tw = tx.getLocalBounds().width;
            tx.setPosition((W - tw) * 0.5f, oy + (495.f + 10.f * (1.f - a)) * s);
            target.draw(tx);
        }
    }
}

} // namespace logo
} // namespace game
