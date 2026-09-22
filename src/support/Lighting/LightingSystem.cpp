#include "support/Lighting/LightingSystem.h"
#include "core/RadialTexture.h"
#include <algorithm>
#include <cmath>

namespace support {

void LightingSystem::init() {
    // Power 1.3 (era 1.6): falloff mais aberto, sem "spotlight" de borda
    // dura — combinado com o halo do fake bloom, virava dupla borda.
    playerTex_.loadFromImage(core::makeRadialImage(128, 1.3f));
    playerTex_.setSmooth(true);
    ready_ = true;
}

void LightingSystem::drawRadial(sf::RenderTarget& target,
                                 sf::Vector2f worldPos,
                                 float radius,
                                 sf::Color color) {
    if (!ready_) return;
    const float w = static_cast<float>(playerTex_.getSize().x);
    const RadialTransform t = radialTransform(w, radius);
    sf::Sprite spr(playerTex_);
    spr.setOrigin(t.origin, t.origin);
    spr.setPosition(worldPos); // coords de mundo (view de mundo ativa)
    spr.setScale(t.scale, t.scale);
    spr.setColor(color);

    sf::RenderStates rs;
    rs.blendMode = sf::BlendAdd;
    target.draw(spr, rs);
}

void LightingSystem::drawPlayerLight(sf::RenderTarget& target,
                                     float worldX, float worldY) {
    if (!ready_) return;
    // Alpha inverso ao sol: forte à noite (player carrega a visão),
    // quase invisível de dia (sol já ilumina). Antes era 140 fixo.
    float darkness = 1.f;
    if (cycle_) {
        const auto s = cycle_->sample();
        darkness = darknessOf(s.sunIntensity, s.moonIntensity);
    }
    const auto a = static_cast<sf::Uint8>(
        255.f * glowAlphaScale(darkness) * std::clamp(master_, 0.f, 1.f));
    drawRadial(target, {worldX, worldY}, playerRadius_,
               sf::Color(255, 240, 200, a));
}

sf::Color LightingSystem::skyColor() const {
    if (!cycle_) return sf::Color(135, 206, 235);
    const auto s = cycle_->sample();
    return sf::Color(s.skyR, s.skyG, s.skyB);
}

sf::Color LightingSystem::ambientSky(float /*playerY*/, float /*surfaceY*/) const {
    // Sem checagem de superfície: céu do ciclo sempre. Caverna selada
    // fica preta via lightmap (skyLight 0 × tint = 0), não via clear.
    return skyColor();
}

sf::Color LightingSystem::lightTint() const {
    if (!cycle_) return sf::Color::White;
    const auto s = cycle_->sample();
    // Piso 0.55: âncoras dão o MATIZ, o multiplicador dá a HORA.
    // Sem piso a noite ia a 0.21 (breu); sem multiplicador o ciclo
    // some (meia-noite 0.72 ≈ dia). Contraste dia/noite: 1.0 → 0.39.
    // Subsolo selado continua preto (grid 0 × qualquer tint = 0).
    const float b = std::clamp(s.sunIntensity + s.moonIntensity, 0.55f, 1.f);
    return sf::Color(static_cast<sf::Uint8>(s.tintR * b),
                     static_cast<sf::Uint8>(s.tintG * b),
                     static_cast<sf::Uint8>(s.tintB * b));
}

float LightingSystem::lightAt(float worldY, float surfaceY,
                              float ambient, float fadeDepth) {
    const float depth = worldY - surfaceY;
    if (depth < 0.f) return ambient;             // acima: luz plena do ciclo
    if (depth > fadeDepth) return 0.f;           // fundo: escuro
    return ambient * (1.f - depth / fadeDepth);
}

} // namespace support
