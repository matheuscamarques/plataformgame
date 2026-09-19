#pragma once
#include "../core/System.h"
#include "Body.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace support {

struct GameContext;
class EnemyResources;
class ParticleSystem;

struct ExplosionDef {
    float radius      = 40.f;
    int   damage      = 25;
    float postureDmg  = 20.f;
    int   tilesRadius = 3;
    float knockback   = 250.f; // reservado: recursos não têm pos
};

// Alvo da explosão. Caller monta a lista (Player + Slimes).
struct ExplosionTarget {
    sf::Vector2f    center;
    Body           *body = nullptr;
    EnemyResources *resources = nullptr; // nullptr = Player, sem recursos
    bool            isPlayer = false;
};

class ExplosionSystem : public core::System {
public:
    const char *name() const override { return "ExplosionSystem"; }
    int priority() const override { return 320; }

    void tick(float /*dt*/, GameContext & /*ctx*/) override {} // explosões são síncronas

    // Explode e retorna quantos alvos foram atingidos (útil para testes).
    int explode(sf::Vector2f center, const ExplosionDef &def, GameContext &ctx);

    void setParticleSystem(ParticleSystem *p) { particles_ = p; }

private:
    ParticleSystem *particles_ = nullptr;

    void breakTilesInCircle(sf::Vector2f center, int tilesRadius, GameContext &ctx);
    bool applyToTarget(const ExplosionTarget &t, sf::Vector2f center,
                       const ExplosionDef &def);
};

} // namespace support
