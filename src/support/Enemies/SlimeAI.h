/**
 * @file src/support/Enemies/SlimeAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara a IA simples de patrulha e perseguição do slime.
 * @details Define classe SlimeAI com onTick e flag chasing, usada por EnemySystem e criada via BehaviorRegistry.
 */

#pragma once

#include "entities/Entity.hpp"
#include "Behavior.h"

namespace support {

// SlimeAI: patrulha (vai-e-volta com timer + vira na parede) e persegue
// o player no alcance, pulando com cooldown. Física (gravidade/snap)
// fica no EnemySystem; aqui só intenção (vx) e pulo.
class SlimeAI : public Behavior {
public:
    const char *name() const override { return "SlimeAI"; }
    core::EntityKind kind() const override { return core::EntityKind::Slime; }
    void onTick(Enemy &e, float dt, GameContext &ctx) override;
    void setGrounded(bool g) override { grounded = g; }

    bool grounded = false;
    // Observabilidade p/ label de debug (F5). Sem lógica de gameplay.
    bool chasing() const { return chasing_; }

private:
    bool chasing_ = false;
    float dir_ = 1.0f;
    int flipTimer_ = 0;
    int hopCooldown_ = 0;
    float lastX_ = 0.0f;
    bool hasLast_ = false;
};

} // namespace support
