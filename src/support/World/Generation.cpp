#include "Generation.h"

#include "../../core/Noise.h"

namespace support {

namespace {
// Meio da superfície (era `m/2` no mapa fixo 50x1000).
const int SURFACE_MID = 25;
}

int surfaceHeight(int tx, uint32_t seed) {
    float relief = core::valueNoise2D(tx * 0.02f, 3.7f, seed); // [0,1] suave
    return SURFACE_MID - 4 + static_cast<int>(relief * 9.0f);
}

int tileType(int tx, int ty, uint32_t seed) {
    int surface = surfaceHeight(tx, seed);
    if (ty > surface) {
        // Maciço: sempre sólido; tipo cosmético varia por hash.
        float pick = core::rand01(tx, ty, seed ^ 0x9E3779B9u);
        if (pick < 0.15f)      return 1;
        else if (pick < 0.35f) return 2;
        else if (pick < 0.55f) return 3;
        else if (pick < 0.75f) return 4;
        else                   return 5;
    }
    if (ty == surface) return 4; // topo: sempre sólido
    float plat = core::rand01(tx, ty, seed ^ 0x51F37EDu);
    return plat < 0.035f ? 2 : 0; // plataformas esparsas, resto vazio
}

} // namespace support
