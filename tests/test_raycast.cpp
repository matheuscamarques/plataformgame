#include <cassert>
#include <cstdio>

#include "world/Chunk.h"
#include "world/LightPropagator.h"
#include "world/RaycastLight.h"
#include "world/Tile.h"

// Raycast: só geometria e grids (sem GL). Chunk não copia: in-place.
// Oclusão == BFS (só Ar/Água passam).
int main() {
    using namespace support;

    { // WallBlocksShadow (parede em x=8: esquerda e parede visíveis, direita não)
        Chunk c;
        for (int y = 0; y < Chunk::H; ++y)
            c.tiles[y * Chunk::W + 8] = Tile::Stone;
        RaycastLight::castRays(c, 4.5f, 8.5f);
        assert(c.isVisible(4, 8));
        assert(c.isVisible(7, 8));
        assert(c.isVisible(8, 8)); // parede é visível, mas corta o resto
        assert(!c.isVisible(9, 8));
        assert(!c.isVisible(12, 8));
    }
    { // EmptyChunkVisibleAroundSource (16px alcance cobre o 16x16 do centro)
        Chunk c;
        RaycastLight::castRays(c, 8.f, 8.f);
        assert(c.isVisible(0, 0)); // canto: dist ~11.3 < 16
        assert(c.isVisible(15, 15));
        assert(c.isVisible(8, 0));
        assert(c.isVisible(0, 8));
    }
    { // ExactDiagonalBlocked (raio 45° acerta o bloco em cheio: atrás, sombra)
        // Diagonal exata fonte(4.5,4.5)→(12,12) passa POR (8,8): física
        // honesta diz sombra (raio vizinho passa ao lado — granularidade).
        Chunk c;
        c.tiles[8 * Chunk::W + 8] = Tile::Stone;
        RaycastLight::castRays(c, 4.5f, 4.5f);
        assert(c.isVisible(8, 8));
        assert(!c.isVisible(12, 12));
        // Ao lado do bloco, passa: mesma linha fora da sombra.
        assert(c.isVisible(12, 8));
        assert(c.isVisible(8, 12));
    }
    { // OriginAlwaysVisible (origem visível mesmo em sólido)
        Chunk c;
        c.tiles[8 * Chunk::W + 8] = Tile::Stone;
        RaycastLight::castRays(c, 8.5f, 8.5f);
        assert(c.isVisible(8, 8));
    }
    { // CombinedRaycastPlusBfs (parede em x=8: luz só à esquerda)
        Chunk c;
        for (int y = 0; y < Chunk::H; ++y)
            c.tiles[y * Chunk::W + 8] = Tile::Stone;
        LightPropagator::addBlockSource(c, 4, 8, 8, true);
        assert(c.blockLight[8 * Chunk::W + 4] == 8);
        assert(c.blockLight[8 * Chunk::W + 7] > 0);
        assert(c.blockLight[8 * Chunk::W + 9] == 0);
        assert(c.blockLight[8 * Chunk::W + 12] == 0);
    }
    { // VisibleCountBounded (vazio do centro: 256/256)
        Chunk c;
        RaycastLight::castRays(c, 8.f, 8.f);
        assert(RaycastLight::visibleCount(c) == Chunk::W * Chunk::H);
    }

    std::printf("raycast test OK\n");
    return 0;
}
