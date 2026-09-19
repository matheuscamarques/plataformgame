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

} // namespace support
