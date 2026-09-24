/**
 * @file tests/test_coords.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava core::Coords (Fase 4 da fundação).
 * @details Cobre worldToTile (positivos, negativos, borda), tileToWorld, tileToChunk e chunkOrigin com round-trip, roda com make test que compila em build/tests/test_coords.
 */

#include <cassert>
#include <cstdio>

#include "core/Coords.h"

int main() {
    using core::ChunkPos;
    using core::TilePos;
    using core::WorldPos;

    { // WorldToTile (floor: borda e negativos corretos)
        assert((core::worldToTile({0.f, 0.f}) == TilePos{0, 0}));
        assert((core::worldToTile({49.9f, 49.9f}) == TilePos{0, 0}));
        assert((core::worldToTile({50.f, 50.f}) == TilePos{1, 1}));
        assert((core::worldToTile({100.f, 1350.f}) == TilePos{2, 27}));
        assert((core::worldToTile({-0.1f, -0.1f}) == TilePos{-1, -1}));
        assert((core::worldToTile({-50.f, -50.f}) == TilePos{-1, -1}));
        assert((core::worldToTile({-50.1f, -50.1f}) == TilePos{-2, -2}));
    }
    { // TileToWorld (canto do tile)
        const WorldPos w = core::tileToWorld(TilePos{2, 27});
        assert(w.x == 100.f && w.y == 1350.f);
        const WorldPos n = core::tileToWorld(TilePos{-1, -1});
        assert(n.x == -50.f && n.y == -50.f);
    }
    { // TileToChunk (tamanho 16, negativos com floor)
        assert((core::tileToChunk(TilePos{0, 0}, 16) == ChunkPos{0, 0}));
        assert((core::tileToChunk(TilePos{15, 15}, 16) == ChunkPos{0, 0}));
        assert((core::tileToChunk(TilePos{16, 16}, 16) == ChunkPos{1, 1}));
        assert((core::tileToChunk(TilePos{-1, -1}, 16) == ChunkPos{-1, -1}));
        assert((core::tileToChunk(TilePos{-16, -16}, 16) == ChunkPos{-1, -1}));
        assert((core::tileToChunk(TilePos{-17, -17}, 16) == ChunkPos{-2, -2}));
    }
    { // ChunkOriginRoundTrip (origem + round-trip)
        assert((core::chunkOrigin(ChunkPos{0, 0}, 16) == TilePos{0, 0}));
        assert((core::chunkOrigin(ChunkPos{-1, 2}, 16) == TilePos{-16, 32}));
        const TilePos t{37, -5};
        const ChunkPos c = core::tileToChunk(t, 16);
        const TilePos o = core::chunkOrigin(c, 16);
        assert(t.x >= o.x && t.x < o.x + 16);
        assert(t.y >= o.y && t.y < o.y + 16);
    }

    std::printf("coords test OK\n");
    return 0;
}
