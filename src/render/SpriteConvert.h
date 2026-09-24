/**
 * @file src/render/SpriteConvert.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Conversor de arte ASCII+PaletteEntry para SpriteData (fronteira).
 * @details Extrai RGBA e partId de PaletteEntry (SFML) p/ SpritePalEntry neutro; usado uma vez por frame no boot do backend, incluído por quem alimenta RenderBackend com sprites do registry.
 */

#pragma once

#include "core/SpriteData.h"
#include "core/sprite_from_ascii.h"

namespace render {

// Converte arte do registry (rows + PaletteEntry) p/ SpriteData neutro.
// Char fora da paleta vira índice 0 (transparente), como no makeSprite.
inline core::SpriteData toSpriteData(const char *const *rows, int w, int h,
                                     const core::PaletteEntry *pal,
                                     std::size_t palCount) {
    core::SpriteData out;
    out.width = w;
    out.height = h;
    out.indices.assign(static_cast<std::size_t>(w * h), 0);
    out.pal.reserve(palCount);
    for (std::size_t i = 0; i < palCount; ++i) {
        core::SpritePalEntry e;
        e.ch = pal[i].ch;
        const sf::Color &c = pal[i].color;
        e.color = (c.a == 0) ? 0 : core::rgba(c.r, c.g, c.b, c.a);
        e.part = pal[i].part;
        out.pal.push_back(e);
    }
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

} // namespace render
