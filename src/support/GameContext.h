#pragma once

#include <vector>

class Player;

namespace support {

class World;
class InputMap;
class EnemySystem;
class ThrowSystem;
class ExplosionSystem;
struct ExplosionTarget;

// Visões (sem ownership) que os sistemas recebem por tick.
// Dono continua sendo o Game.
struct GameContext {
    World *world = nullptr;
    ::Player *player = nullptr;
    InputMap *input = nullptr;
    EnemySystem *enemies = nullptr;
    ThrowSystem *throws = nullptr;
    ExplosionSystem *explodes = nullptr;

    // Preenchido pelo Game antes de explosões (Player + Slimes).
    // Dono: Game (vive no stack do tick).
    std::vector<ExplosionTarget> *explosionTargets = nullptr;
};

} // namespace support
