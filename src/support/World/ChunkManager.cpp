#include "ChunkManager.h"

#include <cmath>
#include <cstdlib>

#include "../../defines.h"
#include "Generation.h"

namespace support {

namespace {
// Linha d'água em tiles do mundo (era `i > m/2` no mapa fixo 50x1000).
const int WATER_ROW = 25;
}

ChunkManager::ChunkManager(uint32_t seed, int radius, std::size_t maxLoaded)
    : seed_(seed), radius_(radius), maxLoaded_(maxLoaded) {}

static void paint(Entity *e, int t) {
    if (t == 1)      e->setFillColor(sf::Color(60, 60, 60));
    else if (t == 2) e->setFillColor(sf::Color(146, 90, 43));
    else if (t == 3) e->setFillColor(sf::Color(120, 60, 0));
    else if (t == 4) e->setFillColor(sf::Color(159, 89, 30));
    else if (t == 5) e->setFillColor(sf::Color(150, 75, 0));
    else if (t == 6) e->setFillColor(sf::Color(194, 178, 128)); // areia costeira
}

void ChunkManager::generate(int cx, int cy) {
    auto c = std::make_unique<Chunk>();
    c->cx = cx;
    c->cy = cy;
    for (int ly = 0; ly < Chunk::H; ly++) {
        for (int lx = 0; lx < Chunk::W; lx++) {
            int tx = cx * Chunk::W + lx;
            int ty = cy * Chunk::H + ly;
            int t = support::tileType(tx, ty, seed_);
            c->setTileFromGeneration(lx, ly, t);
            if (t == 0) {
                if (ty > WATER_ROW) {
                    auto water = std::make_unique<Entity>(
                        WATER, tx * BLOCK_SIZE, ty * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    water->setFillColor(sf::Color(0, 255, 255));
                    c->index(water.get());
                    c->entities.push_back(std::move(water));
                }
                continue;
            }
            auto platform = std::make_unique<Entity>(
                COLIDE, tx * BLOCK_SIZE, ty * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
            paint(platform.get(), t);
            c->index(platform.get());
            c->entities.push_back(std::move(platform));
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
    for (auto it = chunks_.begin(); it != chunks_.end();) {
        ChunkCoord c = chunkCoordFromKey(it->first);
        if (std::abs(c.x - center.x) > radius_ + 1 ||
            std::abs(c.y - center.y) > radius_ + 1) {
            lruIndex_.erase(it->first);
            lru_.remove(it->first);
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
