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
    float relief = core::valueNoise2D(tx * RELIEF_FREQ, RELIEF_Y, seed); // [0,1] suave
    int base = SURFACE_MID - 4 + static_cast<int>(relief * RELIEF_AMP);
    float m = mountainMask(tx, 0, seed);
    if (m > MOUNTAIN_THRESHOLD) {
        // Base comum: morro/montanha, 0-20 tiles.
        float t = (m - MOUNTAIN_THRESHOLD) / (1.0f - MOUNTAIN_THRESHOLD);
        base -= static_cast<int>(t * 20.0f);
        // Bônus de pico: raro, cúbico — só o extremo da cauda conta.
        // 1 Everest cercado de 6000m, não drama em toda crista.
        float p = peakMask(tx, 0, seed);
        if (p > PEAK_THRESHOLD) {
            float pn = (p - PEAK_THRESHOLD) / (1.0f - PEAK_THRESHOLD);
            base -= static_cast<int>(pn * pn * pn * PEAK_BONUS);
        }
        // Vales entre cristas não viram oceano interno.
        if (base > SEA_LEVEL + 2) base = SEA_LEVEL + 2;
        // Cliff: desce um degrau abrupto (parede lisa vertical).
        // É o que transforma rampa caminhável em obstáculo de wall-climb.
        if (cliffMask(tx, 0, seed) > CLIFF_THRESHOLD) {
            base -= CLIFF_HEIGHT;
        }
    }
    // Fossa primeiro (aprofunda), monte depois (sobe). Ordem fixa:
    // invertida, o monte cavaria dentro da fossa.
    // Só em oceano fora de montanha: em vale de range, a fossa furaria
    // o clamp e criaria oceano interno. Rampa shore 0..1: sem ela, a
    // coluna com base==SEA_LEVEL+1 ganharia +20 de uma vez na costa.
    if (base > SEA_LEVEL && m <= MOUNTAIN_THRESHOLD) {
        float shore = (base - SEA_LEVEL) / 4.0f;
        if (shore > 1.0f) shore = 1.0f;
        float tr = trenchMask(tx, 0, seed);
        if (tr > TRENCH_THRESHOLD) {
            float tt = (tr - TRENCH_THRESHOLD) / (1.0f - TRENCH_THRESHOLD);
            // t^1.2: fundo mais chato que pico tem topo.
            base += static_cast<int>(std::pow(tt, 1.2f) * TRENCH_DEPTH_MAX * shore);
        }
        float p = peakMask(tx, 0, seed);
        if (p > SEAMOUNT_THRESHOLD) {
            float pn = (p - SEAMOUNT_THRESHOLD) / (1.0f - SEAMOUNT_THRESHOLD);
            base -= static_cast<int>(pn * pn * 10.0f * shore); // sobe até 10, nunca emerge
            if (base < SEA_LEVEL + 2) base = SEA_LEVEL + 2;
        }
    }
    return base;
}

float mountainMask(int tx, int ty, uint32_t seed) {
    return core::ridgedFbm(tx / 384.0f, ty / 384.0f, seed + 1009u, 5);
}

float peakMask(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 512.0f, ty / 512.0f, seed + 7079u, 3);
}

float trenchMask(int tx, int ty, uint32_t seed) {
    // Escala de lagoa: a decisão (surfaceHeight) usa a linha ty=0, e em
    // /1024 a fossa mais próxima ficava a 2000+ tiles em 2/3 seeds;
    // em /384 a taxa efetiva variava 0-3% (anti-correlação com oceano
    // por mesma ordem de grandeza). Em /128 a taxa efetiva é 1.4-1.8%
    // estável nas 3 seeds. Y anisotrópico só aparece no PNG.
    return core::fbm(tx / 128.0f, ty / 192.0f, seed + 8081u, 3);
}

float cliffMask(int tx, int ty, uint32_t seed) {
    // Alta em X (cortes finos), baixa em Y (não espalha na vertical).
    // Seed 9091 livre (em uso: 1009/2017/3019/4021/6067/7079/8081/XORs).
    return core::fbm(tx / 48.0f, ty / 512.0f, seed + 9091u, 2);
}

float caveNoise(int tx, int ty, uint32_t seed) {
    return core::fbm(tx / 64.0f, ty / 64.0f, seed + 2017u, 3);
}

float wormDist(int tx, int ty, uint32_t seed) {
    // Salt 7121 livre (em uso: 1009/2017/3019/4021/6067/7079/8081/9091/XORs).
    float n = core::fbm(tx * WORM_FREQ, ty * WORM_FREQ, seed + 7121u, 2);
    return std::fabs(n - 0.5f);
}

bool wormCave(int tx, int ty, uint32_t seed) {
    return wormDist(tx, ty, seed) < WORM_WIDTH;
}

bool wormMouth(int tx, int ty, uint32_t seed) {
    return wormDist(tx, ty, seed) < WORM_WIDTH * WORM_MOUTH_FRAC;
}

float lakeWet(int tx, int ty, uint32_t seed) {
    // fbm 2D direto (não por coluna): lago acompanha o relevo do noise.
    float n = core::fbm(tx * 0.020f, ty * 0.020f, seed + 7151u, 3);
    float t = (n - LAKE_THRESHOLD) / 0.10f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return t;
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
    // Vão livre: mínima e máxima do terreno no span. Ilha paira acima
    // do ponto mais alto (nunca enterrada, nunca fragmentada) e some
    // se encostar no mar. Altura de pulo (~5 tiles): ilha a 3-4 do chão
    // é alcançável; a 6+ vira decoração inalcançável ("voando").
    // Desnível > 4 no vão também some (flutuaria alto sobre o vale).
    // Tudo-ou-nada por ilha, sem fragmentos.
    int sMin = 1000000, sMax = -1000000;
    for (int x = ox; x < ox + w; x++) {
        int s = surfaceHeight(x, seed);
        if (s < sMin) sMin = s;
        if (s > sMax) sMax = s;
    }
    if (sMax > SEA_LEVEL) return false; // céu limpo sobre o mar
    if (sMax - sMin > 4) return false; // vão desigual: voaria alto
    int hy = sMin - 3 - int(core::rand01(g, 37, seed ^ SALT) * 2.0f);
    return ty == hy;
}

bool snowcap(int surface) {
    // Só altitude: surface <= 14 exige uplift 15+ (só pico chega lá).
    return surface <= 14;
}

namespace {
// Densidade por bioma (fração de colunas elegíveis que tentam árvore).
float treeDensity(Biome b) {
    switch (b) {
        case Biome::Forest:    return 0.35f;
        case Biome::Taiga:     return 0.28f;
        case Biome::Savanna:   return 0.05f;
        case Biome::Grassland: return 0.03f;
        default:               return 0.0f; // oceano/praia/deserto/tundra
    }
}
} // namespace

bool treeWants(int tx, uint32_t seed, int surfaceY, Biome biome, TreeParams &out) {
    float density = treeDensity(biome);
    if (density <= 0.0f) return false;
    // Salt 0x7A11*: disjunto dos demais (checados: todos os +N e XORs).
    if (core::rand01(tx, 0x7A11, seed) >= density) return false;
    // Inclinação: vizinhos reais, não o parâmetro (mock plano passa).
    if (std::abs(surfaceHeight(tx - 1, seed) - surfaceY) > 1) return false;
    if (std::abs(surfaceHeight(tx + 1, seed) - surfaceY) > 1) return false;
    int trunkRange = TREE_TRUNK_MAX - TREE_TRUNK_MIN + 1;
    out.trunkH = TREE_TRUNK_MIN + int(core::rand01(tx, 0x7A12, seed) * trunkRange) % trunkRange;
    if (biome == Biome::Taiga) out.trunkH += 2; // conífera mais alta
    int canopyRange = TREE_CANOPY_MAX - TREE_CANOPY_MIN + 1;
    out.canopyR = TREE_CANOPY_MIN + int(core::rand01(tx, 0x7A13, seed) * canopyRange) % canopyRange;
    return true;
}

Tile pickOre(int tx, int ty, uint32_t seed, int surface, int depth) {
    (void)surface;
    (void)depth;
    // Um veio por estrato, faixas absolutas disjuntas (ver Stratum.h).
    // r uniforme por tile; thresholds ~1.5-4.5% da pedra do estrato.
    float r = core::rand01(tx, ty, seed + 6067u);
    if (ty >= 200 && ty < 1400) {
        if (r > 0.955f) return Tile::OreCopper;
    } else if (ty >= 1400 && ty < 2600) {
        if (r > 0.980f) return Tile::OreIron;
    } else if (ty >= 2600 && ty < 3800) {
        if (r > 0.996f) return Tile::OreSilver;
    } else if (ty >= 3800 && ty < 5000) {
        if (r > 0.990f) return Tile::OreGold;
    } else if (ty >= 5000 && ty < 6200) {
        if (r > 0.982f) return Tile::OreCrystal;
    } else if (ty >= 6200 && ty < 7400) {
        if (r > 0.984f) return Tile::OrePlatinum;
    } else if (ty >= 7400 && ty < 8600) {
        if (r > 0.986f) return Tile::OreMithril;
    } else if (ty >= 8600 && ty < 9800) {
        if (r > 0.988f) return Tile::OreAdamant;
    }
    return Tile::Air;
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

Tile biomeTopTile(Biome b) {
    switch (b) {
        case Biome::Ocean:     return Tile::Sand; // fundo do mar: areia
        case Biome::Beach:     return Tile::Sand;
        case Biome::Desert:    return Tile::Sand;
        case Biome::Savanna:   return Tile::SavannaTop;
        case Biome::Grassland: return Tile::Grass; // terra com grama
        case Biome::Forest:    return Tile::Grass; // terra com grama
        case Biome::Taiga:     return Tile::TaigaTop;
        case Biome::Tundra:    return Tile::TundraTop;
        default:               return Tile::FallbackTop;
    }
}

bool isCave(int tx, int ty, uint32_t seed, int surfaceY, float mountain) {
    // Boca primeiro: centro do verme abre passagem na faixa do guard.
    // Fora do centro, o guard protege (nunca perto da superfície).
    if (ty < surfaceY + 3) return wormMouth(tx, ty, seed);
    // Verme soma ao blob (nunca substitui): ou escava, ou deixa quieto.
    if (wormCave(tx, ty, seed)) return true;
    // Divisor 40 (não 25): falloff satura devagar; threshold fica acima
    // da mediana (~0.49) em todo lugar — pedra continua sendo a maioria.
    float depth = float(ty - surfaceY) / CAVE_DEPTH_RANGE;
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

Tile tileType(int tx, int ty, uint32_t seed) {
    return tileType(tx, ty, seed, computeColumn(tx, seed));
}

// Ar vira água abaixo do nível do mar (banda do oceano + cavernas fundas
// inundadas). Um lugar só — ChunkManager não decide mais água.
static Tile airOrWater(int ty) {
    return ty >= SEA_LEVEL ? Tile::Water : Tile::Air;
}

ColumnData computeColumn(int tx, uint32_t seed) {
    ColumnData col;
    col.surface = surfaceHeight(tx, seed);
    col.mountain = mountainMask(tx, 0, seed);
    col.temperature = temperature(tx, col.surface, seed);
    col.humidity = humidity(tx, col.surface, seed);
    col.ocean = col.surface > SEA_LEVEL;
    return col;
}

Tile tileType(int tx, int ty, uint32_t seed, const ColumnData &col) {
    // Bedrock vence tudo. Nenhuma camada cava aqui.
    if (ty >= WORLD_BOTTOM) return Tile::Bedrock;

    int surface = col.surface;
    bool ocean = col.ocean;
    if (ty > surface) {
        // Ordem sagrada: caverna vence terra e pedra. Se isCave, é ar
        // mesmo dentro da zona de terra.
        if (isCave(tx, ty, seed, surface, col.mountain)) {
            // Caverna funda vira lava, não ar. Com +5 de folga para a
            // boca não abrir direto na lava. Acima do surface, nunca.
            if (isLavaDepth(ty) && ty > surface + 5) return Tile::Lava;
            // Lago pinta caverna já carvada em qualquer profundidade
            // (poça subterrânea funda é feature, não bug).
            if (lakeWet(tx, ty, seed) > 0.0f) return Tile::Water;
            // Fora isso, água só perto do mar: caverna funda seca é ar.
            if (ty >= SEA_LEVEL && ty <= WATER_FILL_MAX) return Tile::Water;
            return Tile::Air;
        }
        if (ty <= surface + DIRT_DEPTH) {
            // Sob o oceano, areia continua areia (praia não vira terra).
            if (ocean) return Tile::Sand;
            return Tile::Dirt; // terra
        }
        // Pedra com minérios (raridade por profundidade, ordem do raro).
        Tile ore = pickOre(tx, ty, seed, surface, ty - surface);
        return ore != Tile::Air ? ore : Tile::Stone;
    }
    // Topo: boca de verme abre passagem (resto do guard protege).
    // Neve depois da boca: pico com entrada mostra a entrada.
    // Clima por último.
    if (ty == surface) {
        // Boca no topo: buraco. Em oceano é água (submerso), em terra é ar.
        if (isCave(tx, ty, seed, surface, col.mountain))
            return ocean ? Tile::Water : Tile::Air;
        if (snowcap(surface)) return Tile::Snow;
        Biome b = pickBiome(col.temperature, col.humidity,
                            ocean, isCoastal(surface));
        return biomeTopTile(b);
    }
    // Ilha flutuante coerente (ou nada): sem ruído de tile isolado.
    if (ocean) return airOrWater(ty);
    return islandTile(tx, ty, seed) ? Tile::IslandPlatform : airOrWater(ty);
}

} // namespace support
