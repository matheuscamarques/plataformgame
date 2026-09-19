#include <cassert>
#include <cstdio>
#include "support/World/BlockRegistry.h"
#include "support/World/Generation.h"
#include "support/World/Tile.h"

// Gated (generates=false): VolcanicBomb e SolarFlare existem no registry
// e na BLOCKS, mas nunca saem do hook.
int main() {
    using namespace support;
    BlockRegistry &reg = BlockRegistry::instance();

    { // ExactlyTwoGated
        int gated = 0;
        for (auto &e : reg.all()) {
            if ((e.isRare || e.isFlavor) && !e.generates) gated++;
        }
        assert(gated == 2);
    }
    { // GatedEntriesAreBombAndFlare
        const BlockEntry *bomb = reg.find(Tile::VolcanicBomb);
        const BlockEntry *flare = reg.find(Tile::SolarFlare);
        assert(bomb != nullptr && flare != nullptr);
        assert(!bomb->generates && !flare->generates);
        assert(bomb->isRare && flare->isRare);
    }
    { // GatedExcludedFromRaresFor (S6/S9: hook não vê)
        assert(reg.raresFor(6).empty() && reg.raresFor(9).empty());
    }
    { // GatedNeverGenerated (varre S6 e S9 de verdade)
        for (int tx = -200; tx < 200; tx += 2) {
            for (int ty = 6200; ty < 7400; ty += 10) {
                assert(tileType(tx, ty, 1337u) != Tile::VolcanicBomb);
            }
            for (int ty = 9800; ty < 11000; ty += 10) {
                assert(tileType(tx, ty, 1337u) != Tile::SolarFlare);
            }
        }
    }

    std::printf("gated test OK\n");
    return 0;
}
