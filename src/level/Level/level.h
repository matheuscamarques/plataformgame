#ifndef PLATOFORMGAME_LEVEL_H
#define PLATOFORMGAME_LEVEL_H
#include <cstdint>
#include <memory>
#include <vector>

#include "../../entities/entity/entity.hpp"
#include "../../quadtree/quadtree.h"
#include "../../world/chunkmanager.h"
#include "../../defines.h"

// Fachada sobre o ChunkManager: mantém os chunks próximos ao player
// carregados e expõe as entidades ativas como views (sem ownership).
class Level {
    public:
        explicit Level(uint32_t seed);
        std::unique_ptr<Quadtree> quadtree;

        // Recarrega/descarrega chunks em torno do tile do player e
        // reconstrói as listas ativas. Chamar uma vez por tick.
        void update(int playerTileX, int playerTileY);

        // Views não-owning para as entidades dos chunks ativos.
        std::vector<Entity*> & getPlatforms();
        std::vector<Entity*> & getColidePlatforms();
        size_t loadedChunkCount() const { return chunks.loadedCount(); }
        uint32_t getSeed() const { return chunks.getSeed(); }

    private:
        ChunkManager chunks;
        std::vector<Entity*> activePlatforms;
        std::vector<Entity*> activeColides;
};


#endif //PLATOFORMGAME_LEVEL_H
