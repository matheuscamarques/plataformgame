/**
 * @file src/core/Random.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Gerador global para efeitos visuais fora da gameplay.
 * @details Expõe rng, seedRng e randRange com mt19937, usado por partículas e spawn, gameplay usa Noise.h.
 */

#pragma once
#include <random>

namespace core {

// RNG global para efeitos (partículas, spawn). Lógica de gameplay usa
// Noise.h determinístico por seed — nunca isto.
inline std::mt19937& rng() {
    static std::mt19937 r{12345u};
    return r;
}

inline void seedRng(unsigned int s) { rng().seed(s); }

inline float randRange(float lo, float hi) {
    std::uniform_real_distribution<float> d(lo, hi);
    return d(rng());
}

} // namespace core
