#pragma once
#include "core/Cooldown.h"
#include "core/System.h"
#include "support/Combat/Body.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class Entity;

namespace support {

struct GameContext;
class DropSystem;
class EnemyResources;
class ParticleSystem;

struct ExplosionDef {
    float radius      = 40.f;
    int   damage      = 25;
    float postureDmg  = 20.f;
    int   tilesRadius = 3;
    float knockback   = 250.f; // impulso no mover (vx/vy somam)
};

// Alvo da explosão. Caller monta a lista (Player + Slimes).
// mover: Entity real (vx/vy privados, sem Vector2 endereçável —
// por isso ponteiro para Entity, não para vel).
// Campos novos no fim: inits antigos de 4 campos continuam válidos.
struct ExplosionTarget {
    sf::Vector2f    center;
    Body           *body = nullptr;
    EnemyResources *resources = nullptr; // nullptr = Player, sem recursos
    bool            isPlayer = false;
    ::Entity       *mover = nullptr;     // nullptr = sem knockback
    core::Cooldown *knockbackLock = nullptr; // nullptr = IA sobrescreve vel
};

class ExplosionSystem : public core::System {
public:
    const char *name() const override { return "ExplosionSystem"; }
    int priority() const override { return 320; }

    void tick(float /*dt*/, GameContext & /*ctx*/) override {} // explosões são síncronas

    // Explode e retorna quantos alvos foram atingidos (útil para testes).
    int explode(sf::Vector2f center, const ExplosionDef &def, GameContext &ctx);

    void setParticleSystem(ParticleSystem *p) { particles_ = p; }

    // Drop de blocos quebrados (fase 2): BreakTilesInCircle deposita via
    // dropId do BlockDef. Nulo = sem drops (testes focados em dano).
    void setDropSystem(DropSystem *d) { drops_ = d; }

private:
    ParticleSystem *particles_ = nullptr;
    DropSystem *drops_ = nullptr;

    void breakTilesInCircle(sf::Vector2f center, int tilesRadius, GameContext &ctx);
    bool applyToTarget(const ExplosionTarget &t, sf::Vector2f center,
                       const ExplosionDef &def);
};

} // namespace support
