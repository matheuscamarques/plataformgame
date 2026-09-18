//
// Fachada sobre o ChunkManager (Fase 4).
//

#include "level.h"

Level::Level(uint32_t seed)
    : quadtree(std::make_unique<Quadtree>(0.0f, 0.0f, 0.f, 0.f, 0, 0)),
      chunks(seed) {
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

std::vector<Entity*> & Level::getPlatforms() {
    return activePlatforms;
}

std::vector<Entity*> & Level::getColidePlatforms() {
    return activeColides;
}
