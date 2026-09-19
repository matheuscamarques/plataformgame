#pragma once
#include "../core/System.h"

namespace support {

struct GameContext;

// Dano de contato do slime: sobreposição com o player aplica 10 com
// i-frames do próprio player (0.6s). Sem telegraph no MVP.
// priority 310 (após melee, antes da explosão).
class ContactDamageSystem : public core::System {
public:
    const char *name() const override { return "ContactDamageSystem"; }
    int priority() const override { return 310; }

    void tick(float /*dt*/, GameContext &ctx) override;
};

} // namespace support
