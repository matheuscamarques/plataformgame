#include "../BlockRegistry.h"

namespace support {

namespace {
// bit(s) = estrato s no mask.
constexpr uint16_t bit(int s) { return static_cast<uint16_t>(1u << s); }
// S1..S10 (superfície S0 não tem variante: Stone legado).
constexpr uint16_t S1 = bit(1), S2 = bit(2), S3 = bit(3), S4 = bit(4),
                   S5 = bit(5), S6 = bit(6), S7 = bit(7), S8 = bit(8),
                   S9 = bit(9), S10 = bit(10);
} // namespace

// --- Base rocks (1 por estrato a partir do S3; S9 temporário) ---
REGISTER_BLOCK({"granite", Tile::Granite, BlockKind::Solid, 90, 100, 115,
                S3 | S4, 0.f, true});
REGISTER_BLOCK({"crystal_rock", Tile::CrystalRock, BlockKind::Solid, 85, 105, 120,
                S5, 0.f, true});
REGISTER_BLOCK({"basalt", Tile::Basalt, BlockKind::Solid, 60, 55, 55,
                S6, 0.f, true});
REGISTER_BLOCK({"void_stone", Tile::VoidStone, BlockKind::Solid, 45, 35, 60,
                S7 | S10, 0.f, true});
REGISTER_BLOCK({"blood_rock", Tile::BloodRock, BlockKind::Solid, 85, 30, 40,
                S8 | S9, 0.f, true}); // S9 temporário: PlasmaStone vem no C2

// --- Flavors (1 por estrato S1..S5; hash01 com salt próprio) ---
REGISTER_BLOCK({"pebbled_stone", Tile::PebbledStone, BlockKind::Solid, 100, 100, 105,
                S1, 0.04f, false, true, false, false, true, 0xF001u});
REGISTER_BLOCK({"fungal_stone", Tile::FungalStone, BlockKind::Solid, 130, 90, 140,
                S2, 0.06f, false, true, false, false, true, 0xF002u});
REGISTER_BLOCK({"streaked_stone", Tile::StreakedStone, BlockKind::Solid, 120, 130, 145,
                S3, 0.06f, false, true, false, false, true, 0xF003u});
REGISTER_BLOCK({"cobblestone", Tile::Cobblestone, BlockKind::Solid, 110, 105, 95,
                S4, 0.05f, false, true, false, false, true, 0xF004u});
REGISTER_BLOCK({"crystal_cluster", Tile::CrystalCluster, BlockKind::Solid, 80, 220, 230,
                S5, 0.04f, false, true, false, false, true, 0xF005u});

} // namespace support
