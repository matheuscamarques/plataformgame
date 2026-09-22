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

class ChunkLoader;

// Streaming determinístico por seed + LRU clean-only.
//
// - Geração é função pura de (tile, seed): chunk clean descartado
//   regenera idêntico; por isso o LRU só precisa reter modificados.
// - Chunk modified NUNCA é descartado pelo LRU (perderia mudança).
// - Geração é síncrona e barata (<2ms); async (camada 6) é opt-in via
//   ChunkLoader — mesmo resultado, sem hitch. Default: sync.
class ChunkManager {
public:
    explicit ChunkManager(uint32_t seed, int radius = 2, std::size_t maxLoaded = 128);

    // Garante carregados os chunks num raio `radius` do tile central.
    void update(int centerTileX, int centerTileY);

    // Variante async (camada 6): pede faltantes ao loader, consome até
    // `budget` prontos por chamada (adopt + relight na main). Evict igual.
    // Loader desligado (default) = update() síncrono, sem thread.
    void updateAsync(int centerTileX, int centerTileY, ChunkLoader& loader,
                     int budget = 2);

    // Constrói o chunk nu (tiles + entidades + árvores; SEM relight e sem
    // inserir no mapa). Puro em (seed, coords): thread-safe, é o que o
    // worker do ChunkLoader executa. Relight/textura ficam na main.
    std::unique_ptr<Chunk> buildBare(int cx, int cy);

    Chunk *find(int cx, int cy);
    const Chunk *find(int cx, int cy) const;

    // Views dos chunks carregados (ordem arbitrária).
    std::vector<Chunk *> loaded();
    std::vector<const Chunk *> loaded() const;

    // Evict de modifieds ociosos (opção A): a cratera some após idle
    // e o chunk regenera do seed ao voltar. update() chama com o
    // default; teste injeta idle menor. Usa lastAccess_ (touch() já
    // atualiza todo update na janela — player nunca evicta a si).
    void evictIdleModified(float maxIdleSeconds);
    static constexpr float kModifiedEvictIdleSeconds = 120.f;

    size_t loadedCount() const { return chunks_.size(); }
    size_t modifiedCount() const;
    uint32_t getSeed() const { return seed_; }
    int getRadius() const { return radius_; }

private:
    void generate(int cx, int cy);
    // Insere chunk pronto no mapa + LRU (main thread).
    void adopt(std::unique_ptr<Chunk> c);
    // Relight do chunk + 4 vizinhos existentes (main thread).
    void relightAt(int cx, int cy);
    // Expurgo fora do raio + 1 (compartilhado update/updateAsync).
    void evictAround(ChunkCoord center);
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
