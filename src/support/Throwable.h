#pragma once
#include <SFML/System/Vector2.hpp>
#include <cstdint>

namespace support {

enum class ThrowKind : uint8_t {
    Dynamite,
    GoldNugget,   // stub — passivo futuro
    Rock,         // stub — variação futura
    Spit          // projétil de slime: linear, sem fuse, dano no impacto
};

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
};

} // namespace support
