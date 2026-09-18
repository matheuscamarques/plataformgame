#ifndef PLATOFORMGAME_LEVEL_H
#define PLATOFORMGAME_LEVEL_H
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "../../entities/entity/entity.hpp"
#include "../../world/chunkmanager.h"
#include "../../defines.h"

// Fachada sobre o ChunkManager: mantém os chunks próximos ao player
// carregados e expõe as entidades ativas como views (sem ownership).
// Consultas espaciais via SpatialHash de cada chunk (ver query()).
class Level {
    public:
        explicit Level(uint32_t seed);

        // Recarrega/descarrega chunks em torno do tile do player e
        // reconstrói as listas ativas. Chamar uma vez por tick.
        void update(int playerTileX, int playerTileY);

        // Espalha a query (x, y, w, h) pelos hashes dos chunks
        // carregados. 'out' é limpo antes. Sem duplicatas entre chunks.
        void query(float x, float y, float w, float h, std::vector<Entity*> &out);

        // Debug visual do SpatialHash (grade em coords de mundo).
        void debugCells(float x, float y, float w, float h,
                        std::vector<std::pair<int,int>> &out);
        int debugCellCount(int cx, int cy);

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
