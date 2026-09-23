/**
 * @file src/support/Effects/Throwable.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define dados de projétil arremessável com pavio e dano.
 * @details Declara enum ThrowKind e struct Throwable com posição, velocidade, fuse, raios de explosão e quebra, usada pelo ThrowSystem e Player tryThrow.
 */

#pragma once
#include <SFML/System/Vector2.hpp>
#include <cstdint>

namespace support {

enum class ThrowKind : uint8_t {
    Dynamite,
    GoldNugget,   // stub — passivo futuro
    Rock,         // stub — variação futura
    Spit,         // projétil de slime: linear, sem fuse, dano no impacto
    Barrel,       // barril do anão: rola, detona no fuse (sem quique ainda)
    Tnt,          // escada de bombas: TNT (média)
    C4,           // C4 militar (grande)
    Moab          // MOAB (colossal)
};

// Bombas do player (fuse + telegraph + glow + luz no grid).
// Spit/Barrel/Nugget/Rock ficam de fora (comportamento próprio).
inline bool isPlayerBomb(ThrowKind k) {
    return k == ThrowKind::Dynamite || k == ThrowKind::Tnt ||
           k == ThrowKind::C4 || k == ThrowKind::Moab;
}

struct Throwable {
    sf::Vector2f pos{0.f, 0.f};
    sf::Vector2f vel{0.f, 0.f};
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
