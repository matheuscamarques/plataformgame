#include "Generation.h"

#include <cmath>
#include <initializer_list>

#include "../../core/Noise.h"
#include "ChunkKey.h"

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

bool islandTile(int tx, int ty, uint32_t seed) {
    const int G = 40;
    const uint32_t SALT = 0x6A09E667u; // bits de pi; disjunto dos demais salts
    int g = floorDiv(tx, G);
    if (core::rand01(g, 7, seed ^ SALT) > 0.45f) return false;
    int ox = g * G + int(core::rand01(g, 13, seed ^ SALT) * 24.0f); // 0..23
    int w = 4 + int(core::rand01(g, 29, seed ^ SALT) * 4.0f);       // 4..7
    if (tx < ox || tx >= ox + w) return false;
    int cx = ox + w / 2;
    if (isOceanColumn(cx, seed)) return false; // céu limpo sobre o mar
    int hy = surfaceHeight(cx, seed) - 6 - int(core::rand01(g, 37, seed ^ SALT) * 5.0f);
    return ty == hy;
}

bool snowcap(int tx, uint32_t seed, int surface) {
    float m = mountainMask(tx, 0, seed);
    float t = (m - MOUNTAIN_THRESHOLD) / (1.0f - MOUNTAIN_THRESHOLD);
    return t > 0.65f && surface <= 14;
}

int findSpawnTileX(int nearX, uint32_t seed) {
    // Flanco de montanha: uplift 8..20 (terra garantida: s <= 21 < mar).
    for (int d = 0; d <= 2000; d++) {
        for (int tx : {nearX + d, nearX - d}) {
            int up = 29 - surfaceHeight(tx, seed);
            if (up >= 8 && up <= 20) return tx;
        }
    }
    return nearX; // fallback: nunca acontece nas seeds medidas
}

Biome pickBiome(float temp, float humid, bool ocean, bool coastal) {
    if (ocean) return Biome::Ocean;
    if (coastal) return Biome::Beach;
    if (temp > T_HOT) {
        if (humid < H_DRY) return Biome::Desert;
        if (humid < H_WET) return Biome::Savanna;
        return Biome::Forest;
    }
    if (temp > T_COLD) {
        if (humid < H_DRY) return Biome::Grassland;
        return Biome::Forest;
    }
    if (humid < H_DRY) return Biome::Tundra;
    return Biome::Taiga;
}

int biomeTopTile(Biome b) {
    switch (b) {
        case Biome::Ocean:     return 6; // fundo do mar: areia
        case Biome::Beach:     return 6;
        case Biome::Desert:    return 6;
        case Biome::Savanna:   return 5;
        case Biome::Grassland: return 9; // terra com grama
        case Biome::Forest:    return 9; // terra com grama
        case Biome::Taiga:     return 3;
        case Biome::Tundra:    return 1;
        default:               return 4;
    }
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
    // Topo: clima da coluna na altura da superfície (não do tile fundo).
    // Neve primeiro: pico alto e forte passa na frente do bioma.
    if (ty == surface) {
        if (snowcap(tx, seed, surface)) return 8;
        Biome b = pickBiome(temperature(tx, surface, seed),
                            humidity(tx, surface, seed),
                            ocean, isCoastal(surface));
        return biomeTopTile(b);
    }
    // Ilha flutuante coerente (ou nada): sem ruído de tile isolado.
    if (ocean) return 0;
    return islandTile(tx, ty, seed) ? 2 : 0;
}

} // namespace support
