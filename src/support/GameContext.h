#pragma once

#include <vector>

class Player;

namespace support {

class World;
class InputMap;
class EnemySystem;
class ThrowSystem;
class ExplosionSystem;
class DropSystem;
struct ExplosionTarget;
class ScreenshotSystem;

// Visões (sem ownership) que os sistemas recebem por tick.
// Dono continua sendo o Game.
struct GameContext {
    World *world = nullptr;
    ::Player *player = nullptr;
    InputMap *input = nullptr;
    EnemySystem *enemies = nullptr;
    ThrowSystem *throws = nullptr;
    ExplosionSystem *explodes = nullptr;
    DropSystem *drops = nullptr;

    // Preenchido pelo Game antes de explosões (Player + Slimes).
    // Dono: Game (vive no stack do tick).
    std::vector<ExplosionTarget> *explosionTargets = nullptr;

    // Debug visual (screenshots auto). Nulo em testes headless.
    ScreenshotSystem *screenshots = nullptr;
};

} // namespace support
