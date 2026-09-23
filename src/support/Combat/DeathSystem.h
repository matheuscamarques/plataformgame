/**
 * @file src/support/Combat/DeathSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara o sistema que limpa mortos e entrega partículas e drops.
 * @details Define classe DeathSystem prioridade 330 com setters de Drop e Particle, registrado no loop via GameContext.
 */

#pragma once
#include "core/System.h"

namespace support {

struct GameContext;
class DropSystem;
class ParticleSystem;

class DeathSystem : public core::System {
public:
    const char *name() const override { return "DeathSystem"; }
    int priority() const override { return 330; } // após explosão, antes de particles

    void tick(float /*dt*/, GameContext &ctx) override;

    void setDropSystem(DropSystem *d)         { drops_     = d; }
    void setParticleSystem(ParticleSystem *p) { particles_ = p; }

private:
    DropSystem     *drops_     = nullptr;
    ParticleSystem *particles_ = nullptr;
};

} // namespace support
