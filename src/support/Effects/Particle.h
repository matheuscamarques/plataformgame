/**
 * @file src/support/Effects/Particle.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define dados de partícula individual e seus tipos.
 * @details Declara enum ParticleKind com detritos, poeira e faísca mais struct Particle com posição, velocidade, cor e tempo de vida, usada pelo ParticleSystem em pools.
 */

#pragma once

#include "core/Vec.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>

namespace support {

enum class ParticleKind : uint8_t {
    StoneDebris, DirtDebris, OreGold, OreIron, OreCrystal,
    WoodDebris, WaterDrop,
    Dust, Spark, Smoke
};

struct Particle {
    core::Vec2f pos{0.f, 0.f};
    core::Vec2f vel{0.f, 0.f};
    core::Vec2f size{2.f, 2.f};
    sf::Color    color{255, 255, 255, 255};
    ParticleKind kind = ParticleKind::Dust;
    float        lifetime    = 0.f;
    float        maxLifetime = 1.f;
    float        gravity     = 0.f;
    bool         collides    = false;
    bool         active      = false;
};

} // namespace support
