#include "Generation.h"

#include "../../core/Noise.h"

namespace support {

namespace {
// Meio da superfície (era `m/2` no mapa fixo 50x1000).
const int SURFACE_MID = 25;
}

int surfaceHeight(int tx, uint32_t seed) {
    float relief = core::valueNoise2D(tx * 0.02f, 3.7f, seed); // [0,1] suave
    int base = SURFACE_MID - 4 + static_cast<int>(relief * 9.0f);
    float m = mountainMask(tx, 0, seed);
    if (m > MOUNTAIN_THRESHOLD) {
        float t = (m - MOUNTAIN_THRESHOLD) / (1.0f - MOUNTAIN_THRESHOLD);
        base -= static_cast<int>(t * 18.0f); // montanha sobe (y menor = mais alto)
    }
    return base;
}

float mountainMask(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 256.0f, ty / 256.0f, seed + 1009u, 3);
}

float caveNoise(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 64.0f, ty / 64.0f, seed + 2017u, 3);
}

bool isCave(int tx, int ty, uint32_t seed, int surfaceY) {
    if (ty < surfaceY + 3) return false; // guard: nunca perto da superfície
    float depth = float(ty - surfaceY) / 25.0f;
    if (depth > 1.0f) depth = 1.0f; // Y infinito não pode virar oco
    return caveNoise(tx, ty, seed) > CAVE_BASE - depth * CAVE_DEPTH_FALLOFF;
}

int tileType(int tx, int ty, uint32_t seed) {
    int surface = surfaceHeight(tx, seed);
    if (ty > surface) {
        // Caverna antes do tipo: buraco é ar, não pedra.
        if (isCave(tx, ty, seed, surface)) return 0;
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
