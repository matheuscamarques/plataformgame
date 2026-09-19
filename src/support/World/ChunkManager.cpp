#include "ChunkManager.h"

#include <cmath>
#include <cstdlib>

#include "../../defines.h"
#include "Block.h"
#include "Generation.h"

namespace support {

ChunkManager::ChunkManager(uint32_t seed, int radius, std::size_t maxLoaded)
    : seed_(seed), radius_(radius), maxLoaded_(maxLoaded) {}

void ChunkManager::generate(int cx, int cy) {
    auto c = std::make_unique<Chunk>();
    c->cx = cx;
    c->cy = cy;
    // Uma ColumnData por coluna do chunk: surface/mask/clima/oceano
    // calculados 1x e reusados nos 16 tiles (antes: por tile, ~16x noise).
    support::ColumnData cols[Chunk::W];
    for (int lx = 0; lx < Chunk::W; lx++) {
        cols[lx] = support::computeColumn(cx * Chunk::W + lx, seed_);
    }
    for (int ly = 0; ly < Chunk::H; ly++) {
        for (int lx = 0; lx < Chunk::W; lx++) {
            int tx = cx * Chunk::W + lx;
            int ty = cy * Chunk::H + ly;
            Tile t = support::tileType(tx, ty, seed_, cols[lx]);
            c->setTileFromGeneration(lx, ly, t);

            const BlockDef &def = support::blockDef(t);
            if (def.kind == support::BlockKind::Air) continue;

            // Liquid (água, lava) e Deco (futuro: tronco/folha) não colidem.
            // Lava distingue pelo ID (dano vai na Fase C).
            int entityKind = (def.kind == support::BlockKind::Solid) ? COLIDE
                           : (t == support::Tile::Lava) ? LAVA
                           : (def.kind == support::BlockKind::Liquid) ? WATER : 0;
            auto e = std::make_unique<Entity>(
                entityKind, tx * BLOCK_SIZE, ty * BLOCK_SIZE,
                BLOCK_SIZE, BLOCK_SIZE);
            e->setFillColor(def.color);
            c->index(e.get());
            c->entities.push_back(std::move(e));
        }
    }
    c->touch();
    int64_t k = chunkKey(ChunkCoord{cx, cy});
    chunks_.emplace(k, std::move(c));
    lru_.push_front(k);
    lruIndex_[k] = lru_.begin();
}

void ChunkManager::touchKey(int64_t k) {
    auto it = lruIndex_.find(k);
    if (it != lruIndex_.end()) lru_.erase(it->second);
    lru_.push_front(k);
    lruIndex_[k] = lru_.begin();
}

Chunk *ChunkManager::find(int cx, int cy) {
    auto it = chunks_.find(chunkKey(ChunkCoord{cx, cy}));
    return it == chunks_.end() ? nullptr : it->second.get();
}

const Chunk *ChunkManager::find(int cx, int cy) const {
    auto it = chunks_.find(chunkKey(ChunkCoord{cx, cy}));
    return it == chunks_.end() ? nullptr : it->second.get();
}

void ChunkManager::update(int centerTileX, int centerTileY) {
    ChunkCoord center = chunkCoordFromWorld(centerTileX, centerTileY, Chunk::W);
    for (int cy = center.y - radius_; cy <= center.y + radius_; cy++) {
        for (int cx = center.x - radius_; cx <= center.x + radius_; cx++) {
            int64_t k = chunkKey(ChunkCoord{cx, cy});
            auto it = chunks_.find(k);
            if (it == chunks_.end()) {
                generate(cx, cy);
            } else {
                it->second->touch();
                touchKey(k);
            }
        }
    }
    // Descarrega fora do raio + 1 (margem contra churn na borda).
    // Modificado nunca descarrega aqui (perderia mudança sem log nem
    // crash); fica até o LRU decidir — que também o poupa.
    for (auto it = chunks_.begin(); it != chunks_.end();) {
        ChunkCoord c = chunkCoordFromKey(it->first);
        if (std::abs(c.x - center.x) > radius_ + 1 ||
            std::abs(c.y - center.y) > radius_ + 1) {
            if (it->second->modified()) {
                ++it;
                continue;
            }
            auto lruIt = lruIndex_.find(it->first);
            if (lruIt != lruIndex_.end()) {
                lru_.erase(lruIt->second);
                lruIndex_.erase(lruIt);
            }
            it = chunks_.erase(it);
        } else {
            ++it;
        }
    }
    evictIfNeeded();
}

void ChunkManager::evictIfNeeded() {
    while (chunks_.size() > maxLoaded_) {
        bool evicted = false;
        // Do mais antigo ao mais recente; pula modificados.
        for (auto rit = lru_.rbegin(); rit != lru_.rend(); ++rit) {
            auto cit = chunks_.find(*rit);
            if (cit == chunks_.end()) continue;
            if (cit->second->modified()) continue;
            lruIndex_.erase(*rit);
            lru_.erase(std::next(rit).base());
            chunks_.erase(cit);
            evicted = true;
            break;
        }
        if (!evicted) break; // tudo modificado: segura em vez de perder dado
    }
}

std::vector<Chunk *> ChunkManager::loaded() {
    std::vector<Chunk *> out;
    out.reserve(chunks_.size());
    for (auto &kv : chunks_) out.push_back(kv.second.get());
    return out;
}

std::vector<const Chunk *> ChunkManager::loaded() const {
    std::vector<const Chunk *> out;
    out.reserve(chunks_.size());
    for (auto &kv : chunks_) out.push_back(kv.second.get());
    return out;
}

size_t ChunkManager::modifiedCount() const {
    size_t n = 0;
    for (auto &kv : chunks_)
        if (kv.second->modified()) n++;
    return n;
}

} // namespace support
