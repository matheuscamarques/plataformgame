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
