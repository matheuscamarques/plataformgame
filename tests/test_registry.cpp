#include <cassert>
#include <cstdio>
#include <string_view>
#include "world/Block.h"
#include "world/BlockRegistry.h"
#include "world/Tile.h"

// Registry: 10 entradas, base por estrato, sync com BLOCKS[].
int main() {
    using namespace support;
    BlockRegistry &reg = BlockRegistry::instance();

    { // TenEntriesWithMatchingBlocksTable
        assert(reg.all().size() == 75u);
        for (auto &e : reg.all()) {
            const BlockDef &b = blockDef(e.tile);
            assert(std::string_view(b.name) == std::string_view(e.name));
            assert(b.color.r == e.r && b.color.g == e.g && b.color.b == e.b);
            assert(b.kind == e.kind);
            assert(e.chance >= 0.f && e.chance < 1.f);
            if (!e.generates) { // só os 2 gated de 2b
                assert(e.tile == Tile::VolcanicBomb || e.tile == Tile::SolarFlare);
            }
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
    { // FlavorsForStrata (spot: Pebbled S1, GeodeStone raro S3)
        assert(reg.flavorsFor(1)[0]->tile == Tile::PebbledStone);
        assert(reg.raresFor(3)[0]->tile == Tile::GeodeStone); // 2b: raro S3
    }
    { // TileBudgetGuard (124 cabem em uint8_t com folga)
        assert(static_cast<int>(Tile::COUNT) == 116);
        assert(TILE_COUNT <= 200u);
    }
    { // FlavorsPerStratum (flood 2c: S1=3, S2-S9=5-6, S10=9 fundido)
        const std::size_t expect[11] = {0, 3, 6, 6, 6, 6, 5, 6, 6, 6, 9};
        for (int s = 0; s <= 10; ++s) {
            assert(reg.flavorsFor(s).size() == expect[s]);
            for (auto *f : reg.flavorsFor(s)) {
                assert(f->chance > 0.01f && f->chance < 0.10f);
            }
        }
    }
    { // RaresForStrata (1 por estrato S1..S5/S7/S8/S10; S6/S9 gated=vazio)
        assert(reg.raresFor(1)[0]->tile == Tile::FossilStone);
        assert(reg.raresFor(2)[0]->tile == Tile::GlowCap);
        assert(reg.raresFor(5)[0]->tile == Tile::CrystalHeart);
        assert(reg.raresFor(10)[0]->tile == Tile::WorldEdge);
        assert(reg.raresFor(6).empty() && reg.raresFor(9).empty());
        // Raro tem exatamente 1 estrato no mask.
        for (auto &e : reg.all()) {
            if (!e.isRare) continue;
            assert(__builtin_popcount(e.strataMask) == 1);
            assert(e.chance > 0.f && e.chance < 0.05f);
        }
    }

    std::printf("registry test OK\n");
    return 0;
}
