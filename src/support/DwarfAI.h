#pragma once

#include <cstdint>

#include "../core/Cooldown.h"
#include "../entities/entity/entity.hpp"
#include "Behavior.h"

namespace support {

// Anão básico: patrulha a home, alerta, joga dinamite, picaretada de
// perto, recua quando o player sai. Sem interrupt-on-hit (Behavior não
// tem hook de dano — Recover 0.6s é a janela de punish). Sem skills,
// sem paciência, sem variante: isso é Anão Elite.
// Estado mora aqui (como SlimeAI guarda dir_); corpo/recursos no Enemy.
enum class DwarfState : uint8_t {
    Patrol,
    Alert,
    ThrowWindup,
    ThrowRelease,
    Recover,
    Melee,
    Retreat
};

class DwarfAI : public Behavior {
public:
    struct Config {
        int hp = 60;
        float aggroRange = 200.f; // px
        float homeRadius = 96.f;  // px
        float throwRange = 128.f; // px
        float meleeRange = 32.f;  // px
    };

    DwarfAI() = default;
    explicit DwarfAI(Config cfg) : cfg_(cfg) {}

    const char *name() const override { return "DwarfAI"; }
    void onTick(Enemy &e, float dt, GameContext &ctx) override;

    // Observabilidade de teste (não API de gameplay).
    DwarfState state() const { return state_; }

private:
    Config cfg_;
    DwarfState state_ = DwarfState::Patrol;

    sf::Vector2f home_{0.f, 0.f};
    bool homed_ = false;
    int patrolDir_ = 1;

    core::Cooldown stateTimer_;
    core::Cooldown throwCd_{1.8f};
    core::Cooldown meleeCd_{0.8f};

    void changeState(DwarfState s, float duration = 0.f);
    void tickPatrol(Enemy &e, float dt, GameContext &ctx);
    void tickCombat(Enemy &e, float dt, GameContext &ctx);
    void throwDynamite(Enemy &e, GameContext &ctx);
};

} // namespace support
