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
        // Fórmula real: base [21,29] - t*20 (0..20) - p^3*25 (0..25).
        assert(s >= 21 - 20 - 25 && s <= 29);
        // Clamp explícito: em zona de montanha nunca abaixo de SEA+2.
        if (mountainMask(j, 0, seed) > MOUNTAIN_THRESHOLD) assert(s <= SEA_LEVEL + 2);
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
            if (i == s) {
                // topo decide por bioma (areia continua 6 na costa),
                // ou neve 8 nos picos altos e fortes.
                bool ocean = isOceanColumn(j, seed);
                Biome b = pickBiome(temperature(j, s, seed), humidity(j, s, seed),
                                    ocean, isCoastal(s));
                int expected = snowcap(s) ? 8 : biomeTopTile(b);
                assert(t == expected);
                if (isCoastal(s)) assert(t == 6);
            }
            if (i < s) assert(t == 0 || t == 2);          // ar ou plataforma
            assert(tileType(j, i, seed) == t);            // determinístico
        }
    }
    std::printf("surface+montanha OK: faixa=[%d,%d] maxStep=%d\n", lo, hi, maxStep);
    assert(maxStep <= 4); // montanha íngreme, mas sem paredão
    return 0;
}
