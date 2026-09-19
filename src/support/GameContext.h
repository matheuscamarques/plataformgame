#pragma once

class Player;

namespace support {

class World;
class InputMap;

// Visões (sem ownership) que os sistemas recebem por tick.
// Dono continua sendo o Game.
struct GameContext {
    World *world = nullptr;
    ::Player *player = nullptr;
    InputMap *input = nullptr;
};

} // namespace support
