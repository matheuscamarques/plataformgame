#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Chunk.h"
#include "ChunkKey.h"

namespace support {

// Streaming determinístico por seed + LRU clean-only.
//
// - Geração é função pura de (tile, seed): chunk clean descartado
//   regenera idêntico; por isso o LRU só precisa reter modificados.
// - Chunk modified NUNCA é descartado pelo LRU (perderia mudança).
// - Geração é síncrona e barata (<2ms); async só depois de medir.
class ChunkManager {
public:
    explicit ChunkManager(uint32_t seed, int radius = 2, std::size_t maxLoaded = 128);

    // Garante carregados os chunks num raio `radius` do tile central.
    void update(int centerTileX, int centerTileY);

    Chunk *find(int cx, int cy);
    const Chunk *find(int cx, int cy) const;

    // Views dos chunks carregados (ordem arbitrária).
    std::vector<Chunk *> loaded();
    std::vector<const Chunk *> loaded() const;

    size_t loadedCount() const { return chunks_.size(); }
    size_t modifiedCount() const;
    uint32_t getSeed() const { return seed_; }
    int getRadius() const { return radius_; }

private:
    static int tileType(int tx, int ty, uint32_t seed);
    void generate(int cx, int cy);
    void touchKey(int64_t k);
    void evictIfNeeded();

    uint32_t seed_;
    int radius_;
    std::size_t maxLoaded_;
    std::unordered_map<int64_t, std::unique_ptr<Chunk>> chunks_;

    // LRU: front = mais recente. Só chunks clean são elegíveis.
    std::list<int64_t> lru_;
    std::unordered_map<int64_t, std::list<int64_t>::iterator> lruIndex_;
};

} // namespace support
