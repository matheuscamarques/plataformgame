#include <cassert>
#include <cstdio>
#include <string_view>
#include "support/World/Block.h"
#include "support/World/BlockRegistry.h"
#include "support/World/Tile.h"

// Registry: 10 entradas, base por estrato, sync com BLOCKS[].
int main() {
    using namespace support;
    BlockRegistry &reg = BlockRegistry::instance();

    { // TenEntriesWithMatchingBlocksTable
        assert(reg.all().size() == 11u);
        for (auto &e : reg.all()) {
            const BlockDef &b = blockDef(e.tile);
            assert(std::string_view(b.name) == std::string_view(e.name));
            assert(b.color.r == e.r && b.color.g == e.g && b.color.b == e.b);
            assert(b.kind == e.kind);
            assert(e.chance >= 0.f && e.chance < 1.f);
            assert(e.generates);
        }
    }
    { // BaseForStrata (S3+ tem base; S0-S2 = Stone legado)
        assert(reg.baseFor(0) == nullptr);
        assert(reg.baseFor(1) == nullptr && reg.baseFor(2) == nullptr);
        assert(reg.baseFor(3)->tile == Tile::Granite);
        assert(reg.baseFor(4)->tile == Tile::Granite);
        assert(reg.baseFor(5)->tile == Tile::CrystalRock);
        assert(reg.baseFor(6)->tile == Tile::Basalt);
        assert(reg.baseFor(7)->tile == Tile::VoidStone);
        assert(reg.baseFor(8)->tile == Tile::BloodRock);
        assert(reg.baseFor(9)->tile == Tile::PlasmaStone); // 2a: fix S9
        assert(reg.baseFor(10)->tile == Tile::VoidStone);
        assert(reg.find(Tile::Granite) != nullptr);
        assert(reg.find(Tile::Stone) == nullptr); // legado não registrado
    }
    { // FlavorsForStrata (1 por estrato S1..S5, resto vazio ainda)
        for (int s = 1; s <= 5; ++s) assert(reg.flavorsFor(s).size() == 1u);
        for (int s = 6; s <= 10; ++s) assert(reg.flavorsFor(s).empty());
        assert(reg.flavorsFor(1)[0]->tile == Tile::PebbledStone);
        assert(reg.raresFor(3).empty()); // raros vêm no C2
    }
    { // TileBudgetGuard (124 cabem em uint8_t com folga)
        assert(static_cast<int>(Tile::COUNT) == 54);
        assert(TILE_COUNT <= 200u);
    }

    std::printf("registry test OK\n");
    return 0;
}
