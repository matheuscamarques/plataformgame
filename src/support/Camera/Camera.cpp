#include "Camera.h"

#include "../../core/Math.h"

namespace support {

void Camera::setViewport(float w, float h) {
    viewW_ = w;
    viewH_ = h;
}

void Camera::follow(float targetX, float targetY) {
    float tx = pos_.x;
    float ty = pos_.y;
    if (targetX > viewW_ / 2.0f) tx = targetX - viewW_ / 2.0f;
    if (targetY > viewH_ / 2.0f) ty = targetY - viewH_ / 2.0f;
    pos_.x = core::lerp(pos_.x, tx, lerp_);
    pos_.y = core::lerp(pos_.y, ty, lerp_);
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
