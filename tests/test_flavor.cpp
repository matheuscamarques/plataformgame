#include <cassert>
#include <cstdio>
#include "support/World/Block.h"
#include "support/World/Generation.h"

// Flavor: determinístico, fração calibrada, ore intacto, superfície intacta.
int main() {
    using namespace support;

    { // Deterministic (strataRock puro em (tile, seed))
        for (int ty : {300, 1500, 3000, 4500, 5500}) {
            assert(strataRock(37, ty, 1337u) == strataRock(37, ty, 1337u));
            assert(tileType(37, ty, 1337u) == tileType(37, ty, 1337u));
        }
    }
    { // FlavorFraction (~4% PebbledStone na pedra do S1)
        int flavor = 0, stone = 0;
        for (int tx = -400; tx < 400; tx += 2) {
            for (int ty = 200; ty < 1400; ty += 20) {
                Tile t = tileType(tx, ty, 1337u);
                if (t == Tile::PebbledStone) flavor++;
                else if (t == Tile::Stone) stone++;
            }
        }
        const float f = (float)flavor / (flavor + stone);
        std::printf("S1 pebbled frac=%.4f (n=%d)\n", f, flavor + stone);
        assert(f > 0.02f && f < 0.06f);
    }
    { // BaseReplacesStone (S3 sem-ore = Granite ou flavor, nunca Stone)
        int granite = 0, other = 0;
        for (int tx = -400; tx < 400; tx += 2) {
            for (int ty = 2600; ty < 3800; ty += 20) {
                Tile t = tileType(tx, ty, 1337u);
                if (t == Tile::Stone) other++; // fura: base não pegou
                else if (t == Tile::Granite || t == Tile::StreakedStone) granite++;
            }
        }
        std::printf("S3 granite-like=%d stone-left=%d\n", granite, other);
        assert(granite > 0 && other == 0);
    }
    { // SurfaceUntouched (ty<200: só Stone ou ore, nunca flavor)
        for (int tx = -400; tx < 400; tx += 2) {
            int s = surfaceHeight(tx, 1337u);
            for (int ty = s + 4; ty < s + 40 && ty < 200; ty++) {
                Tile t = tileType(tx, ty, 1337u);
                if (!isSolid(t)) continue; // caverna/água: fora do hook
                assert(t == Tile::Stone || t == Tile::OreCopper ||
                       t == Tile::OreIron || t == Tile::OreGold);
            }
        }
    }

    std::printf("flavor test OK\n");
    return 0;
}
