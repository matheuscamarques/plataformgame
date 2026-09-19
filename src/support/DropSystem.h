#pragma once
#include "../core/Pool.h"
#include "../core/System.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>

namespace support {

struct GameContext;

struct XPOrb {
    sf::Vector2f pos{0.f, 0.f};
    sf::Vector2f vel{0.f, 0.f};
    int          value = 1;
    float        lifetime = 0.f;
    float        maxLifetime = 12.f;
    bool         active = false;
    bool         magnetized = false;
};

// Orbs de XP: pool próprio, ciclo próprio — sem lógica de gameplay
// no ParticleSystem. Sem colisão com tiles no MVP (orb atravessa
// parede: se o slime morre na beirada, nada fica preso).
class DropSystem : public core::System {
public:
    const char *name() const override { return "DropSystem"; }
    int priority() const override { return 400; }

    void tick(float dt, GameContext &ctx) override;
    void render(sf::RenderTarget &target);

    XPOrb *spawnXP(sf::Vector2f pos, int value = 1);

    std::size_t activeCount() const { return pool_.activeCount(); }
    int totalCollected() const { return collected_; }

private:
    core::Pool<XPOrb> pool_{128};
    int collected_ = 0;

    static constexpr float kMagnetRadius  = 64.f;
    static constexpr float kCollectRadius = 12.f;
    static constexpr float kMagnetSpeed   = 280.f;
    static constexpr float kGravity       = 500.f;
};

} // namespace support
