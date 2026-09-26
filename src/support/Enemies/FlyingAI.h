/**
 * @file src/support/Enemies/FlyingAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief IA voadora: persegue em 2D sem gravidade, orbita perto.
 * @details Ignora gravidade/chão (EnemySystem desvia pela flag); skills por dado (primeira Melee perto, primeira Ranged longe); patrulha em deriva senoidal, incluída por HarpyAI/DemonEyeAI via BehaviorRegistry.
 */

#pragma once

#include "Behavior.h"

namespace support {

// Voador básico: persegue o player em 2D até o aggro, orbita perto.
// Física integra a velocidade que a IA seta (sem gravidade).
class FlyingAI : public Behavior {
public:
    struct Config {
        float aggroRange = 260.f;
        float flySpeed = 5.f;
        float orbitRadius = 70.f;
    };

    FlyingAI() = default;
    explicit FlyingAI(Config cfg) : cfg_(cfg) {}

    const char *name() const override { return "FlyingAI"; }
    core::EntityKind kind() const override { return core::EntityKind::Harpy; }
    bool ignoresGravity() const override { return true; }
    void onTick(Enemy &e, float dt, GameContext &ctx) override;
    void setGrounded(bool) override {} // voador nunca tem chão

protected:
    Config cfg_;
    int tick_ = 0;
};

} // namespace support
