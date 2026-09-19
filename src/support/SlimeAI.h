#pragma once

#include "../entities/entity/entity.hpp"
#include "Behavior.h"

namespace support {

// Slime: patrulha (vai-e-volta com timer + vira na parede) e persegue
// o player no alcance, pulando com cooldown. Física (gravidade/snap)
// fica no EnemySystem; aqui só intenção (vx) e pulo.
class SlimeAI : public Behavior {
public:
    const char *name() const override { return "SlimeAI"; }
    void onTick(Entity &e, float dt, GameContext &ctx) override;
    void setGrounded(bool g) override { grounded = g; }

    bool grounded = false;

private:
    float dir_ = 1.0f;
    int flipTimer_ = 0;
    int hopCooldown_ = 0;
    float lastX_ = 0.0f;
    bool hasLast_ = false;
};

} // namespace support
