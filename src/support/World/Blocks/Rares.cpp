#include "../BlockRegistry.h"

namespace support {

namespace {
constexpr uint16_t bit(int s) { return static_cast<uint16_t>(1u << s); }
constexpr uint16_t S1 = bit(1), S2 = bit(2), S3 = bit(3), S4 = bit(4),
                   S5 = bit(5), S6 = bit(6), S7 = bit(7), S8 = bit(8),
                   S9 = bit(9), S10 = bit(10);
} // namespace

// 1 raro por estrato S1..S10, hash01 puro com salt por tipo (uniforme,
// sem cluster de fbm). 2 gated (generates=false): VolcanicBomb precisa
// de explosão no break, SolarFlare de aura de calor — sistemas futuros.
REGISTER_BLOCK({"fossil_stone", Tile::FossilStone, BlockKind::Solid, 165, 155, 130,
                S1, 0.010f, false, false, true, false, true, 0xB001u});
REGISTER_BLOCK({"glow_cap", Tile::GlowCap, BlockKind::Solid, 90, 200, 180,
                S2, 0.020f, false, false, true, false, true, 0xB002u});
REGISTER_BLOCK({"geode_stone", Tile::GeodeStone, BlockKind::Solid, 100, 130, 155,
                S3, 0.015f, false, false, true, false, true, 0xB003u});
REGISTER_BLOCK({"ancient_lamp", Tile::AncientLamp, BlockKind::Solid, 200, 160, 90,
                S4, 0.010f, false, false, true, false, true, 0xB004u});
REGISTER_BLOCK({"crystal_heart", Tile::CrystalHeart, BlockKind::Solid, 120, 230, 240,
                S5, 0.008f, false, false, true, false, true, 0xB005u});
REGISTER_BLOCK({"volcanic_bomb", Tile::VolcanicBomb, BlockKind::Solid, 140, 60, 50,
                S6, 0.010f, false, false, true, false, false, 0xB006u});
REGISTER_BLOCK({"void_shard", Tile::VoidShard, BlockKind::Solid, 90, 60, 120,
                S7, 0.010f, false, false, true, false, true, 0xB007u});
REGISTER_BLOCK({"beating_heart", Tile::BeatingHeart, BlockKind::Solid, 170, 50, 60,
                S8, 0.005f, false, false, true, false, true, 0xB008u});
REGISTER_BLOCK({"solar_flare", Tile::SolarFlare, BlockKind::Solid, 255, 220, 140,
                S9, 0.005f, false, false, true, false, false, 0xB009u});
REGISTER_BLOCK({"world_edge", Tile::WorldEdge, BlockKind::Solid, 30, 30, 40,
                S10, 0.003f, false, false, true, false, true, 0xB00Au});

} // namespace support
