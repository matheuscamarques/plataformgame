#include <cassert>
#include <cstdio>
#include "support/World/ChunkManager.h"
#include "support/World/Tile.h"

// Eviction A: modified ocioso evicta (cratera some, regenera do seed);
// modificado fresco e chunk na janela sobrevivem.
int main() {
    using namespace support;

    { // IdleModifiedEvicts (idle 0s: tudo modificado ocioso cai)
        ChunkManager cm(1337u);
        cm.update(0, 0);
        Chunk *c = cm.find(0, 0);
        assert(c != nullptr);
        c->setTile(0, 0, Tile::Stone); // marca modified
        assert(cm.modifiedCount() >= 1u);

        cm.update(500, 0); // longe: modified pinna (sem evict seria stay)
        assert(cm.find(0, 0) != nullptr);

        const std::size_t before = cm.loadedCount();
        cm.evictIdleModified(0.f);
        assert(cm.find(0, 0) == nullptr);
        assert(cm.loadedCount() < before);
    }
    { // FreshModifiedSurvives (idle grande: nada cai)
        ChunkManager cm(1337u);
        cm.update(0, 0);
        cm.find(0, 0)->setTile(0, 0, Tile::Stone);
        const std::size_t before = cm.loadedCount();
        cm.evictIdleModified(3600.f);
        assert(cm.find(0, 0) != nullptr);
        assert(cm.loadedCount() == before);
    }
    { // EvictedRegeneratesClean (volta do seed, sem modified)
        ChunkManager cm(1337u);
        cm.update(0, 0);
        cm.find(0, 0)->setTile(0, 0, Tile::Stone);
        cm.update(500, 0);
        cm.evictIdleModified(0.f);
        assert(cm.find(0, 0) == nullptr);
        cm.update(0, 0); // volta: regenera
        Chunk *c = cm.find(0, 0);
        assert(c != nullptr && !c->modified());
    }

    std::printf("eviction test OK\n");
    return 0;
}
