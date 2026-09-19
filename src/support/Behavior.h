#pragma once

class Entity;

namespace support {

struct GameContext;

// Behavior = identidade da entidade (SlimeAI, Patrol...).
// Uma entidade tem no máximo 1. Capacidades (vida, hitbox) são Fase C.
class Behavior {
public:
    virtual ~Behavior() = default;
    virtual const char *name() const = 0;
    virtual void onTick(Entity &e, float dt, GameContext &ctx) = 0;
    // Física avisa; default ignora (nem todo Behavior pula).
    virtual void setGrounded(bool) {}
};

} // namespace support
