#pragma once

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <utility>
#include <vector>

class Player;

namespace support {

class World;

// Overlay de debug do mundo: retângulo da query, células do SpatialHash
// com contagens, total de plataformas e candidatos perto do player.
//
// Fonte por referência: o Game continua dono (sem AssetManager ainda).
// Toggle via InputMap (F1); nasce visível para preservar o comportamento.
class DebugOverlay {
public:
    void toggle() { visible_ = !visible_; }
    bool visible() const { return visible_; }

    void render(sf::RenderWindow &window, const sf::Font &font,
                World &world, ::Player &player,
                std::size_t totalPlatforms);

private:
    bool visible_ = true;
};

} // namespace support
