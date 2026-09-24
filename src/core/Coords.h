/**
 * @file src/core/Coords.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Coordenadas tipadas mundo/tile/chunk + conversões (Fase 4).
 * @details WorldPos (px float), TilePos e ChunkPos (ints) com worldToTile/tileToWorld/tileToChunk/chunkOrigin; única fonte de floor/kBlockSize, incluído por quem converte coordenada.
 */

#pragma once

#include <cmath>

#include "core/Config.h"

namespace core {

// Posição em pixels no mundo (float, pode ser negativa).
struct WorldPos {
    float x = 0.f;
    float y = 0.f;
};

// Tile em grade (ints, negativos válidos — floor, nunca trunc).
struct TilePos {
    int x = 0;
    int y = 0;
    bool operator==(const TilePos &o) const { return x == o.x && y == o.y; }
};

// Chunk em grade (ints, negativos válidos).
struct ChunkPos {
    int x = 0;
    int y = 0;
    bool operator==(const ChunkPos &o) const { return x == o.x && y == o.y; }
};

// Mundo (px) -> tile (floor p/ negativos corretos).
inline TilePos worldToTile(WorldPos p) {
    const float bs = static_cast<float>(kBlockSize);
    return {static_cast<int>(std::floor(p.x / bs)),
            static_cast<int>(std::floor(p.y / bs))};
}

// Tile -> mundo (canto superior-esquerdo do tile, px).
inline WorldPos tileToWorld(TilePos t) {
    const float bs = static_cast<float>(kBlockSize);
    return {static_cast<float>(t.x) * bs, static_cast<float>(t.y) * bs};
}

// Tile -> chunk que o contém (floor p/ negativos corretos).
inline ChunkPos tileToChunk(TilePos t, int chunkSize) {
    return {static_cast<int>(std::floor(static_cast<float>(t.x) /
                                        static_cast<float>(chunkSize))),
            static_cast<int>(std::floor(static_cast<float>(t.y) /
                                        static_cast<float>(chunkSize)))};
}

// Chunk -> tile de origem (canto superior-esquerdo).
inline TilePos chunkOrigin(ChunkPos c, int chunkSize) {
    return {c.x * chunkSize, c.y * chunkSize};
}

} // namespace core
