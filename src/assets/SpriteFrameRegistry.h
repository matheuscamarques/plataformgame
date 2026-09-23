/**
 * @file src/assets/SpriteFrameRegistry.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara estrutura e acesso aos dados de frames de sprite.
 * @details Define struct SpriteFrameData com rows, tamanho e paleta mais função frameData, incluída por quem precisa converter id em pixels sem arrastar Sprites.
 */

#pragma once
#include <cstddef>

#include "core/sprite_from_ascii.h"
#include "support/Combat/SpriteFrame.h"

namespace assets {

// Dado de frame por id. Corpo no .cpp (único TU que arrasta Sprites.h).
struct SpriteFrameData {
    const char* const* rows = nullptr;
    int w = 0, h = 0;
    const core::PaletteEntry* pal = nullptr;
    std::size_t palCount = 0;
};

SpriteFrameData frameData(support::SpriteFrameId id);

} // namespace assets
