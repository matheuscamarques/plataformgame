/**
 * @file src/core/Vec.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Vetor 2D neutro (sem SFML) para gameplay, física e coordenadas.
 * @details Template Vec2<T> com aliases Vec2f/Vec2i, operadores e length/normalized, incluído por gameplay; fronteira SFML mora em VecSfml.h.
 */

#pragma once

#include <cmath>

namespace core {

// Vetor 2D puro (layout x,y; sem métodos SFML).
template <typename T>
struct Vec2 {
    T x = T(0);
    T y = T(0);

    constexpr Vec2() = default;
    constexpr Vec2(T x_, T y_) : x(x_), y(y_) {}

    constexpr Vec2 operator+(const Vec2 &o) const { return {x + o.x, y + o.y}; }
    constexpr Vec2 operator-(const Vec2 &o) const { return {x - o.x, y - o.y}; }
    constexpr Vec2 operator*(T s) const { return {x * s, y * s}; }
    constexpr Vec2 operator/(T s) const { return {x / s, y / s}; }
    Vec2 &operator+=(const Vec2 &o) { x += o.x; y += o.y; return *this; }
    Vec2 &operator-=(const Vec2 &o) { x -= o.x; y -= o.y; return *this; }
    Vec2 &operator*=(T s) { x *= s; y *= s; return *this; }
    Vec2 &operator/=(T s) { x /= s; y /= s; return *this; }
    constexpr bool operator==(const Vec2 &o) const {
        return x == o.x && y == o.y;
    }
    constexpr bool operator!=(const Vec2 &o) const {
        return !(*this == o);
    }

    T lengthSq() const { return x * x + y * y; }
    float length() const {
        return std::sqrt(static_cast<float>(x * x + y * y));
    }
    Vec2<float> normalized() const {
        const float l = length();
        if (l <= 1e-6f) return {0.f, 0.f};
        return {static_cast<float>(x) / l, static_cast<float>(y) / l};
    }
    constexpr T dot(const Vec2 &o) const { return x * o.x + y * o.y; }
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

} // namespace core
