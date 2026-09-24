/**
 * @file src/support/Effects/ThrowSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara sistema de arremessáveis e efeitos visuais de explosão.
 * @details Define classe System com pool de 64, BlastVisual, throwItem, spawnBlast, tickBlasts e glow radial puro tntGlowParams, usada por Player e Renderer sem depender de LightingSystem.
 */

#pragma once

#include "core/Vec.h"
#include "core/System.h"
#include "core/Pool.h"
#include "Throwable.h"
#include <algorithm>
#include <cmath>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <functional>
#include <vector>

namespace support {

// Desenha glow radial (injetado pelo Renderer via LightingSystem).
// Assinatura desacoplada: ThrowSystem nunca inclui LightingSystem.
using BlastGlowFn = std::function<void(core::Vec2f, float, sf::Color)>;

struct GameContext;
class ExplosionSystem;

// Parâmetros do glow da faísca (puros, testáveis): raio cresce e
// esquenta perto do boom; flicker de 2 frequências. Renderer converte
// em sprite (cores/alpha em 0..255).
struct TntGlowParams {
    float radius;
    float r, g, b, a;
};

inline TntGlowParams tntGlowParams(float fuse, float tNow) {
    const float flicker = 1.f
        + 0.15f * std::sin(tNow * 25.f)
        + 0.10f * std::sin(tNow * 37.f);
    const float heat = std::clamp(1.f - fuse / 0.8f, 0.f, 1.f);
    TntGlowParams p;
    // Raio VISUAL (centro à borda): 25 renderiza 50px, como antes.
    p.radius = 25.f * flicker * (1.f + heat * 0.8f);
    p.r = 255.f;
    p.g = 180.f - 80.f * heat;
    p.b = 80.f - 60.f * heat;
    p.a = 120.f * flicker;
    return p;
}
class ParticleSystem;

// Anel de explosão: nasce no boom, expande até radius em maxTtl.
// Lógica pura (testável headless); desenho em renderBlasts (precisa GL).
// Flash de luz: overlay ADD temporário (não persiste no grid, atravessa
// parede — aceitável, explosão é rápida). TNT no chão não ilumina.
struct BlastVisual {
    core::Vec2f center;
    float        radius;      // alvo (o raio real do dano)
    float        ttl;         // restante
    float        maxTtl = 0.35f;
    float        lightRadius; // raio VISUAL (renderiza 3× o dano de largura)
    float        lightPeak = 1.0f; // intensidade máxima (0..1)
    bool         active = false;
};

class ThrowSystem : public core::System {
public:
    const char *name() const override { return "ThrowSystem"; }
    int priority() const override { return 220; } // após movement, antes de combat

    void tick(float dt, GameContext &ctx) override;

    // Lança um item. Retorna ponteiro ou nullptr se pool cheio.
    Throwable *throwItem(core::Vec2f from, core::Vec2f vel,
                         ThrowKind kind = ThrowKind::Dynamite);

    // Fiação (chamada uma vez no setup)
    void setExplosionSystem(ExplosionSystem *e) { explosions_ = e; }
    void setParticleSystem(ParticleSystem *p)   { particles_  = p; }

    std::size_t activeCount() const { return pool_.activeCount(); }

    // Limpa todos (restart da run).
    void clear() { pool_.releaseAll(); blasts_.clear(); }

    // Blast do boom: registrado por ExplosionSystem::explode().
    void spawnBlast(core::Vec2f center, float radius);
    void tickBlasts(float dt);
    // glow nulo = pula o flash (headless). Com glow: via drawRadial.
    void renderBlasts(sf::RenderTarget& target, BlastGlowFn glow = nullptr);
    void clearBlasts();
    std::size_t activeBlastCount() const;

    // Render/debug: itera os ativos sem expor o pool.
    // (Pool::forEachActive é não-const; render também é.)
    template <typename F>
    void forEachActive(F &&fn) { pool_.forEachActive(fn); }

private:
    core::Pool<Throwable> pool_{64};
    std::vector<BlastVisual> blasts_;
    ExplosionSystem *explosions_ = nullptr;
    ParticleSystem  *particles_  = nullptr;

    void handleTileCollision(Throwable &t, GameContext &ctx);
    void handleFuse(Throwable &t, GameContext &ctx);
};

} // namespace support
