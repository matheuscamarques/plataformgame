/**
 * @file src/support/Camera/Camera.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara câmera 2D com seguimento, conversão e screen shake.
 * @details Define classe Camera com viewport, lerp, deadzone, follow, trauma e conversões mundo-tela, incluída por game.h e usada por GameContext e Renderer.
 */

#pragma once

#include "core/Vec.h"

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
    void setDeadzone(float w, float h) { deadzone_ = core::Vec2f(w, h); }

    void follow(float targetX, float targetY);

    // Trauma p/ screen shake (item 23): 0 = quieta, 1 = caos.
    // Soma (clamp 1), decai 1.5/s. Offset quadrático + ruído temporal.
    void addTrauma(float t);
    void tickTrauma(float dt);
    core::Vec2f shakeOffset() const;

    // Posição inclui o shake (view, screenshots e ranges consistentes).
    core::Vec2f position() const;
    sf::FloatRect viewRect() const;
    core::Vec2f screenToWorld(core::Vec2f screen) const;
    core::Vec2f worldToScreen(core::Vec2f world) const;

private:
    core::Vec2f pos_{0.f, 0.f};    core::Vec2f deadzone_{0.f, 0.f};
    float viewW_ = 0.f;
    float viewH_ = 0.f;
    float lerp_ = 1.0f;
    float trauma_ = 0.f; // screen shake: soma em evento, decai por tick
};

} // namespace support
