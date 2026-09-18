//
// Fachada sobre o ChunkManager (Fase 4 + SpatialHash).
//

#include "level.h"

Level::Level(uint32_t seed)
    : chunks(seed) {
    update(0, 0); // área de spawn
}

void Level::update(int playerTileX, int playerTileY) {
    chunks.update(playerTileX, playerTileY);
    activePlatforms.clear();
    activeColides.clear();
    for (Chunk *c : chunks.loaded()) {
        for (auto &slot : c->entities) {
            Entity *e = slot.get();
            activePlatforms.push_back(e);
            if (e->getName() != WATER) activeColides.push_back(e);
        }
    }
}

void Level::query(float x, float y, float w, float h, std::vector<Entity*> &out) {
    out.clear();
    std::vector<Entity*> found;
    for (Chunk *c : chunks.loaded()) {
        c->hash.query(x, y, w, h, found);
        out.insert(out.end(), found.begin(), found.end());
    }
}

void Level::debugCells(float x, float y, float w, float h,
                       std::vector<std::pair<int,int>> &out) {
    out.clear();
    auto loaded = chunks.loaded();
    if (!loaded.empty()) loaded.front()->hash.debugCells(x, y, w, h, out);
}

int Level::debugCellCount(int cx, int cy) {
    int total = 0;
    for (Chunk *c : chunks.loaded()) total += c->hash.getCellCount(cx, cy);
    return total;
}

std::vector<Entity*> & Level::getPlatforms() {
    return activePlatforms;
}

std::vector<Entity*> & Level::getColidePlatforms() {
    return activeColides;
}
