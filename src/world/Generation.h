/**
 * @file src/world/Generation.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Interface pura da geração de mundo sem SFML ou entidades.
 * @details Declara tileType, ColumnData, máscaras, biomas, árvores, mar e lava com constantes calibradas, incluída por ChunkManager e ChunkLoader.
 */

#pragma once
#include <cstdint>

#include "Tile.h"

namespace support {

// Regras de geração por tile do mundo (puras, sem SFML/Entity).
// Toda função é determinística em (tile, seed): mesmo input, mesmo output,
// inclusive para coords negativas. Salts: terreno usa XOR 0x9E3779B9 e
// 0x51F37ED; camadas novas usam +1009/+2017/+3019 (domínios disjuntos).
// Calibragem do relevo base (o que você mexe para afinar o terreno).
inline constexpr float RELIEF_FREQ = 0.02f;
inline constexpr float RELIEF_Y = 3.7f;
inline constexpr float RELIEF_AMP = 9.0f;
// Alcance do falloff de caverna em tiles (depth satura em 1.0 aqui).
inline constexpr float CAVE_DEPTH_RANGE = 40.0f;
int surfaceHeight(int tx, uint32_t seed);
// IDs em Tile.h (um lugar só). 0 = vazio; demais variam por tabela.
Tile tileType(int tx, int ty, uint32_t seed);
// Tudo que é por-coluna, calculado 1x: superfície, máscara, clima, oceano.
// tileType(tx,ty) sem ColumnData existe só por compat (testes/viz) e
// computa a coluna inline — em chunk quente use sempre a sobrecarga.
struct ColumnData {
    int surface = 0;
    float mountain = 0.0f;
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool ocean = false;
};
ColumnData computeColumn(int tx, uint32_t seed);
Tile tileType(int tx, int ty, uint32_t seed, const ColumnData &col);
// Espessura da terra: constante (não noise). Compartilhada com o viz.
inline constexpr int DIRT_DEPTH = 3;

// Máscara de montanha em [0,1]: ridged multifractal (cristas, não manchas).
// Define ONDE é montanha. Pico raro vem da peakMask abaixo.
inline constexpr float MOUNTAIN_THRESHOLD = 0.35f;
float mountainMask(int tx, int ty, uint32_t seed);

// Máscara de pico em [0,1]: fbm de frequência baixa (/512: blobs de
// centenas de tiles, cobertura uniforme entre seeds). Define ONDE tem
// Everest dentro da cordilheira e ONDE tem monte submarino no oceano.
// Calibrado em linha 1D ty=0 (uso real): >0.75 => 2.8-11% por seed.
// (Em /1024, a seed 1337 não passava de 0.70 nunca: loteria de slice.)
inline constexpr float PEAK_THRESHOLD = 0.75f;
inline constexpr float PEAK_BONUS = 25.0f;
float peakMask(int tx, int ty, uint32_t seed);

// Fossa oceânica: faixa alongada e funda (anisotrópica de propósito).
// Threshold calibrado em uso real 1D: >0.70 => 4-14% das colunas.
// Monte submarino reusa peakMask com threshold próprio.
inline constexpr float TRENCH_THRESHOLD = 0.70f;
inline constexpr int TRENCH_DEPTH_MAX = 20;
inline constexpr float SEAMOUNT_THRESHOLD = 0.70f;
float trenchMask(int tx, int ty, uint32_t seed);

// Cliff: corte vertical raro dentro de montanha. Frequência ALTA em X
// (faixas finas) para não virar planalto. Cria o degrau que o wall-climb
// precisa existir para escalar — sem cliff, montanha é rampa caminhável.
inline constexpr float CLIFF_THRESHOLD = 0.80f;
inline constexpr int CLIFF_HEIGHT = 4;
float cliffMask(int tx, int ty, uint32_t seed);

// Caverna: threshold base calibrado por histograma 2D (3 seeds,
// área 800x200): >0.62 => ~18%, >0.55 => ~34%. Densidade cresce
// com a profundidade via CAVE_DEPTH_FALLOFF (depth clampado em 1,
// senão o Y infinito vira oco).
inline constexpr float CAVE_BASE = 0.62f;
// Falloff fraco de propósito: threshold nunca desce abaixo da mediana
// do noise (~0.49), senão a maioria vira oco (pedra vira exceção).
// 0.06 => fundo plano ~= 0.56 (30%), fundo montanha ~= 0.50 (40%).
inline constexpr float CAVE_DEPTH_FALLOFF = 0.06f;
// Bônus de montanha e piso: pior caso 0.62-0.10-0.12=0.40 seria ~70%
// oco no fundo; o piso segura em 0.45.
inline constexpr float CAVE_MOUNTAIN_BONUS = 0.12f;
inline constexpr float CAVE_MIN_THRESHOLD = 0.45f;
float caveNoise(int tx, int ty, uint32_t seed);
// Guard primeiro: nunca acima de surfaceY + 2. surfaceY e mountainMask
// por parâmetro (não recalcula: 2x por tile em chunk quente).
bool isCave(int tx, int ty, uint32_t seed, int surfaceY, float mountain);

// Bioma, commit 1: só as camadas (tileType não consulta ainda).
// Frequências DIFERENTES de propósito: mesma frequência nas duas
// geraria só a diagonal quente-úmido (biomas correlacionados).
// Salts 3019/4021 livres (em uso: 1009, 2017, XORs, fbm +i*1013).
float temperature(int tx, int ty, uint32_t seed); // features grandes
float humidity(int tx, int ty, uint32_t seed);    // features médias

// Ilha flutuante: plataforma horizontal 4-7 tiles, 6-10 acima da
// superfície local. Uma por grupo de 40 colunas (45% dos grupos têm).
// Determinística por grupo; pontas enterradas somem no morro (natural).
// Nunca sobre o oceano. Substitui o ruído 3.5% que parecia "bloco voando".
bool islandTile(int tx, int ty, uint32_t seed);

// Worms (túneis vermiformes): seguem a iso-linha n~=0.5 do noise,
// gerando túneis conectados em vez de blobs. Aditivo ao blob:
// worm nunca substitui, só acrescenta. Calibrado: width 0.020 =>
// área 5-17%, conectividade 0.77-1.0, bocas 5-8% das colunas.
// Mouth: fração do centro que abre boca (ty em [surface, surface+2]).
inline constexpr float WORM_FREQ = 0.010f;
inline constexpr float WORM_WIDTH = 0.020f;
inline constexpr float WORM_MOUTH_FRAC = 0.5f;
float wormDist(int tx, int ty, uint32_t seed); // |n-0.5|, sem threshold
bool wormCave(int tx, int ty, uint32_t seed);  // dist < width
bool wormMouth(int tx, int ty, uint32_t seed); // centro: abre boca
// Lagos: não carvam nada, pintam ar já carvado. fbm /0.020 (mais blobs
// que /0.008: fração wet>0 estável 9-12% nas 3 seeds em vez de 3-41%).
// Threshold 0.68 calibrado por histograma.
inline constexpr float LAKE_THRESHOLD = 0.68f;
float lakeWet(int tx, int ty, uint32_t seed); // [0,1]: 0 seco, 1 cheio

// Neve no pico: só altitude (surface <= 14 exige uplift alto, que só
// montanha alcança). t alto sem altitude é flanco, não pico.
bool snowcap(int surface);

// Minérios: Tile do veio ou Air (sem minério). Faixas ABSOLUTAS de ty
// (um veio por estrato: identidade por profundidade). Ordem de checagem
// não importa (faixas disjuntas). Diamond/Coal viraram legado: continuam
// no enum/blocos, mas não geram mais.
// Thresholds herdados do esquema antigo (ordens de grandeza medidas).
Tile pickOre(int tx, int ty, uint32_t seed, int surface, int depth);
// Variedade visual: base rock + flavors do estrato (registry).
// Ore tem prioridade (só roda quando pickOre deu Air).
Tile strataRock(int tx, int ty, uint32_t seed);
// Spawn: primeira coluna a partir de nearX com flanco de montanha
// (uplift 8..20: terra garantida, nem mar nem pico). Determinístico.
// Player cai do céu até a superfície — qualquer seed funciona.
int findSpawnTileX(int nearX, uint32_t seed);

// Bioma: tabela de dupla entrada temperatura x umidade.
// Thresholds = tercis globais medidos em 3 seeds (cada quadrante 8.5%+):
// temp p33≈0.42/p66≈0.55, humid p33≈0.43/p66≈0.57. Fronteira estrita '>'
// (== cai na faixa de baixo).
inline constexpr float T_HOT = 0.55f;
inline constexpr float T_COLD = 0.42f;
inline constexpr float H_DRY = 0.43f;
inline constexpr float H_WET = 0.57f;

enum class Biome { Ocean, Beach, Desert, Savanna, Grassland, Forest, Taiga, Tundra, COUNT };

// Clima do tile (tx,ty); ocean/coastal têm precedência (mar virou bioma).
Biome pickBiome(float temp, float humid, bool ocean, bool coastal);
// Tile de topo para o bioma (subsolo não muda).
Tile biomeTopTile(Biome b);

// Árvores decorativas (sem colisão, sem dano — Fase C decide o resto).
// Uma por grupo de colunas; tronco + copa elíptica; nunca enterra sólido
// (quem estampa verifica Air). TREE_SPACING exported p/ range expandido.
inline constexpr int TREE_SPACING = 4;
inline constexpr int TREE_TRUNK_MIN = 4;
inline constexpr int TREE_TRUNK_MAX = 7;
inline constexpr int TREE_CANOPY_MIN = 2;
inline constexpr int TREE_CANOPY_MAX = 3;
inline constexpr int TREE_CANOPY_R_MAX = 3;
inline constexpr int TREE_EXPAND = TREE_CANOPY_R_MAX + TREE_SPACING;
struct TreeParams { int trunkH = 0; int canopyR = 0; };
// Puro e mockável: surfaceY e bioma vêm de fora (teste injeta plano).
// Inclinação usa surfaceHeight real dos vizinhos.
bool treeWants(int tx, uint32_t seed, int surfaceY, Biome biome, TreeParams &out);
// Lava e fundo do mundo. Caverna vira lava SÓ em isLavaDepth (estrato 6+):
// estender o fundo sem isso inundaria tudo de lava a partir de ty=51.
// Abaixo de WORLD_BOTTOM, tudo é bedrock (nada cava, nada é gerado).
// Água de caverna só perto do mar (WATER_FILL_MAX): fundo inundado
// também não faz sentido — caverna funda é ar (ou lava no estrato 6+).
// Mar: linha do mapa (tile row). Histograma (3 seeds x 4000 cols):
// L=26 => 14-21% de colunas oceânicas. Só decide o que fica entre
// surf e SEA_LEVEL.
inline constexpr int SEA_LEVEL = 26;
inline constexpr int WATER_FILL_MAX = SEA_LEVEL + 12;
inline constexpr int LAVA_DEPTH_START = 6200; // entrada do estrato 6
// Poça, não oceano: lava só onde lakeWet passa do limiar (opção B —
// mais raro que água; hazard, não decoração). Medido: wet>0.15 em
// caverna funda ≈ 8% das cavernas ⇒ ~3.3% da faixa (poças coerentes,
// não pimenta). Piso do test_lava reflete o modelo novo, não o antigo.
inline constexpr float LAVA_WET_THRESHOLD = 0.15f;
inline constexpr int WORLD_BOTTOM = 12000;   // ~1h de descida
static_assert(LAVA_DEPTH_START > WATER_FILL_MAX, "lava e água não podem se cruzar");
static_assert(WORLD_BOTTOM > LAVA_DEPTH_START, "fundo abaixo da lava");
inline bool isLavaDepth(int ty) { return ty >= LAVA_DEPTH_START; }
// Coluna oceânica = terreno abaixo da linha do mar.
inline bool isOceanColumn(int tx, uint32_t seed) {
    return surfaceHeight(tx, seed) > SEA_LEVEL;
}
// Costa = superfície a ±3 tiles do nível do mar (topo vira areia).
inline bool isCoastal(int surfaceY) {
    int d = surfaceY > SEA_LEVEL ? surfaceY - SEA_LEVEL : SEA_LEVEL - surfaceY;
    return d <= 3;
}

} // namespace support
