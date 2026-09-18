#pragma once

#include <cmath>
#include <cstdint>

// Geração determinística por posição (base do mundo infinito).
// A mesma (tx, ty, seed) sempre devolve o mesmo valor, sem estado global,
// então qualquer chunk pode ser gerado sob demanda sem gerar os vizinhos.

inline uint32_t hash2(int tx, int ty, uint32_t seed) {
    uint32_t h = seed
        + static_cast<uint32_t>(tx) * 374761393u
        + static_cast<uint32_t>(ty) * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

// Float em [0, 1) determinístico para o tile (tx, ty).
inline float rand01(int tx, int ty, uint32_t seed) {
    return (hash2(tx, ty, seed) & 0xFFFFFFu) / float(0x1000000);
}

// Value noise 2D suave em [0, 1] (para terreno/cavernas na Fase 5).
inline float valueNoise2D(float x, float y, uint32_t seed) {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - xi;
    float yf = y - yi;
    float u = xf * xf * (3.0f - 2.0f * xf);
    float v = yf * yf * (3.0f - 2.0f * yf);
    float a = rand01(xi, yi, seed);
    float b = rand01(xi + 1, yi, seed);
    float c = rand01(xi, yi + 1, seed);
    float d = rand01(xi + 1, yi + 1, seed);
    return a + (b - a) * u + (c - a) * v + (a - b - c + d) * u * v;
}
