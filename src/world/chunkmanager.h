#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "chunk.h"

// Gera chunks sob demanda e descarrega os distantes.
// Toda geração é função pura de (tile, seed): recarregar um chunk
// devolve exatamente o mesmo conteúdo.
class ChunkManager {
public:
    explicit ChunkManager(uint32_t seed, int radius = 2);

    // Garante carregados os chunks num raio `radius` do tile central.
    void update(int centerTileX, int centerTileY);

    // Views dos chunks carregados (ordem arbitrária).
    std::vector<Chunk*> loaded();
    size_t loadedCount() const { return chunks.size(); }
    uint32_t getSeed() const { return seed; }
    int getRadius() const { return radius; }

private:
    static int64_t key(int cx, int cy);
    static int floorDiv(int a, int b);
    static int tileType(int tx, int ty, uint32_t seed);
    void generate(int cx, int cy);

    uint32_t seed;
    int radius;
    std::unordered_map<int64_t, std::unique_ptr<Chunk>> chunks;
};
