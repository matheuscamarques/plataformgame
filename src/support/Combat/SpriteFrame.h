/**
 * @file src/support/Combat/SpriteFrame.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define identificadores leves de frame para player, slime e anão.
 * @details Expõe enum SpriteFrameId de 1 byte, lido por Enemy e BodySystem e resolvido no SpriteFrameRegistry para render.
 */

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

    SkeletonIdle,
    SkeletonWalkA,
    SkeletonWalkB,
    SkeletonMelee,

    COUNT
};

} // namespace support
