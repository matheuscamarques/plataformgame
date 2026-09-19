#include "Generation.h"

#include <cmath>

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
        // Raiz quadrada: a máscara passa a maior parte do tempo pouco
        // acima do limiar; linear daria morros de 1-2 tiles (invisíveis).
        // sqrt(t) dobra as montanhas visíveis com o mesmo degrau máximo.
        float t = (m - MOUNTAIN_THRESHOLD) / (1.0f - MOUNTAIN_THRESHOLD);
        base -= static_cast<int>(std::sqrt(t) * 18.0f); // y menor = mais alto
    }
    return base;
}

float mountainMask(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 256.0f, ty / 256.0f, seed + 1009u, 3);
}

float caveNoise(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 64.0f, ty / 64.0f, seed + 2017u, 3);
}

float temperature(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 512.0f, ty / 512.0f, seed + 3019u, 3);
}

float humidity(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 384.0f, ty / 384.0f, seed + 4021u, 3);
}

bool isCave(int tx, int ty, uint32_t seed, int surfaceY, float mountain) {
    if (ty < surfaceY + 3) return false; // guard: nunca perto da superfície
    float depth = float(ty - surfaceY) / 25.0f;
    if (depth > 1.0f) depth = 1.0f; // Y infinito não pode virar oco
    // Ordem fixa: base -> falloff de profundidade -> bônus de montanha -> piso.
    float threshold = CAVE_BASE - depth * CAVE_DEPTH_FALLOFF;
    if (mountain > MOUNTAIN_THRESHOLD) {
        float t = (mountain - MOUNTAIN_THRESHOLD) / (1.0f - MOUNTAIN_THRESHOLD);
        threshold -= t * CAVE_MOUNTAIN_BONUS;
    }
    if (threshold < CAVE_MIN_THRESHOLD) threshold = CAVE_MIN_THRESHOLD;
    return caveNoise(tx, ty, seed) > threshold;
}

int tileType(int tx, int ty, uint32_t seed) {
    int surface = surfaceHeight(tx, seed);
    bool ocean = surface > SEA_LEVEL;
    if (ty > surface) {
        // Caverna antes do tipo: buraco é ar, não pedra.
        // mask calculada 1x aqui (surfaceHeight já pagou a dela).
        if (isCave(tx, ty, seed, surface, mountainMask(tx, 0, seed))) return 0;
        // Maciço: sempre sólido; tipo cosmético varia por hash.
        float pick = core::rand01(tx, ty, seed ^ 0x9E3779B9u);
        if (pick < 0.15f)      return 1;
        else if (pick < 0.35f) return 2;
        else if (pick < 0.55f) return 3;
        else if (pick < 0.75f) return 4;
        else                   return 5;
    }
    // Topo costeiro vira areia (tipo 6); topo comum continua 4.
    if (ty == surface) return isCoastal(surface) ? 6 : 4;
    // Sem plataformas flutuantes sobre o oceano: céu limpo acima do mar.
    if (ocean) return 0;
    float plat = core::rand01(tx, ty, seed ^ 0x51F37EDu);
    return plat < 0.035f ? 2 : 0; // plataformas esparsas, resto vazio
}

} // namespace support
