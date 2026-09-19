#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../core/System.h"
#include "../entities/entity/entity.hpp"
#include "Behavior.h"
#include "Body.h"
#include "EnemyResources.h"
#include "GameContext.h"
#include "../core/Cooldown.h"

namespace support {

class Behavior;

// Slime = corpo (Entity) + identidade (Behavior via registry).
// Dono: EnemySystem. Física: EnemySystem. Desenho: Game via forEach.
struct Slime {
    Entity body;
    std::unique_ptr<Behavior> ai;
    EnemyResources resources;
    Body bodyParts;
    bool grounded = false;
    // IA não sobrescreve vel enquanto roda (knockback visível).
    core::Cooldown knockbackLock;

    Slime(Entity b, std::unique_ptr<Behavior> a)
        : body(std::move(b)), ai(std::move(a)) {}
};

// Factory consome o BehaviorRegistry por string: inimigo novo não
// edita Factory, CombatSystem nem Game (só registra o Behavior).
class Factory {
public:
    // kind desconhecido -> nullptr (nunca crash).
    static std::unique_ptr<Slime> spawnEnemy(const std::string &kind,
                                             float x, float y);
};

// Opera os slimes: IA + física (gravidade, snap no chão via SpatialHash).
// priority 150 (depois de input, junto da IA).
class EnemySystem : public core::System {
public:
    const char *name() const override { return "EnemySystem"; }
    int priority() const override { return 150; }
    void tick(float dt, GameContext &ctx) override;

    // kind desconhecido = ignorado (Factory retorna null).
    void spawn(const std::string &kind, float x, float y);

    void forEach(const std::function<void(Slime &)> &fn);
    std::size_t count() const { return slimes_.size(); }

    // Remove mortos; onDeath(pos do centro) por removido para juice
    // (partículas/drops no DeathSystem). Erase mora aqui, no dono.
    void removeDead(const std::function<void(sf::Vector2f)> &onDeath);

private:
    void physics(Slime &s, GameContext &ctx);

    std::vector<std::unique_ptr<Slime>> slimes_;
};

} // namespace support
