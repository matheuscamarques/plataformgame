#include <cassert>
#include <cstdio>
#include "core/Noise.h"
#include "world/Block.h"
#include "world/Generation.h"

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
    { // RareFraction (~1% FossilStone no S1, hash direto ±30%)
        const float chance = 0.010f;
        const int N = 20000;
        int hits = 0;
        for (int i = 0; i < N; ++i) {
            if (core::rand01(i, 0, 1337u + 0xB001u) < chance) hits++;
        }
        std::printf("fossil hash frac=%.4f\n", (float)hits / N);
        assert(hits > (int)(N * chance * 0.7f) && hits < (int)(N * chance * 1.3f));
    }
    { // RareExistsInStratum (S1 gera FossilStone de verdade via tileType)
        int found = 0;
        for (int tx = -400; tx < 400 && !found; tx += 2) {
            for (int ty = 200; ty < 1400; ty += 10) {
                if (tileType(tx, ty, 1337u) == Tile::FossilStone) { found++; break; }
            }
        }
        assert(found > 0);
    }
    { // RareNeverInWrongStratum (fossil só no S1; varre S2..S10)
        for (int tx = -200; tx < 200; tx += 4) {
            for (int ty = 1400; ty < 12000; ty += 40) {
                assert(tileType(tx, ty, 1337u) != Tile::FossilStone);
            }
        }
    }
    { // DeepFlavorFraction (~5% SilentStone no S7, ±margem larga)
        int flavor = 0, other = 0;
        for (int tx = -200; tx < 200; tx += 2) {
            for (int ty = 7400; ty < 8600; ty += 20) {
                Tile t = tileType(tx, ty, 1337u);
                if (t == Tile::SilentStone) flavor++;
                else if (t == Tile::VoidStone || t == Tile::Stone) other++;
            }
        }
        const float f = (float)flavor / (flavor + other);
        std::printf("S7 silent frac=%.4f\n", f);
        assert(f > 0.02f && f < 0.09f);
    }

    std::printf("flavor test OK\n");
    return 0;
}
