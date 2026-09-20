#pragma once
#include "core/System.h"
#include "core/Pool.h"
#include "Throwable.h"
#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace support {

struct GameContext;
class ExplosionSystem;
class ParticleSystem;

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
    void clear() { pool_.releaseAll(); }

    // Render/debug: itera os ativos sem expor o pool.
    // (Pool::forEachActive é não-const; render também é.)
    template <typename F>
    void forEachActive(F &&fn) { pool_.forEachActive(fn); }

private:
    core::Pool<Throwable> pool_{64};
    ExplosionSystem *explosions_ = nullptr;
    ParticleSystem  *particles_  = nullptr;

    void handleTileCollision(Throwable &t, GameContext &ctx);
    void handleFuse(Throwable &t, GameContext &ctx);
};

} // namespace support
