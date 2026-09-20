#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "world/Generation.h"

// Um veio por estrato, faixas absolutas disjuntas. Superfície (ty<200)
// não tem minério; Diamond/Coal são legado (não geram).
int main() {
    using namespace support;

    const int LO[8] = {200, 1400, 2600, 3800, 5000, 6200, 7400, 8600};
    const int HI[8] = {1400, 2600, 3800, 5000, 6200, 7400, 8600, 9800};

    for (uint32_t seed : {1337u, 999u, 42u}) {
        int found[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        int dia = 0, coal = 0, n = 0;
        for (int v = 0; v < 8; v++) {
            for (int tx = -400; tx < 400; tx += 4) {
                for (int ty = LO[v]; ty < HI[v]; ty += 20) {
                    Tile t = tileType(tx, ty, seed);
                    n++;
                    switch (t) {
                        case Tile::OreCopper:   found[0]++; assert(ty < 1400); break;
                        case Tile::OreIron:     found[1]++; assert(ty >= 1400 && ty < 2600); break;
                        case Tile::OreSilver:   found[2]++; assert(ty >= 2600 && ty < 3800); break;
                        case Tile::OreGold:     found[3]++; assert(ty >= 3800 && ty < 5000); break;
                        case Tile::OreCrystal:  found[4]++; assert(ty >= 5000 && ty < 6200); break;
                        case Tile::OrePlatinum: found[5]++; assert(ty >= 6200 && ty < 7400); break;
                        case Tile::OreMithril:  found[6]++; assert(ty >= 7400 && ty < 8600); break;
                        case Tile::OreAdamant:  found[7]++; assert(ty >= 8600 && ty < 9800); break;
                        case Tile::OreDiamond:  dia++; break;
                        case Tile::OreCoal:     coal++; break;
                        default: break;
                    }
                    assert(tileType(tx, ty, seed) == t); // determinismo
                }
            }
        }
        // Todo veio aparece em toda seed; legado não gera.
        for (int v = 0; v < 8; v++) {
            std::printf("seed=%u veio%d=%d ", seed, v, found[v]);
            assert(found[v] > 0);
        }
        std::printf("dia=%d coal=%d\n", dia, coal);
        assert(dia == 0 && coal == 0);

        // Superfície limpa: pedra rasa sem minério.
        for (int tx = -400; tx < 400; tx += 4) {
            int s = surfaceHeight(tx, seed);
            for (int ty = s + 4; ty < s + 40 && ty < 200; ty++) {
                Tile t = tileType(tx, ty, seed);
                assert(t != Tile::OreCopper && t != Tile::OreIron &&
                       t != Tile::OreGold && t != Tile::OreSilver &&
                       t != Tile::OreCrystal && t != Tile::OrePlatinum &&
                       t != Tile::OreMithril && t != Tile::OreAdamant);
            }
        }
    }

    std::printf("ores test OK\n");
    return 0;
}
