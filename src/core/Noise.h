/**
 * @file src/core/Noise.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Ruído determinístico por posição para geração infinita do mundo.
 * @details Oferece hash2, rand01, valueNoise2D e fbm sem estado global, usado por geração de terreno e cavernas.
 */

#pragma once

#include <cmath>
#include <cstdint>

namespace core {

// Geração determinística por posição (base do mundo infinito).
// A mesma (tx, ty, seed) sempre devolve o mesmo valor, sem estado global,
// então qualquer chunk pode ser gerado sob demanda sem gerar os vizinhos.
// Tudo puro e header-only; camadas novas usam salts +1009/+2017/+3019
// (os salts XOR 0x9E3779B9/0x51F37ED já estão em uso pelo terreno).
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

// Value noise 2D suave em [0, 1] com interpolação bilinear.
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

// Fractal Brownian Motion: várias oitavas de value noise, retorna [0, 1].
inline float fbm(float x, float y, uint32_t seed, int octaves = 4) {
    float sum = 0.0f, amp = 0.5f, freq = 1.0f, norm = 0.0f;
    for (int i = 0; i < octaves; ++i) {
        sum += valueNoise2D(x * freq, y * freq, seed + static_cast<uint32_t>(i) * 1013u) * amp;
        norm += amp;
        amp *= 0.5f;
        freq *= 2.0f;
    }
    return sum / norm;
}

// Ridged multifractal: manchas viram cristas ramificadas com picos
// afiados (montanhas de verdade, não colinas). Cada oitava vira crista
// (1-|2n-1|), afina (^2), e detalhe fino só nasce onde já há crista (n*prev).
inline float ridgedFbm(float x, float y, uint32_t seed, int octaves = 5) {
    float sum = 0.0f, amp = 1.0f, freq = 1.0f, prev = 1.0f, norm = 0.0f;
    for (int i = 0; i < octaves; ++i) {
        float n = valueNoise2D(x * freq, y * freq, seed + static_cast<uint32_t>(i) * 1013u);
        n = 1.0f - std::fabs(2.0f * n - 1.0f);
        n = n * n;
        n *= prev;
        sum += n * amp;
        norm += amp;
        prev = n;
        amp *= 0.5f;
        freq *= 2.0f;
    }
    return norm > 0.0f ? sum / norm : 0.0f;
}

} // namespace core
