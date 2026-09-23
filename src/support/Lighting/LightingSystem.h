/**
 * @file src/support/Lighting/LightingSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara fachada de iluminação com ciclo dia e noite.
 * @details Define classe com init GL, ciclo lido por referência, drawPlayerLight, drawRadial e helpers puros de cor e tint, incluída por game.h e usada pelo Renderer.
 */

#pragma once
#include "core/DayNightCycle.h"
#include <algorithm>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace support {

// Fachada de luz: céu do ciclo (clear color) + overlay radial do player
// (BlendAdd direto, sem buffer) + tint do ciclo p/ sprites do lightmap.
// O sol por tile vem de LightPropagator (grids) desenhado no Renderer;
// aqui mora só o que é por frame e barato.
// skyColor/ambientSky/lightTint são puros (testáveis headless); o resto
// precisa de GL (init/draw no jogo).
class LightingSystem {
public:
    // Textura radial 1× no boot (precisa de GL — nunca em teste).
    void init();

    // Ciclo: o LightingSystem só LÊ. O tick mora no App/Game.
    void setDayNight(const core::DayNightCycle* cycle) { cycle_ = cycle; }

    // Overlay de luz do player em coords de mundo (view de mundo ativa).
    void drawPlayerLight(sf::RenderTarget& target, float worldX, float worldY);

    // Fonte única de glow radial (player, TNT, blast): origem e escala
    // derivadas da textura — nunca literal. `radius` = raio visual
    // (centro à borda); largura renderizada = 2×radius.
    void drawRadial(sf::RenderTarget& target,
                    sf::Vector2f worldPos,
                    float radius,
                    sf::Color color);

    // Radial p/ glows (player, TNT, explosão reusam; sem cópia).
    const sf::Texture& glowTexture() const { return playerTex_; }

    // Cor de fundo interpolada do ciclo (para clear do window).
    sf::Color skyColor() const;

    // Céu dinâmico acima do solo; preto abaixo (lightmap cobre).
    sf::Color ambientSky(float playerY, float surfaceY) const;

    // Tint quente/frio do ciclo p/ sprites do lightmap.
    // Grids são baked cheios (topologia); dia/noite vira multiplicador
    // de cor por frame — sem relight por tempo.
    // Sem × brilho: as âncoras já codificam o look (noite = tint fria).
    // Subsolo selado continua preto (grid 0 × qualquer tint = 0).
    sf::Color lightTint() const;

    // Matemática do glow (pura, testável): origem = centro da textura,
    // escala = raio visual / metade. Trava a classe "origin hardcoded".
    struct RadialTransform {
        float origin;
        float scale;
    };
    static RadialTransform radialTransform(float texW, float radius) {
        const float half = texW * 0.5f;
        if (half <= 0.f) return {0.f, 1.f};
        return {half, radius / half};
    }

    // Escuridão ambiente 0..1 (pura, testável): 0 = meio-dia, 1 = breu.
    static float darknessOf(float sunIntensity, float moonIntensity) {
        const float ambient =
            std::clamp(sunIntensity + moonIntensity, 0.f, 1.f);
        return 1.f - ambient;
    }
    // Escala do glow: 0.35 de dia (sol já ilumina) → 0.75 à noite.
    // Teto 0.75 (não 1.0) p/ não saturar o centro em BlendAdd.
    static float glowAlphaScale(float darkness) {
        return 0.35f + 0.4f * std::clamp(darkness, 0.f, 1.f);
    }

    void setPlayerRadius(float px)   { playerRadius_ = px; }
    void setMasterIntensity(float v) { master_ = v; }

    // Matemática do fade de profundidade (pura, testável). Mantida p/
    // compat de teste; o render por tile usa LightPropagator.
    static float lightAt(float worldY, float surfaceY,
                         float ambient, float fadeDepth);

private:
    sf::Texture       playerTex_;
    float             playerRadius_ = 90.f;
    float             master_       = 1.f;
    const core::DayNightCycle* cycle_ = nullptr;
    bool              ready_        = false;
};

} // namespace support
