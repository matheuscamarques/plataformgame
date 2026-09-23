/**
 * @file src/support/Camera/Camera.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Aplica seguimento com deadzone, interpolação e tremor de tela.
 * @details Implementa setViewport, follow com recentralização por eixo via lerp, mais trauma com decaimento e offset, usada por Game tick e Renderer via Camera.h.
 */

#include "Camera.h"

#include <algorithm>
#include <cmath>

#include "core/Math.h"
#include "core/Time.h"
#include "core/Time.h"

namespace support {

void Camera::setViewport(float w, float h) {
    viewW_ = w;
    viewH_ = h;
}

void Camera::follow(float targetX, float targetY) {
    float cx = pos_.x + viewW_ / 2.0f;
    float cy = pos_.y + viewH_ / 2.0f;
    float gx = pos_.x;
    float gy = pos_.y;
    // Fora da deadzone? Recentraliza nesse eixo (vale p/ ambos os lados).
    if (std::fabs(targetX - cx) * 2.0f > deadzone_.x) gx = targetX - viewW_ / 2.0f;
    if (std::fabs(targetY - cy) * 2.0f > deadzone_.y) gy = targetY - viewH_ / 2.0f;
    pos_.x = core::lerp(pos_.x, gx, lerp_);
    pos_.y = core::lerp(pos_.y, gy, lerp_);
}

sf::Vector2f Camera::position() const {
    const sf::Vector2f off = shakeOffset();
    return {pos_.x + off.x, pos_.y + off.y};
}

void Camera::addTrauma(float t) {
    trauma_ = std::min(1.f, trauma_ + t);
}

void Camera::tickTrauma(float dt) {
    trauma_ = std::max(0.f, trauma_ - dt * 1.5f);
}

sf::Vector2f Camera::shakeOffset() const {
    if (trauma_ <= 0.f) return {0.f, 0.f};
    const float amt = trauma_ * trauma_; // quadrático: susto grande pesa
    const float t = core::Time::elapsed();
    return {std::sin(t * 91.7f) * 12.f * amt,
            std::sin(t * 71.3f) * 12.f * amt};
}

sf::FloatRect Camera::viewRect() const {
    return sf::FloatRect(pos_.x, pos_.y, viewW_, viewH_);
}

sf::Vector2f Camera::screenToWorld(sf::Vector2f screen) const {
    return screen + pos_;
}

sf::Vector2f Camera::worldToScreen(sf::Vector2f world) const {
    return world - pos_;
}

} // namespace support
