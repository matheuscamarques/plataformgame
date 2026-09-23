/**
 * @file src/core/sprite_from_ascii.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Construtor de texturas pixel art a partir de arte ASCII.
 * @details Mapeia caracteres para cores e partes do corpo via makeSprite, usado por itens, Player e inimigos.
 */

#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cstddef>

#include "core/BodyPart.h"

namespace core {

// Um caractere = um pixel. Linhas devem ter exatamente `w` chars;
// (teste cobre widths — textura exige contexto GL, sem teste headless).
// `part` liga pixel a hitbox (rebuildFromSprite); None = só visual.
struct PaletteEntry {
    char ch;
    sf::Color color;
    BodyPartId part = BodyPartId::None;
};

inline sf::Texture makeSprite(const char *const *rows, int w, int h,
                              const PaletteEntry *pal, std::size_t palCount) {
    sf::Image img;
    img.create(static_cast<unsigned>(w), static_cast<unsigned>(h),
               sf::Color::Transparent);
    for (int y = 0; y < h; ++y) {
        const char *row = rows[y];
        for (int x = 0; x < w; ++x) {
            const char ch = row[x];
            for (std::size_t i = 0; i < palCount; ++i) {
                if (pal[i].ch == ch) {
                    img.setPixel(static_cast<unsigned>(x),
                                 static_cast<unsigned>(y), pal[i].color);
                    break;
                }
            }
        }
    }
    sf::Texture t;
    t.loadFromImage(img);
    t.setSmooth(false); // pixel art: sem interpolação no upscale
    return t;
}

} // namespace core
