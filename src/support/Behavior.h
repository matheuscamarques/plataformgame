#pragma once

class Entity;

namespace support {

struct Enemy;
struct GameContext;

// Behavior = identidade da entidade (SlimeAI, DwarfAI...).
// Recebe Enemy (não só o corpo): skills pagam recursos, IA lê hp.
// Uma entidade tem no máximo 1. Capacidades (vida, hitbox) são Fase C.
// Hooks opcionais (default no-op): Elite usa onTakeHit p/ interrupt;
// ninguém implementa ainda (DwarfAI pune via Recover).
class Behavior {
public:
    virtual ~Behavior() = default;
    virtual const char *name() const = 0;
    virtual void onTick(Enemy &e, float dt, GameContext &ctx) = 0;
    // Física avisa; default ignora (nem todo Behavior pula).
    virtual void setGrounded(bool) {}
    // Ciclo de vida: Factory (spawn), Melee/Explosion (hit aplicado),
    // EnemySystem::removeDead (morte). Default ignora.
    virtual void onSpawn(Enemy &, GameContext &) {}
    virtual void onTakeHit(Enemy &, int /*applied*/, GameContext &) {}
    virtual void onDeath(Enemy &, GameContext &) {}
};

} // namespace support
