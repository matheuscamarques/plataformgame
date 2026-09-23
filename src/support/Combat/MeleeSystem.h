/**
 * @file src/support/Combat/MeleeSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara o sistema de melee do player com prioridade de combate.
 * @details Define classe MeleeSystem prioridade 300 com tick e setter de partículas, registrado no loop via GameContext.
 */

#pragma once
#include "core/System.h"

namespace support {

struct GameContext;
class ParticleSystem;

// Melee do player (K): avança a state machine do swing e aplica dano
// nos slimes vivos dentro da hitbox, 1 hit por swing por slime.
// priority 300 (banda de combate, antes da explosão).
class MeleeSystem : public core::System {
public:
    const char *name() const override { return "MeleeSystem"; }
    int priority() const override { return 300; }

    void tick(float dt, GameContext &ctx) override;

    void setParticleSystem(ParticleSystem *p) { particles_ = p; }

private:
    ParticleSystem *particles_ = nullptr;
};

} // namespace support
