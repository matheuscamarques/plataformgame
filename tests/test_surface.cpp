#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "core/Noise.h"
#include "support/World/Generation.h"

int main() {
    using namespace core;
    using namespace support;
    uint32_t seed = 1337u;

    // máscara em [0,1]
    for (int j = -500; j < 500; j++) {
        float m = mountainMask(j, 0, seed);
        assert(m >= 0.0f && m <= 1.0f);
    }

    int prev = -1000, maxStep = 0, lo = 1000, hi = -1000;
    for (int j = -1200; j < 1200; j++) {
        int s = surfaceHeight(j, seed);
        // base [21,30] menos até 18 de montanha
        assert(s >= 25 - 4 - 18 && s <= 25 + 5);
        if (s < lo) lo = s;
        if (s > hi) hi = s;
        if (prev != -1000) {
            int step = std::abs(s - prev);
            if (step > maxStep) maxStep = step;
        }
        prev = s;
        // coerência tileType x superfície
        for (int i = s - 2; i <= s + 2; i++) {
            int t = tileType(j, i, seed);
            if (i > s) assert(t != 0);                    // maciço sempre sólido
            if (i == s) assert(t == 4);                   // topo sempre 4
            if (i < s) assert(t == 0 || t == 2);          // ar ou plataforma
            assert(tileType(j, i, seed) == t);            // determinístico
        }
    }
    std::printf("surface+montanha OK: faixa=[%d,%d] maxStep=%d\n", lo, hi, maxStep);
    assert(maxStep <= 4); // montanha íngreme, mas sem paredão
    return 0;
}
