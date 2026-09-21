#pragma once
#include "core/DayNightCycle.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <functional>

namespace support {

// Iluminação 2D: gradiente do sol por coluna (vertex strip) + luz do
// player (sprite radial, BlendAdd), composto com BlendMultiply.
// Consome DayNightCycle (só lê; tick mora no Game). Subterrâneo apaga
// por fade de profundidade. Precisa de GL (init/composite no jogo;
// skyColor/ambientSky são puros e testáveis headless).
class LightingSystem {
public:
    void init(unsigned width, unsigned height);

    using SurfaceSampler = std::function<float(float worldX)>;
    void setSurfaceSampler(SurfaceSampler s) { surfaceAt_ = std::move(s); }

    // Ciclo: o LightingSystem só LÊ. O tick mora no App/Game.
    void setDayNight(const core::DayNightCycle* cycle) { cycle_ = cycle; }

    void beginFrame();
    void addSunGradient(float camX, float camY, float viewW, float viewH);
    void addPlayerLight(float worldX, float worldY, float camX, float camY);
    void endFrame();
    void composite(sf::RenderTarget& target);

    // Cor de fundo interpolada do ciclo (para clear do window).
    sf::Color skyColor() const;

    // Céu dinâmico acima do solo; preto abaixo (lightmap cobre).
    sf::Color ambientSky(float playerY, float surfaceY) const;

    void setSunFadeDepth(float worldPx) { fadeDepth_ = worldPx; }
    void setPlayerRadius(float px)      { playerRadius_ = px; }
    void setMasterIntensity(float v)    { master_ = v; }

    // Matemática do fade (pura, testável): 1 acima do solo → 0 no fundo.
    static float lightAt(float worldY, float surfaceY,
                         float ambient, float fadeDepth);

private:
    sf::RenderTexture buffer_;
    sf::Texture       playerTex_;
    float             fadeDepth_    = 300.f;
    float             playerRadius_ = 90.f;
    float             master_       = 1.f;
    const core::DayNightCycle* cycle_ = nullptr;
    SurfaceSampler    surfaceAt_;
    bool              ready_        = false;
};

} // namespace support
