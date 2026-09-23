/**
 * @file tests/test_hash.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava hash2 determinístico com negativas por seed.
 * @details Cobre Noise e rand01, roda com make test que compila em build/tests/test_hash.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include "core/Noise.h"

int main() {
    using namespace core;
    // determinismo: mesma entrada, mesma saída
    assert(hash2(10, 20, 1337u) == hash2(10, 20, 1337u));
    assert(rand01(10, 20, 1337u) == rand01(10, 20, 1337u));
    // coordenadas negativas funcionam
    float neg = rand01(-5000, -3, 1337u);
    assert(neg >= 0.0f && neg < 1.0f);
    // seeds diferentes divergem (prob. esmagadora)
    int diff = 0;
    for (int i = 0; i < 100; i++)
        if (rand01(i, 7, 1337u) != rand01(i, 7, 999u)) diff++;
    assert(diff > 90);
    // faixa de rand01
    for (int i = 0; i < 1000; i++) {
        float r = rand01(i, i * 3 - 500, 42u);
        assert(r >= 0.0f && r < 1.0f);
    }
    // valueNoise em [0,1] e suave
    for (int i = 0; i < 500; i++) {
        float n = valueNoise2D(i * 0.13f, 2.7f, 1337u);
        assert(n >= 0.0f && n <= 1.0f);
        float n2 = valueNoise2D(i * 0.13f + 0.01f, 2.7f, 1337u);
        assert(std::fabs(n2 - n) < 0.05f);
    }
    // pontos inteiros do noise batem com rand01 dos cantos
    assert(valueNoise2D(3.0f, 4.0f, 77u) == rand01(3, 4, 77u));
    std::printf("hash test OK (seed=%u hash=%u)\n", 1337u, hash2(10, 20, 1337u));
    return 0;
}
