/**
 * @file src/world/World.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Fachada que liga chunks, luz, física e render do mundo.
 * @details Implementa update, tileAt, breakTile, query e listas ativas a partir de ChunkManager, chamada pelo game loop e features.
 */

#include "World.h"

#include <algorithm>
#include <vector>

#include "defines.h"
#include "entities/Entity.hpp"
#include "Block.h"
#include "ChunkLoader.h"
#include "Generation.h"
#include "LightPropagator.h"
#include "core/Coords.h"

namespace support {

World::World(uint32_t seed)
    : chunks_(seed) {
    update(0, 0); // área de spawn
}

void World::update(int playerTileX, int playerTileY) {
    if (loader_ && loader_->running())
        chunks_.updateAsync(playerTileX, playerTileY, *loader_, 2);
    else
        chunks_.update(playerTileX, playerTileY);
    activePlatforms_.clear();
    activeColides_.clear();
    for (Chunk *c : chunks_.loaded()) {
        for (auto &slot : c->entities) {
            Entity *e = slot.get();
            activePlatforms_.push_back(e);
            // Água, lava e deco não colidem como parede (Fase C dá dano).
            if (e->getName() != core::kIdWater && e->getName() != core::kIdLava &&
                e->getName() != core::kIdTreeTrunk && e->getName() != core::kIdTreeLeaf)
                activeColides_.push_back(e);
        }
    }
}

std::unique_ptr<Chunk> World::buildBareChunk(int cx, int cy) {
    return chunks_.buildBare(cx, cy);
}

Tile World::tileAt(int worldTileX, int worldTileY) const {
    ChunkCoord c = chunkCoordFromWorld(worldTileX, worldTileY, Chunk::W);
    const Chunk *chunk = chunks_.find(c.x, c.y);
    if (!chunk) return Tile::Air;
    return chunk->tile(worldTileX - c.x * Chunk::W, worldTileY - c.y * Chunk::H);
}

bool World::isSolid(int worldTileX, int worldTileY) const {
    return support::isSolid(tileAt(worldTileX, worldTileY));
}

float World::surfaceYAt(float worldX) const {
    const int tx = core::worldToTile({worldX, 0.f}).x;
    return static_cast<float>(support::surfaceHeight(tx, getSeed()))
         * core::kBlockSize;
}

bool World::breakTile(int worldTileX, int worldTileY, Tile *broken) {
    ChunkCoord c = chunkCoordFromWorld(worldTileX, worldTileY, Chunk::W);
    Chunk *chunk = chunks_.find(c.x, c.y);
    if (!chunk) return false;
    const int lx = worldTileX - c.x * Chunk::W;
    const int ly = worldTileY - c.y * Chunk::H;
    const Tile cur = chunk->tile(lx, ly);
    // Só Sólido e Deco quebram. Ar não há o que fazer; Bedrock é
    // indestrutível; Liquid (água/lava) a explosão não toca.
    if (cur == Tile::Air || cur == Tile::Bedrock) return false;
    if (isLiquid(cur)) return false;
    if (broken) *broken = cur;
    chunk->setTile(lx, ly, Tile::Air); // marca modified: persiste no LRU
    // Expurga as views: a Entity morre aqui, mas activePlatforms_/
    // activeColides_ só seriam rebuilt no próximo update() — o render
    // do frame atual dereferenciaria (segfault do J).
    std::vector<Entity *> removed;
    removed.reserve(4);
    chunk->removeEntitiesAt(worldTileX, worldTileY, &removed);
    if (!removed.empty()) {
        auto expunge = [&](std::vector<Entity *> &v) {
            v.erase(std::remove_if(v.begin(), v.end(), [&](Entity *e) {
                for (Entity *r : removed)
                    if (r == e) return true;
                return false;
            }), v.end());
        };
        expunge(activePlatforms_);
        expunge(activeColides_);
    }
    // Luz: relight do chunk (só grids + dirty, sem GL — headless-safe).
    // Borda do chunk: vizinhos também (luz vaza entre chunks).
    LightPropagator::relightChunk(
        *chunk, chunks_.find(c.x, c.y - 1), chunks_.find(c.x - 1, c.y),
        chunks_.find(c.x + 1, c.y), chunks_.find(c.x, c.y + 1));
    if (lx == 0 || lx == Chunk::W - 1 || ly == 0 || ly == Chunk::H - 1) {
        auto relightNb = [&](int nx, int ny) {
            if (Chunk *nb = chunks_.find(nx, ny))
                LightPropagator::relightChunk(
                    *nb, chunks_.find(nx, ny - 1), chunks_.find(nx - 1, ny),
                    chunks_.find(nx + 1, ny), chunks_.find(nx, ny + 1));
        };
        relightNb(c.x - 1, c.y);
        relightNb(c.x + 1, c.y);
        relightNb(c.x, c.y - 1);
        relightNb(c.x, c.y + 1);
    }
    return true;
}

void World::query(float x, float y, float w, float h, std::vector<Entity*> &out) {
    out.clear();
    std::vector<Entity*> found;
    for (Chunk *c : chunks_.loaded()) {
        c->hash.query(x, y, w, h, found);
        out.insert(out.end(), found.begin(), found.end());
    }
}

void World::debugCells(float x, float y, float w, float h,
                       std::vector<std::pair<int,int>> &out) {
    out.clear();
    auto loaded = chunks_.loaded();
    if (!loaded.empty()) loaded.front()->hash.debugCells(x, y, w, h, out);
}

int World::debugCellCount(int cx, int cy) {
    int total = 0;
    for (Chunk *c : chunks_.loaded()) total += c->hash.getCellCount(cx, cy);
    return total;
}

const std::vector<Entity*> & World::getPlatforms() {
    return activePlatforms_;
}

const std::vector<Entity*> & World::getColidePlatforms() {
    return activeColides_;
}

} // namespace support
