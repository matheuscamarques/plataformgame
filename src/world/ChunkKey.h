/**
 * @file src/world/ChunkKey.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Chaves e conversão de coordenadas de chunk e tile.
 * @details Fornece ChunkCoord, floorDiv para negativos, chunkKey int64 e conversões com chunkSize, usado por ChunkManager e World para indexar mapa.
 */

#pragma once
#include <cstdint>

namespace support {

// Coordenada de chunk (não de tile). Pode ser negativa.
struct ChunkCoord {
    int x = 0;
    int y = 0;
    bool operator==(const ChunkCoord& o) const { return x == o.x && y == o.y; }
};

// Divisão com arredondamento para baixo, correta para negativos.
// C++ '%' devolve negativo quando 'a' é negativo — isso ajusta.
inline int floorDiv(int a, int b) {
    int q = a / b;
    int r = a % b;
    if (r != 0 && ((r < 0) != (b < 0))) --q;
    return q;
}

// Combina dois int32 numa chave int64 única. O cast para uint32_t
// em 'y' garante que valores negativos não invadam os bits de 'x'.
inline int64_t chunkKey(ChunkCoord c) {
    return (static_cast<int64_t>(c.x) << 32)
         | static_cast<int64_t>(static_cast<uint32_t>(c.y));
}

inline ChunkCoord chunkCoordFromWorld(int worldTileX, int worldTileY, int chunkSize) {
    return ChunkCoord{
        floorDiv(worldTileX, chunkSize),
        floorDiv(worldTileY, chunkSize)
    };
}

// Recupera a coordenada a partir da chave — necessário para descarregar.
inline ChunkCoord chunkCoordFromKey(int64_t key) {
    return ChunkCoord{
        static_cast<int>(key >> 32),
        static_cast<int>(static_cast<uint32_t>(key & 0xFFFFFFFFu))
    };
}

} // namespace support
