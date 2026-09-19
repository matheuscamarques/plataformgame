#pragma once
#include <cstdint>

namespace support {

// Regras de geração por tile do mundo (puras, sem SFML/Entity).
// Toda função é determinística em (tile, seed): mesmo input, mesmo output,
// inclusive para coords negativas. Salts: terreno usa XOR 0x9E3779B9 e
// 0x51F37ED; camadas novas usam +1009/+2017/+3019 (domínios disjuntos).
int surfaceHeight(int tx, uint32_t seed);
// 0 = vazio, 1..11 = sólido (6 areia, 8 neve, 9 grama, 10 terra, 11 pedra).
int tileType(int tx, int ty, uint32_t seed);
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
int tileType(int tx, int ty, uint32_t seed, const ColumnData &col);
// IDs: 1..5 legado cosmético, 6 areia, 8 neve, 9 grama, 10 terra, 11 pedra.
// Grama ocupou o 9 primeiro (pedido anterior); terra/pedra seguem 10/11
// para nunca reutilizar ID existente.
// Espessura da terra: constante (não noise). Compartilhada com o viz.
inline constexpr int DIRT_DEPTH = 3;

// Máscara de montanha em [0,1]: ridged multifractal (cristas, não manchas).
// Define ONDE é montanha. Pico raro vem da peakMask abaixo.
inline constexpr float MOUNTAIN_THRESHOLD = 0.35f;
float mountainMask(int tx, int ty, uint32_t seed);

// Máscara de pico em [0,1]: fbm de frequência MUITO baixa. Define ONDE
// tem Everest dentro da cordilheira. Só p>0.80 conta, e p³ garante que
// só o extremo da cauda ganha altura: ~0.2-0.5% das montanhas têm pico.
inline constexpr float PEAK_THRESHOLD = 0.80f;
inline constexpr float PEAK_BONUS = 25.0f;
float peakMask(int tx, int ty, uint32_t seed);

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

// Neve no pico: só altitude (surface <= 10 exige uplift 11+, que só
// montanha alcança). t alto sem altitude é flanco, não pico.
bool snowcap(int surface);

// Spawn: primeira coluna a partir de nearX com flanco de montanha
// (uplift 8..20: terra garantida, nem mar nem pico). Determinístico.
// Player cai do céu até a superfície — qualquer seed funciona.
int findSpawnTileX(int nearX, uint32_t seed);

// Bioma: tabela de dupla entrada temperatura x umidade.
// Thresholds = tercis globais medidos em 3 seeds (cada quadrante 8.5%+):
// temp p33≈0.42/p66≈0.55, humid p33≈0.43/p66≈0.57. Fronteira estrita '>'
// (== cai na faixa de baixo). Sem Snow: picos nevados ficam p/ depois.
inline constexpr float T_HOT = 0.55f;
inline constexpr float T_COLD = 0.42f;
inline constexpr float H_DRY = 0.43f;
inline constexpr float H_WET = 0.57f;

enum class Biome { Ocean, Beach, Desert, Savanna, Grassland, Forest, Taiga, Tundra, COUNT };

// Clima do tile (tx,ty); ocean/coastal têm precedência (mar virou bioma).
Biome pickBiome(float temp, float humid, bool ocean, bool coastal);
// Tile de topo para o bioma (subsolo não muda).
int biomeTopTile(Biome b);
// Mar: linha do mapa (tile row), não valor de noise. Calibrado por
// histograma (3 seeds x 4000 cols, fração com surface > L):
// L=25 => 29-41%, L=26 => 14-21%, L=27 => 8-10%. L=26 escolhido:
// banda oceânica (26..surf) cai toda na regra de água legada (ty > 25),
// sem buraco de ar na superfície da água. Não modifica altura nenhuma:
// só decide o que fica entre surf e SEA_LEVEL.
inline constexpr int SEA_LEVEL = 26;
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
