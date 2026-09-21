#pragma once
#include "core/System.h"
#include "core/Pool.h"
#include "Throwable.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <vector>

namespace support {

struct GameContext;
class ExplosionSystem;
class ParticleSystem;

// Anel de explosão: nasce no boom, expande até radius em maxTtl.
// Lógica pura (testável headless); desenho em renderBlasts (precisa GL).
struct BlastVisual {
    sf::Vector2f center;
    float        radius;      // alvo (o raio real do dano)
    float        ttl;         // restante
    float        maxTtl = 0.35f;
    bool         active = false;
};

class ThrowSystem : public core::System {
public:
    const char *name() const override { return "ThrowSystem"; }
    int priority() const override { return 220; } // após movement, antes de combat

    void tick(float dt, GameContext &ctx) override;

    // Lança um item. Retorna ponteiro ou nullptr se pool cheio.
    Throwable *throwItem(sf::Vector2f from, sf::Vector2f vel,
                         ThrowKind kind = ThrowKind::Dynamite);

    // Fiação (chamada uma vez no setup)
    void setExplosionSystem(ExplosionSystem *e) { explosions_ = e; }
    void setParticleSystem(ParticleSystem *p)   { particles_  = p; }

    std::size_t activeCount() const { return pool_.activeCount(); }

    // Limpa todos (restart da run).
    void clear() { pool_.releaseAll(); blasts_.clear(); }

    // Blast do boom: registrado por ExplosionSystem::explode().
    void spawnBlast(sf::Vector2f center, float radius);
    void tickBlasts(float dt);
    void renderBlasts(sf::RenderTarget& target);
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
