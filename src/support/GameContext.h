#pragma once

#include <vector>

class Player;

namespace core {
class AudioSystem;
}

namespace support {

class Camera;
class World;
class InputMap;
class EnemySystem;
class ThrowSystem;
class ExplosionSystem;
class DropSystem;
struct ExplosionTarget;
class ScreenshotSystem;
struct DebugFeed;

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

    // Feed de debug (números + log). Nulo em testes headless.
    DebugFeed *debug = nullptr;

    // SFX procedural. Nulo em testes headless (call sites checam).
    // Dono: Game (vive como membro, buffers no boot).
    core::AudioSystem *audio = nullptr;

    // Câmera p/ screen shake (item 23). Nulo = sem shake. Anexado no fim
    // p/ não quebrar inits posicionais existentes.
    Camera *camera = nullptr;
};

} // namespace support
