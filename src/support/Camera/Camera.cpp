#include "Camera.h"

#include <cmath>

#include "../../core/Math.h"

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
