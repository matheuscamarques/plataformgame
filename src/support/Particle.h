#pragma once
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
    sf::Vector2f pos{0.f, 0.f};
    sf::Vector2f vel{0.f, 0.f};
    sf::Vector2f size{2.f, 2.f};
    sf::Color    color{255, 255, 255, 255};
    ParticleKind kind = ParticleKind::Dust;
    float        lifetime    = 0.f;
    float        maxLifetime = 1.f;
    float        gravity     = 0.f;
    bool         collides    = false;
    bool         active      = false;
};

} // namespace support
