#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "world/hash.h"
int main() {
    const int m = 50;
    uint32_t seed = 1337u;
    int prev = -100, maxStep = 0, solidBelow = 0, checked = 0;
    for (int j = -200; j < 1200; j++) {
        float relief = valueNoise2D(j * 0.02f, 3.7f, seed);
        assert(relief >= 0.0f && relief <= 1.0f);
        int surface = m / 2 - 4 + static_cast<int>(relief * 9.0f);
        assert(surface >= m/2-4 && surface <= m/2+5);
        if (prev != -100) {
            int step = std::abs(surface - prev);
            if (step > maxStep) maxStep = step;
            assert(step <= 1); // chão contínuo: degrau de no máximo 1 tile
        }
        prev = surface;
        for (int i = surface + 1; i < m; i++) { solidBelow++; checked++; }
    }
    assert(checked == solidBelow); // tudo abaixo da superfície é sólido
    std::printf("surface OK: maxStep=%d cols j=[-200,1200)\n", maxStep);
    return 0;
}
