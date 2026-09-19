#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace support {

// Câmera 2D: segue um alvo e converte coords mundo <-> tela.
// A posição é o canto superior-esquerdo da view em coords de mundo.
//
// Follow simétrico com deadzone: a câmera centraliza o alvo, mas só se
// move quando ele sai da deadzone em torno do centro atual. Funciona
// nas duas direções (inclusive coords negativas).
class Camera {
public:
    Camera() = default;

    void setViewport(float w, float h);
    void setLerp(float factor) { lerp_ = factor; }
    void setDeadzone(float w, float h) { deadzone_ = sf::Vector2f(w, h); }

    void follow(float targetX, float targetY);

    sf::Vector2f position() const { return pos_; }
    sf::FloatRect viewRect() const;
    sf::Vector2f screenToWorld(sf::Vector2f screen) const;
    sf::Vector2f worldToScreen(sf::Vector2f world) const;

private:
    sf::Vector2f pos_{0.f, 0.f};
    sf::Vector2f deadzone_{0.f, 0.f};
    float viewW_ = 0.f;
    float viewH_ = 0.f;
    float lerp_ = 1.0f;
};

} // namespace support
