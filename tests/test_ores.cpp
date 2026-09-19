#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "support/World/Generation.h"

// Minérios: raridade por profundidade, medidos em 3 seeds x 4000 cols:
// dia 0.06-0.07%, ag ~0.16%, au ~0.5%, fe ~0.73%, cu ~0.34%, coal ~1.5%.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int n = 0;
        int dia = 0, ag = 0, au = 0, fe = 0, cu = 0, coal = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s + 4; ty < s + 60; ty++) {
                Tile t = tileType(tx, ty, seed);
                n++;
                int depth = ty - s;
                switch (t) {
                    case Tile::OreDiamond:
                        dia++;
                        assert(depth > 25);
                        break;
                    case Tile::OreSilver:
                        ag++;
                        assert(depth > 15);
                        break;
                    case Tile::OreGold: au++; break;
                    case Tile::OreIron: fe++; break;
                    case Tile::OreCopper:
                        cu++;
                        assert(depth <= 12);
                        break;
                    case Tile::OreCoal:
                        coal++;
                        assert(depth > 4);
                        break;
                    default: break;
                }
                assert(tileType(tx, ty, seed) == t); // determinismo
            }
        }
        float fDia = (float)dia / n, fAg = (float)ag / n, fAu = (float)au / n;
        float fFe = (float)fe / n, fCu = (float)cu / n, fCoal = (float)coal / n;
        std::printf("seed=%u dia=%.4f ag=%.4f au=%.4f fe=%.4f cu=%.4f coal=%.4f\n",
                    seed, fDia, fAg, fAu, fFe, fCu, fCoal);
        assert(fDia > 0.0002f && fDia < 0.002f);
        assert(fAg > 0.0005f && fAg < 0.005f);
        assert(fAu > 0.002f && fAu < 0.015f);
        assert(fFe > 0.003f && fFe < 0.02f);
        assert(fCu > 0.001f && fCu < 0.01f);
        assert(fCoal > 0.005f && fCoal < 0.04f);
    }

    std::printf("ores test OK\n");
    return 0;
}
