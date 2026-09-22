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
// Canais (F1 master + F2/F3/F4/F6/F7): cada problema tem sua tela,
// nunca tudo junto. F2 hitboxes nasce ligado (trabalho atual); resto off.
// Fonte por referência: o Game continua dono (sem AssetManager ainda).
// Toggle via InputMap (F1); nasce visível para preservar o comportamento.
class DebugOverlay {
public:
    void toggle() { visible_ = !visible_; }
    bool visible() const { return visible_; }
    void toggleHitboxes() { hitboxes_ = !hitboxes_; }
    void toggleAi() { ai_ = !ai_; }
    void toggleEvents() { events_ = !events_; }
    void toggleWorld() { world_ = !world_; }
    void toggleLightMask() { lightMask_ = !lightMask_; }
    bool hitboxes() const { return hitboxes_; }
    bool ai() const { return ai_; }
    bool events() const { return events_; }
    bool world() const { return world_; }
    bool lightMask() const { return lightMask_; }

    void render(sf::RenderWindow &window, const sf::Font &font,
                World &world, ::Player &player,
                std::size_t totalPlatforms);

private:
    bool visible_ = true;
    bool hitboxes_ = true; // F2: melee/body/weapon/preview/aim/números
    bool ai_ = false; // F5: labels + aggro
    bool events_ = false; // F4: log
    bool world_ = false; // F6: query + grid + hash + plataformas
    bool lightMask_ = false; // F7: máscara do raycast (amarelo = visível)
};

} // namespace support
