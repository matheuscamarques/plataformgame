/**
 * @file src/support/Effects/SpellFX.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa burst de cura, fogo Doom e aura frost (visual puro).
 * @details Sparks com física simples e fade; fogo com autômato celular e paleta quente; aura com 6 cristais orbitando; sem dano, sem estado global, usado por App (burst) e Renderer (fogo/aura).
 */

#include "support/Effects/SpellFX.h"

#include <algorithm>
#include <cmath>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "core/Random.h"

namespace support {

namespace {

uint32_t pack(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (static_cast<uint32_t>(r) << 24) |
           (static_cast<uint32_t>(g) << 16) |
           (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(a);
}

sf::Color unpack(uint32_t c) {
    return {static_cast<uint8_t>(c >> 24), static_cast<uint8_t>(c >> 16),
            static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c)};
}

sf::Color fireColor(float h) {
    if (h <= 0.05f) return sf::Color::Transparent;
    if (h > 0.75f) return {255, 255, 220, 255};
    if (h > 0.50f) return {255, 200, 80, 255};
    if (h > 0.25f) return {240, 120, 30, 255};
    return {140, 30, 10, 255};
}

} // namespace

void SpellFX::burstHeal(core::Vec2f center) {
    // Espiral subindo: velocidade com componente tangencial.
    for (int i = 0; i < 10; ++i) {
        Spark s;
        s.pos = center;
        const float a = core::randRange(0.f, 6.2831853f);
        const float sp = core::randRange(30.f, 90.f);
        const float tx = -std::sin(a) * sp * 0.7f; // tangente = redemoinho
        const float ty = std::cos(a) * sp * 0.7f;
        s.vel = {std::cos(a) * sp * 0.5f + tx,
                 std::sin(a) * sp * 0.5f + ty - 60.f}; // + sobe
        s.life = core::randRange(0.4f, 0.7f);
        s.color = pack(120, 240, 140, 255);
        sparks_.push_back(s);
    }
}

void SpellFX::drawFire(core::Vec2f center, float intensity,
                       sf::RenderTarget &target) {
    fireSeed_ = 1.f; // chama acesa neste frame
    const float px = 2.f; // pixel 2x2 (16x16 vira 32px, tamanho do slime)
    for (int y = 0; y < kFireH; ++y) {
        for (int x = 0; x < kFireW; ++x) {
            const sf::Color c = fireColor(heat_[y * kFireW + x]);
            if (c.a == 0) continue;
            sf::RectangleShape r({px, px});
            r.setPosition(center.x - 16.f + x * px,
                          center.y - 16.f + y * px);
            r.setFillColor(c);
            target.draw(r);
        }
    }
    (void)intensity;
}

void SpellFX::drawFrostAura(core::Vec2f weaponPos, sf::RenderTarget &target) {
    for (int i = 0; i < 6; ++i) {
        const float ang = auraT_ + 6.2831853f / 6.f * i;
        const float x = weaponPos.x + std::cos(ang) * 8.f;
        const float y = weaponPos.y + std::sin(ang) * 8.f;
        sf::RectangleShape c({2.f, 2.f});
        c.setPosition(x, y);
        c.setFillColor(i % 2 ? sf::Color(140, 200, 255, 200)
                             : sf::Color(220, 245, 255, 230));
        target.draw(c);
    }
}

void SpellFX::tick(float dt) {
    // Sparks: integra, flutuam p/ cima (cura sobe), expiram.
    for (auto &s : sparks_) {
        s.age += dt;
        s.pos += s.vel * dt;
        s.vel.y -= 20.f * dt;
    }
    sparks_.erase(std::remove_if(sparks_.begin(), sparks_.end(),
                                 [](const Spark &s) {
                                     return s.age >= s.life;
                                 }),
                  sparks_.end());

    // Fogo: propaga de baixo p/ cima com jitter, esfria; base segue
    // acesa só enquanto drawFire roda (chama apaga sozinha).
    for (int y = 0; y < kFireH - 1; ++y) {
        for (int x = 0; x < kFireW; ++x) {
            const int jitter =
                static_cast<int>(core::randRange(-1.f, 1.f));
            const int sx = std::clamp(x + jitter, 0, kFireW - 1);
            heat_[y * kFireW + x] =
                std::max(0.f, heat_[(y + 1) * kFireW + sx] - 0.08f);
        }
    }
    for (int x = 0; x < kFireW; ++x)
        heat_[(kFireH - 1) * kFireW + x] = fireSeed_;
    fireSeed_ = 0.f;

    auraT_ += dt * 3.f;
}

void SpellFX::render(sf::RenderTarget &target) {
    for (const auto &s : sparks_) {
        const float u = 1.f - s.age / s.life; // 1 -> 0
        sf::Color c = unpack(s.color);
        c.a = static_cast<uint8_t>(255.f * std::max(0.f, u));
        sf::RectangleShape r({2.f, 2.f});
        r.setPosition(s.pos.x, s.pos.y);
        r.setFillColor(c);
        target.draw(r);
    }
}

void SpellFX::clear() {
    sparks_.clear();
    auraT_ = 0.f;
    fireSeed_ = 0.f;
    for (auto &h : heat_) h = 0.f;
}

float SpellFX::heatAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= kFireW || y >= kFireH) return 0.f;
    return heat_[y * kFireW + x];
}

} // namespace support
