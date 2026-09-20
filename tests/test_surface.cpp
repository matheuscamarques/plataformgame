#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include "core/Noise.h"
#include "world/Generation.h"
#include "world/Block.h"

int main() {
    using namespace core;
    using namespace support;
    uint32_t seed = 1337u;

    // máscara em [0,1]
    for (int j = -500; j < 500; j++) {
        float m = mountainMask(j, 0, seed);
        assert(m >= 0.0f && m <= 1.0f);
    }

    int lo = 1000, hi = -1000;
    int mxPlain = 0, mxCliff = 0, cliffCols = 0;
    // Degraus por zona, 3 seeds, range largo (cliff é raro: 0.5-2.5%).
    for (uint32_t sd : {1337u, 999u, 42u}) {
        int pprev = -100000;
        for (int j = -4000; j < 4000; j++) {
            int s = surfaceHeight(j, sd);
            // Fórmula real: base [21,29] - t*20 - p^3*25, +fossa 20.
            assert(s >= 21 - 20 - 25 && s <= 29 + TRENCH_DEPTH_MAX);
            // Clamp explícito: em zona de montanha nunca abaixo de SEA+2.
            if (mountainMask(j, 0, sd) > MOUNTAIN_THRESHOLD) assert(s <= SEA_LEVEL + 2);
            bool clf = mountainMask(j, 0, sd) > MOUNTAIN_THRESHOLD &&
                       cliffMask(j, 0, sd) > CLIFF_THRESHOLD;
            if (clf) cliffCols++;
            if (s < lo) lo = s;
            if (s > hi) hi = s;
            if (pprev != -100000) {
                int step = std::abs(s - pprev);
                if (clf) { if (step > mxCliff) mxCliff = step; }
                else if (step > mxPlain) mxPlain = step;
            }
            pprev = s;
        }
    }
    std::printf("degraus: plano=%d cliff=%d (cliff cols=%d)\n", mxPlain, mxCliff, cliffCols);
    assert(mxPlain <= 4); // rampa caminhável fora de cliff
    assert(mxCliff <= CLIFF_HEIGHT + 2); // parede abrupta, mas limitada
    assert(cliffCols > 0); // cliff existe de verdade
    for (int j = -1200; j < 1200; j++) {
        int s = surfaceHeight(j, seed);
        // coerência tileType x superfície
        for (int i = s - 2; i <= s + 2; i++) {
            Tile t = tileType(j, i, seed);
            // maciço sempre sólido, exceto boca (ar seco, água molhada)
            if (i > s) {
                bool mouthWater = t == Tile::Water && wormMouth(j, i, seed);
                assert(isSolid(t) || mouthWater ||
                       (t == Tile::Air && wormMouth(j, i, seed)));
            }
            if (i == s) {
                // topo decide por bioma (areia continua 6 na costa),
                // neve nos picos, boca (ar em terra, água em oceano).
                bool ocean = isOceanColumn(j, seed);
                Biome b = pickBiome(temperature(j, s, seed), humidity(j, s, seed),
                                    ocean, isCoastal(s));
                Tile expected = wormMouth(j, s, seed)
                    ? (ocean ? Tile::Water : Tile::Air)
                    : snowcap(s) ? Tile::Snow : biomeTopTile(b);
                assert(t == expected);
                // costa é areia, exceto boca (ar/água)
                if (isCoastal(s) && !wormMouth(j, s, seed)) assert(t == Tile::Sand);
            }
            if (i < s) {
                // ar, ilha — ou água na banda do oceano
                bool waterOk = isOceanColumn(j, seed) && i >= SEA_LEVEL
                               && t == Tile::Water;
                assert(t == Tile::Air || t == Tile::IslandPlatform || waterOk);
            }
            assert(tileType(j, i, seed) == t);            // determinístico
        }
    }
    std::printf("surface OK: faixa=[%d,%d] plano=%d cliff=%d (cols=%d)\n",
                lo, hi, mxPlain, mxCliff, cliffCols);
    // Global removido de propósito: fossa/monte somem degraus > 4 fora de
    // cliff. Os invariantes por zona acima são os que valem.
    return 0;
}
