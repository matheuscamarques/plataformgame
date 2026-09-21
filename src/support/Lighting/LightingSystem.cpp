#include "support/Lighting/LightingSystem.h"
#include "core/RadialTexture.h"
#include <algorithm>
#include <cmath>

namespace support {

void LightingSystem::init(unsigned w, unsigned h) {
    buffer_.create(w, h);
    buffer_.setSmooth(true);
    playerTex_.loadFromImage(core::makeRadialImage(128, 2.0f));
    ready_ = true;
}

void LightingSystem::beginFrame() {
    if (!ready_) return;
    buffer_.setView(buffer_.getDefaultView());
    buffer_.clear(sf::Color::Black);
}

void LightingSystem::addSunGradient(float camX, float camY,
                                    float viewW, float viewH) {
    if (!ready_ || !surfaceAt_) return;

    const core::DayNightSample dn = cycle_
        ? cycle_->sample()
        : core::DayNightSample{1.f, 0.f, 135,195,235, 255,250,240};

    // Intensidade efetiva acima do solo: sol + um pouco de lua.
    const float ambient = dn.sunIntensity + dn.moonIntensity;

    // Tint combinado (sol = quente, lua = frio) pelo peso de cada fonte.
    const float totalLight = std::max(0.01f, dn.sunIntensity + dn.moonIntensity);
    const float wSun = dn.sunIntensity / totalLight;
    const float wMoon = dn.moonIntensity / totalLight;
    const auto tintR = static_cast<sf::Uint8>(dn.tintR * wSun + 150 * wMoon);
    const auto tintG = static_cast<sf::Uint8>(dn.tintG * wSun + 180 * wMoon);
    const auto tintB = static_cast<sf::Uint8>(dn.tintB * wSun + 220 * wMoon);

    constexpr float kStep = 8.f;
    const int cols = static_cast<int>(viewW / kStep) + 2;

    sf::VertexArray va(sf::TriangleStrip, cols * 2);

    for (int i = 0; i < cols; ++i) {
        const float screenX = i * kStep;
        const float worldX  = camX + screenX;
        const float surfaceY = surfaceAt_(worldX);

        const float lTop = lightAt(camY,         surfaceY, ambient, fadeDepth_) * master_;
        const float lBot = lightAt(camY + viewH, surfaceY, ambient, fadeDepth_) * master_;

        auto shade = [&](float v) {
            const float u = std::clamp(v, 0.f, 1.f);
            return sf::Color(
                static_cast<sf::Uint8>(tintR * u),
                static_cast<sf::Uint8>(tintG * u),
                static_cast<sf::Uint8>(tintB * u),
                255);
        };

        va[i * 2 + 0] = sf::Vertex({screenX, 0.f},   shade(lTop));
        va[i * 2 + 1] = sf::Vertex({screenX, viewH}, shade(lBot));
    }

    buffer_.draw(va);
}

void LightingSystem::addPlayerLight(float worldX, float worldY,
                                    float camX, float camY) {
    if (!ready_) return;
    sf::Sprite spr(playerTex_);
    spr.setOrigin(playerTex_.getSize().x * 0.5f,
                  playerTex_.getSize().y * 0.5f);
    spr.setPosition(worldX - camX, worldY - camY);
    const float scale = playerRadius_ / 128.f;
    spr.setScale(scale, scale);

    sf::RenderStates rs;
    rs.blendMode = sf::BlendAdd;
    buffer_.draw(spr, rs);
}

void LightingSystem::endFrame() {
    if (!ready_) return;
    buffer_.display();
}

void LightingSystem::composite(sf::RenderTarget& target) {
    if (!ready_) return;
    sf::Sprite light(buffer_.getTexture());
    light.setPosition(0.f, 0.f);
    sf::RenderStates rs;
    rs.blendMode = sf::BlendMultiply;
    target.draw(light, rs);
}

float LightingSystem::lightAt(float worldY, float surfaceY,
                                float ambient, float fadeDepth) {
    const float depth = worldY - surfaceY;
    if (depth < 0.f) return ambient;             // acima: luz plena do ciclo
    if (depth > fadeDepth) return 0.f;           // fundo: escuro
    return ambient * (1.f - depth / fadeDepth);
}

sf::Color LightingSystem::skyColor() const {
    if (!cycle_) return sf::Color(135, 206, 235);
    const auto s = cycle_->sample();
    return sf::Color(s.skyR, s.skyG, s.skyB);
}

sf::Color LightingSystem::ambientSky(float playerY, float surfaceY) const {
    // Se o player está ACIMA do solo, céu dinâmico.
    if (playerY < surfaceY + 20.f) return skyColor();
    // Abaixo: preto (vai ser sobrescrito pelo lightmap).
    return sf::Color(10, 10, 15);
}

} // namespace support
