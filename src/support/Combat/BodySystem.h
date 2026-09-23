/**
 * @file src/support/Combat/BodySystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara o sistema que atualiza hitboxes por parte antes do combate.
 * @details Define classe BodySystem prioridade 250 com tick, registrado no loop de Systems via GameContext.
 */

#pragma once
#include "core/System.h"

namespace support {

// Rebuild das hitboxes por parte, pós-movimento e pré-combate.
// priority 250 (movement antes, combat depois).
class BodySystem : public core::System {
public:
    const char *name() const override { return "BodySystem"; }
    int priority() const override { return 250; }
    void tick(float dt, GameContext &ctx) override;
};

} // namespace support
