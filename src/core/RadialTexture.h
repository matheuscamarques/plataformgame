#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <cmath>

// Textura radial CPU-side (sem GL — testável headless).
// Usada como sprite de luz com BlendAdd (player light, tocha futura).
namespace core {

// Disco branco com falloff suave: 1.0 no centro → 0.0 na borda.
// power > 1 concentra a luz no centro (2.0 = padrão).
inline sf::Image makeRadialImage(unsigned size, float power = 2.0f) {    sf::Image img;
    img.create(size, size, sf::Color::Transparent);
    const float half = static_cast<float>(size) * 0.5f;
    for (unsigned y = 0; y < size; ++y) {
        for (unsigned x = 0; x < size; ++x) {
            const float dx = (static_cast<float>(x) + 0.5f - half) / half;
            const float dy = (static_cast<float>(y) + 0.5f - half) / half;
            const float d = std::sqrt(dx * dx + dy * dy);
            float v = (d >= 1.f) ? 0.f : 1.f - std::pow(d, power);
            const auto a = static_cast<sf::Uint8>(v * 255.f);
            img.setPixel(x, y, sf::Color(255, 255, 255, a));
        }
    }
    return img;
}

// Vinheta: preto com alpha 0 no centro → 255 nas bordas (item 22).
// CPU puro, testável; display com alpha-blend normal após o HUD.
inline sf::Image makeVignetteImage(unsigned size) {
    sf::Image img;
    img.create(size, size, sf::Color::Transparent);
    const float half = static_cast<float>(size) * 0.5f;
    for (unsigned y = 0; y < size; ++y) {
        for (unsigned x = 0; x < size; ++x) {
            const float dx = (static_cast<float>(x) + 0.5f - half) / half;
            const float dy = (static_cast<float>(y) + 0.5f - half) / half;
            const float d = std::sqrt(dx * dx + dy * dy);
            float a = (d - 0.5f) / 0.5f;
            if (a < 0.f) a = 0.f;
            if (a > 1.f) a = 1.f;
            img.setPixel(x, y, sf::Color(0, 0, 0,
                                         static_cast<sf::Uint8>(a * 255.f)));
        }
    }
    return img;
}

} // namespace core
