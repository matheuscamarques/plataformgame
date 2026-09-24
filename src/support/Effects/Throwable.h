/**
 * @file src/support/Effects/Throwable.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define dados de projétil arremessável com pavio e dano.
 * @details Declara enum ThrowKind e struct Throwable com posição, velocidade, fuse, raios de explosão e quebra, usada pelo ThrowSystem e Player tryThrow.
 */

#pragma once

#include "core/Vec.h"
#include <SFML/System/Vector2.hpp>
#include <cstdint>

#include "core/ThrowKind.h"

namespace support {

// Alias: o enum mora em core (ItemDef não pode incluir support).
using ThrowKind = core::ThrowKind;

// Bombas do player (fuse + telegraph + glow + luz no grid).
// Spit/Barrel/Nugget/Rock ficam de fora (comportamento próprio).
inline bool isPlayerBomb(ThrowKind k) {
    return k == ThrowKind::Dynamite || k == ThrowKind::Tnt ||
           k == ThrowKind::C4 || k == ThrowKind::Daisy ||
           k == ThrowKind::Moab;
}

struct Throwable {
    core::Vec2f pos{0.f, 0.f};
    core::Vec2f vel{0.f, 0.f};
    float        gravity     = 600.f;
    float        fuse        = 1.0f;   // >0 = countdown; <=0 = sem fuse
    float        radius      = 40.f;   // raio de explosão em pixels (0 = sem)
    int          damage      = 25;
    float        postureDmg  = 20.f;
    int          tilesRadius = 3;      // raio de quebra em tiles
    float        knockback   = 250.f;  // reservado (recursos não têm pos)
    ThrowKind    kind        = ThrowKind::Dynamite;
    bool         active      = false;
    bool         resting     = false;  // parou de se mover
    float        restingTimer = 0.f;   // tempo parado
    // Cache da última escrita no grid de luz (evita re-flood por frame;
    // item 15: TNT emite blockLight enquanto o fuse corre).
    int          lastLightTileX = -9999;
    int          lastLightTileY = -9999;
    uint8_t      lastLightLevel = 0;
    int          lastChunkX = -9999;
    int          lastChunkY = -9999;
};

} // namespace support
