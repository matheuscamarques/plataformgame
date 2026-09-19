#pragma once
#include <cstdint>

namespace support {

// Regras de geração por tile do mundo (puras, sem SFML/Entity).
// Toda função é determinística em (tile, seed): mesmo input, mesmo output,
// inclusive para coords negativas. Salts: terreno usa XOR 0x9E3779B9 e
// 0x51F37ED; camadas novas usam +1009/+2017/+3019 (domínios disjuntos).
int surfaceHeight(int tx, uint32_t seed);
int tileType(int tx, int ty, uint32_t seed); // 0 = vazio, 1..5 = sólido

// Máscara de montanha em [0,1]: manchas orgânicas via fbm.
// Onde > MOUNTAIN_THRESHOLD, a superfície sobe (commit 1) e a caverna
// densifica (commit 3). Limiar calibrado por histograma: 3 seeds x 4000
// colunas => fração 0.19-0.41. Não chutar threshold sem medir.
// Varia em x (cordilheiras); ty existe para reuso 2D futuro.
inline constexpr float MOUNTAIN_THRESHOLD = 0.56f;
float mountainMask(int tx, int ty, uint32_t seed);

// Caverna: threshold base calibrado por histograma 2D (3 seeds,
// área 800x200): >0.62 => ~18%, >0.55 => ~34%. Densidade cresce
// com a profundidade via CAVE_DEPTH_FALLOFF (depth clampado em 1,
// senão o Y infinito vira oco).
inline constexpr float CAVE_BASE = 0.62f;
inline constexpr float CAVE_DEPTH_FALLOFF = 0.10f;
float caveNoise(int tx, int ty, uint32_t seed);
// Guard primeiro: nunca acima de surfaceY + 2. surfaceY por parâmetro
// (não recalcula surfaceHeight: 2x por tile em chunk quente).
bool isCave(int tx, int ty, uint32_t seed, int surfaceY);

} // namespace support
