/**
 * @file src/core/SpriteData.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Sprite neutro (sem SFML): índices + paleta com cor e parte.
 * @details SpriteData guarda pixels como índices na paleta; cada entrada tem RGBA e BodyPartId (preserva narrowphase por parte). Conversão p/ textura mora no Render2D; makeSprite legado segue em sprite_from_ascii, incluído por registries e backend.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/BodyPart.h"

namespace core {

// Cor neutra RGBA (0xRRGGBBAA) — sem sf::Color fora da borda.
inline constexpr uint32_t rgba(uint8_t r, uint8_t g, uint8_t b,
                               uint8_t a = 255) {
    return (static_cast<uint32_t>(r) << 24) |
           (static_cast<uint32_t>(g) << 16) |
           (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(a);
}

// Uma entrada de paleta: caractere + cor + parte do corpo p/ hit.
struct SpritePalEntry {
    char ch = '.';
    uint32_t color = 0; // RGBA; 0 = transparente
    BodyPartId part = BodyPartId::None;
};

// Sprite neutro: pixels são índices na paleta (0..pal.size()).
struct SpriteData {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> indices;
    std::vector<SpritePalEntry> pal;
};

// Compõe SpriteData a partir de arte ASCII + paleta com parte.
// Char fora da paleta = índice 0 (transparente, nunca crasha).
inline SpriteData toSpriteData(const char *const *rows, int w, int h,
                               const SpritePalEntry *pal,
                               std::size_t palCount) {
    SpriteData out;
    out.width = w;
    out.height = h;
    out.indices.assign(static_cast<std::size_t>(w * h), 0);
    out.pal.assign(pal, pal + palCount);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const char ch = rows[y][x];
            for (std::size_t i = 0; i < palCount; ++i) {
                if (pal[i].ch == ch) {
                    out.indices[static_cast<std::size_t>(y * w + x)] =
                        static_cast<uint8_t>(i);
                    break;
                }
            }
        }
    }
    return out;
}

// Parte do corpo no pixel (x,y) — narrowphase sem SFML.
inline BodyPartId partAt(const SpriteData &s, int x, int y) {
    if (x < 0 || y < 0 || x >= s.width || y >= s.height) return BodyPartId::None;
    const std::size_t i = s.indices[static_cast<std::size_t>(y * s.width + x)];
    if (i >= s.pal.size()) return BodyPartId::None;
    return s.pal[i].part;
}

} // namespace core
