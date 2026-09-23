/**
 * @file src/support/Enemies/Behavior.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara a interface de identidade e ciclo de vida dos inimigos.
 * @details Define classe Behavior com kind, onTick, onSpawn e onDeath, implementada por SlimeAI e DwarfAI via BehaviorRegistry.
 */

#pragma once

#include "core/EntityKind.h"

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
    // Identidade tipada (troca os switches em name() por enum).
    virtual core::EntityKind kind() const = 0;
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
