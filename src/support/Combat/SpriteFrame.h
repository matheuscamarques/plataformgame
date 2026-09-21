#pragma once
#include <cstdint>

// Identificador de frame. Entidades guardam só isso (1 byte, sem
// includes pesados); quem precisa de rows/pal pergunta ao registry
// (assets/SpriteFrameRegistry). Ponteiro de rows como chave foi
// rejeitado: enum explícito, sem sincronia manual além desta lista.
namespace support {

enum class SpriteFrameId : uint8_t {
    None = 0,

    PlayerIdle,
    PlayerWalkA,
    PlayerWalkB,
    PlayerJump,
    PlayerThrow,
    PlayerPunch,
    PlayerPunchUp,
    PlayerPunchDown,
    PlayerHurt,
    PlayerDeath,

    SlimeIdle,
    SlimeSquash,

    DwarfIdle,
    DwarfWalkA,
    DwarfWalkB,
    DwarfThrow,
    DwarfMelee,

    COUNT
};

} // namespace support
