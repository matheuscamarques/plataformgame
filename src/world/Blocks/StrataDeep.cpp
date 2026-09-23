/**
 * @file src/world/Blocks/StrataDeep.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra variações visuais dos estratos profundos S6 a S10.
 * @details Declara dezenas de flavors via REGISTER_BLOCK com cores, chances e salts próprios, consultados pela Generation para variedade em grandes profundidades.
 */

#include "../BlockRegistry.h"

namespace support {

namespace {
constexpr uint16_t bit(int s) { return static_cast<uint16_t>(1u << s); }
constexpr uint16_t S6 = bit(6), S7 = bit(7), S8 = bit(8), S9 = bit(9),
                   S10 = bit(10);
} // namespace

// Flood S6..S10 (2c). S11 do catálogo fundido no S10 (só 11 estratos).
// Obsidian = Tile 29 legado, só ganha registro.
REGISTER_BLOCK({"charred_stone", Tile::CharredStone, BlockKind::Solid, 55, 45, 40,
                S6, 0.05f, false, true, false, false, true, 0xF01Cu});
REGISTER_BLOCK({"obsidian", Tile::Obsidian, BlockKind::Solid, 25, 20, 30,
                S6, 0.03f, false, true, false, false, true, 0xF01Du});
REGISTER_BLOCK({"magma_rock", Tile::MagmaRock, BlockKind::Solid, 120, 50, 30,
                S6, 0.04f, false, true, false, false, true, 0xF01Eu});
REGISTER_BLOCK({"sulfur_stone", Tile::SulfurStone, BlockKind::Solid, 140, 130, 60,
                S6, 0.04f, false, true, false, false, true, 0xF01Fu});
REGISTER_BLOCK({"scorched_earth", Tile::ScorchedEarth, BlockKind::Solid, 80, 60, 45,
                S6, 0.04f, false, true, false, false, true, 0xF020u});
REGISTER_BLOCK({"void_ash", Tile::VoidAsh, BlockKind::Solid, 60, 55, 70,
                S7, 0.05f, false, true, false, false, true, 0xF021u});
REGISTER_BLOCK({"twisted_stone", Tile::TwistedStone, BlockKind::Solid, 70, 55, 90,
                S7, 0.04f, false, true, false, false, true, 0xF022u});
REGISTER_BLOCK({"null_stone", Tile::NullStone, BlockKind::Solid, 35, 30, 50,
                S7, 0.03f, false, true, false, false, true, 0xF023u});
REGISTER_BLOCK({"echo_stone", Tile::EchoStone, BlockKind::Solid, 55, 45, 75,
                S7, 0.04f, false, true, false, false, true, 0xF024u});
REGISTER_BLOCK({"silent_stone", Tile::SilentStone, BlockKind::Solid, 40, 35, 55,
                S7, 0.05f, false, true, false, false, true, 0xF025u});
REGISTER_BLOCK({"star_fragment", Tile::StarFragment, BlockKind::Solid, 60, 50, 90,
                S7, 0.03f, false, true, false, false, true, 0xF026u});
REGISTER_BLOCK({"veined_stone", Tile::VeinedStone, BlockKind::Solid, 100, 35, 45,
                S8, 0.05f, false, true, false, false, true, 0xF027u});
REGISTER_BLOCK({"dried_blood", Tile::DriedBlood, BlockKind::Solid, 90, 50, 45,
                S8, 0.05f, false, true, false, false, true, 0xF028u});
REGISTER_BLOCK({"heartstone", Tile::Heartstone, BlockKind::Solid, 130, 40, 55,
                S8, 0.03f, false, true, false, false, true, 0xF029u});
REGISTER_BLOCK({"scarred_rock", Tile::ScarredRock, BlockKind::Solid, 105, 55, 60,
                S8, 0.04f, false, true, false, false, true, 0xF02Au});
REGISTER_BLOCK({"crimson_crystal", Tile::CrimsonCrystal, BlockKind::Solid, 150, 45, 55,
                S8, 0.03f, false, true, false, false, true, 0xF02Bu});
REGISTER_BLOCK({"flesh_stone", Tile::FleshStone, BlockKind::Solid, 120, 70, 65,
                S8, 0.03f, false, true, false, false, true, 0xF02Cu});
REGISTER_BLOCK({"molten_gold_stone", Tile::MoltenGoldStone, BlockKind::Solid, 180, 140, 60,
                S9, 0.05f, false, true, false, false, true, 0xF02Du});
REGISTER_BLOCK({"ancient_rune", Tile::AncientRune, BlockKind::Solid, 120, 100, 50,
                S9, 0.03f, false, true, false, false, true, 0xF02Eu});
REGISTER_BLOCK({"solar_stone", Tile::SolarStone, BlockKind::Solid, 200, 180, 100,
                S9, 0.03f, false, true, false, false, true, 0xF02Fu});
REGISTER_BLOCK({"celestial_rock", Tile::CelestialRock, BlockKind::Solid, 140, 120, 70,
                S9, 0.03f, false, true, false, false, true, 0xF030u});
REGISTER_BLOCK({"ember_core", Tile::EmberCore, BlockKind::Solid, 160, 90, 40,
                S9, 0.04f, false, true, false, false, true, 0xF031u});
REGISTER_BLOCK({"runed_pillar", Tile::RunedPillar, BlockKind::Solid, 110, 90, 55,
                S9, 0.02f, false, true, false, false, true, 0xF032u});
REGISTER_BLOCK({"voidglass", Tile::Voidglass, BlockKind::Solid, 40, 45, 60,
                S10, 0.04f, false, true, false, false, true, 0xF033u});
REGISTER_BLOCK({"nothingness", Tile::Nothingness, BlockKind::Solid, 20, 20, 25,
                S10, 0.06f, false, true, false, false, true, 0xF034u});
REGISTER_BLOCK({"abyss_stone", Tile::AbyssStone, BlockKind::Solid, 30, 35, 50,
                S10, 0.04f, false, true, false, false, true, 0xF035u});
REGISTER_BLOCK({"final_stone", Tile::FinalStone, BlockKind::Solid, 15, 15, 20,
                S10, 0.03f, false, true, false, false, true, 0xF036u});
REGISTER_BLOCK({"ender_rock", Tile::EnderRock, BlockKind::Solid, 50, 40, 70,
                S10, 0.03f, false, true, false, false, true, 0xF037u});
REGISTER_BLOCK({"null_field", Tile::NullField, BlockKind::Solid, 25, 25, 35,
                S10, 0.05f, false, true, false, false, true, 0xF038u});
REGISTER_BLOCK({"fractured_void", Tile::FracturedVoid, BlockKind::Solid, 30, 25, 40,
                S10, 0.03f, false, true, false, false, true, 0xF039u});
REGISTER_BLOCK({"silent_abyss", Tile::SilentAbyss, BlockKind::Solid, 18, 18, 25,
                S10, 0.06f, false, true, false, false, true, 0xF03Au});
REGISTER_BLOCK({"abyssal_core", Tile::AbyssalCore, BlockKind::Solid, 35, 30, 50,
                S10, 0.03f, false, true, false, false, true, 0xF03Bu});

} // namespace support
