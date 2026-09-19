#include "ChunkManager.h"

#include <chrono>
#include <cmath>
#include <cstdlib>

#include "../../defines.h"
#include "Block.h"
#include "Generation.h"

namespace support {

ChunkManager::ChunkManager(uint32_t seed, int radius, std::size_t maxLoaded)
    : seed_(seed), radius_(radius), maxLoaded_(maxLoaded) {}

// Estampa tronco + copa elíptica nas colunas PRÓPRIAS do chunk.
// Usa tileType (verdade global) em vez dos tiles do chunk: copa e topo
// do tronco frequentemente caem em chunk vizinho (vertical), que pode
// nem existir ainda. Nunca enterra sólido, em qualquer chunk.
static void stampTree(uint32_t seed, Chunk &c, int cx, int tx, int sy, const TreeParams &tp) {
    auto put = [&](int wx, int wy, Tile tile) {
        int lx = wx - cx * Chunk::W;
        if (lx < 0 || lx >= Chunk::W) return; // vizinho estampa a parte dele
        if (tileType(wx, wy, seed) != Tile::Air) return; // nunca enterra
        auto e = std::make_unique<Entity>(
            tile == Tile::TreeTrunk ? TREE_TRUNK : TREE_LEAF,
            wx * BLOCK_SIZE, wy * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
        e->setFillColor(blockDef(tile).color);
        c.index(e.get());
        c.entities.push_back(std::move(e));
    };

    for (int h = 1; h <= tp.trunkH; h++) {
        put(tx, sy - h, Tile::TreeTrunk);
    }
    int cyTop = sy - tp.trunkH - 1;
    int rx = tp.canopyR;
    int ry = std::max(1, (rx * 70) / 100);
    for (int dy = -ry; dy <= ry; dy++) {
        float normY = static_cast<float>(dy) / static_cast<float>(ry);
        float widthF = std::sqrt(std::max(0.0f, 1.0f - normY * normY));
        int halfW = static_cast<int>(rx * widthF);
        for (int dx = -halfW; dx <= halfW; dx++) {
            if (dx == 0) continue; // coluna do tronco: tronco já ocupa
            put(tx + dx, cyTop + dy, Tile::TreeLeaf);
        }
    }
}

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

    // 2ª passada: árvores decorativas (só entidade, tile nunca muda).
    // Range expandido para a copa não ter seam na borda; só estampa
    // colunas próprias (o vizinho estampa a parte dele, mesmo seed).
    // Spacing com lastTreeX no range expandido = determinístico.
    {
        const int x0 = cx * Chunk::W, x1 = x0 + Chunk::W;
        int lastTreeX = (x0 - TREE_EXPAND) - TREE_SPACING - 1;
        for (int tx = x0 - TREE_EXPAND; tx < x1 + TREE_EXPAND; tx++) {
            int sy = surfaceHeight(tx, seed_);
            if (tx - lastTreeX < TREE_SPACING) continue;
            bool ocean = sy > SEA_LEVEL;
            Biome b = pickBiome(temperature(tx, sy, seed_), humidity(tx, sy, seed_),
                                ocean, isCoastal(sy));
            TreeParams tp;
            if (!treeWants(tx, seed_, sy, b, tp)) continue;
            lastTreeX = tx;
            if (tx < x0 || tx >= x1) continue; // tronco fora: vizinho estampa
            stampTree(seed_, *c, cx, tx, sy, tp);
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
    evictIdleModified(kModifiedEvictIdleSeconds);
    evictIfNeeded();
}

void ChunkManager::evictIdleModified(float maxIdleSeconds) {
    const auto now = std::chrono::steady_clock::now();
    for (auto it = chunks_.begin(); it != chunks_.end();) {
        if (!it->second->modified()) {
            ++it;
            continue;
        }
        const std::chrono::duration<float> idle = now - it->second->lastAccess();
        if (idle.count() <= maxIdleSeconds) {
            ++it;
            continue;
        }
        const auto lruIt = lruIndex_.find(it->first);
        if (lruIt != lruIndex_.end()) {
            lru_.erase(lruIt->second);
            lruIndex_.erase(lruIt);
        }
        it = chunks_.erase(it);
    }
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
