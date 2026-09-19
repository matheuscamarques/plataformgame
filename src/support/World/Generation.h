#pragma once
#include <cstdint>

namespace support {

// Regras de geração por tile do mundo (puras, sem SFML/Entity).
// Toda função é determinística em (tile, seed): mesmo input, mesmo output,
// inclusive para coords negativas. Montanhas/cavernas entram aqui depois.
int surfaceHeight(int tx, uint32_t seed);
int tileType(int tx, int ty, uint32_t seed); // 0 = vazio, 1..5 = sólido

} // namespace support
