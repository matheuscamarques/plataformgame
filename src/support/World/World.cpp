#include "World.h"

#include "../../defines.h"
#include "../../entities/entity/entity.hpp"
#include "Block.h"

namespace support {

World::World(uint32_t seed)
    : chunks_(seed) {
    update(0, 0); // área de spawn
}

void World::update(int playerTileX, int playerTileY) {
    chunks_.update(playerTileX, playerTileY);
    activePlatforms_.clear();
    activeColides_.clear();
    for (Chunk *c : chunks_.loaded()) {
        for (auto &slot : c->entities) {
            Entity *e = slot.get();
            activePlatforms_.push_back(e);
            // Água, lava e deco não colidem como parede (Fase C dá dano).
            if (e->getName() != WATER && e->getName() != LAVA &&
                e->getName() != TREE_TRUNK && e->getName() != TREE_LEAF)
                activeColides_.push_back(e);
        }
    }
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
    chunk->removeEntitiesAt(worldTileX, worldTileY);
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
