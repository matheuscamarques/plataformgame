/**
 * @file src/world/Blocks/StrataShallow.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra variações visuais dos estratos rasos S1 a S5.
 * @details Declara flavors como dirt com raiz, micélio e quartzo via REGISTER_BLOCK, usados pela Generation para variedade perto da superfície.
 */

#include "../BlockRegistry.h"

namespace support {

namespace {
constexpr uint16_t bit(int s) { return static_cast<uint16_t>(1u << s); }
constexpr uint16_t S1 = bit(1), S2 = bit(2), S3 = bit(3), S4 = bit(4),
                   S5 = bit(5);
} // namespace

// Flood S1..S5 (2c). Estilo aggregate do C1 (sem lambdas).
// CrackedStone = Tile 27 legado, só ganha registro.
REGISTER_BLOCK({"rooted_dirt", Tile::RootedDirt, BlockKind::Solid, 95, 70, 50,
                S1, 0.03f, false, true, false, false, true, 0xF006u});
REGISTER_BLOCK({"cracked_stone", Tile::CrackedStone, BlockKind::Solid, 105, 100, 95,
                S1, 0.02f, false, true, false, false, true, 0xF007u});
REGISTER_BLOCK({"mycelium", Tile::Mycelium, BlockKind::Solid, 150, 150, 140,
                S2, 0.05f, false, true, false, false, true, 0xF008u});
REGISTER_BLOCK({"bioluminescent_stone", Tile::BioluminescentStone, BlockKind::Solid, 100, 150, 130,
                S2, 0.03f, false, true, false, false, true, 0xF009u});
REGISTER_BLOCK({"petrified_wood", Tile::PetrifiedWood, BlockKind::Solid, 110, 80, 55,
                S2, 0.04f, false, true, false, false, true, 0xF00Au});
REGISTER_BLOCK({"spore_stone", Tile::SporeStone, BlockKind::Solid, 100, 130, 90,
                S2, 0.03f, false, true, false, false, true, 0xF00Bu});
REGISTER_BLOCK({"mossy_boulder", Tile::MossyBoulder, BlockKind::Solid, 85, 115, 70,
                S2, 0.03f, false, true, false, false, true, 0xF00Cu});
REGISTER_BLOCK({"schist", Tile::Schist, BlockKind::Solid, 85, 95, 110,
                S3, 0.05f, false, true, false, false, true, 0xF00Du});
REGISTER_BLOCK({"quartz_stone", Tile::QuartzStone, BlockKind::Solid, 130, 140, 155,
                S3, 0.04f, false, true, false, false, true, 0xF00Eu});
REGISTER_BLOCK({"iron_banded", Tile::IronBanded, BlockKind::Solid, 140, 95, 70,
                S3, 0.04f, false, true, false, false, true, 0xF00Fu});
REGISTER_BLOCK({"pale_stone", Tile::PaleStone, BlockKind::Solid, 140, 145, 150,
                S3, 0.03f, false, true, false, false, true, 0xF010u});
REGISTER_BLOCK({"mineral_crust", Tile::MineralCrust, BlockKind::Solid, 110, 120, 130,
                S3, 0.02f, false, true, false, false, true, 0xF011u});
REGISTER_BLOCK({"plank", Tile::Plank, BlockKind::Solid, 105, 75, 45,
                S4, 0.06f, false, true, false, false, true, 0xF012u});
REGISTER_BLOCK({"broken_brick", Tile::BrokenBrick, BlockKind::Solid, 120, 100, 90,
                S4, 0.04f, false, true, false, false, true, 0xF013u});
REGISTER_BLOCK({"coal_dust", Tile::CoalDust, BlockKind::Solid, 60, 60, 65,
                S4, 0.04f, false, true, false, false, true, 0xF014u});
REGISTER_BLOCK({"rusted_metal", Tile::RustedMetal, BlockKind::Solid, 130, 70, 50,
                S4, 0.03f, false, true, false, false, true, 0xF015u});
REGISTER_BLOCK({"support_beam", Tile::SupportBeam, BlockKind::Solid, 95, 70, 45,
                S4, 0.02f, false, true, false, false, true, 0xF016u});
REGISTER_BLOCK({"geode", Tile::Geode, BlockKind::Solid, 100, 130, 155,
                S5, 0.04f, false, true, false, false, true, 0xF017u});
REGISTER_BLOCK({"prismatic_stone", Tile::PrismaticStone, BlockKind::Solid, 150, 140, 180,
                S5, 0.03f, false, true, false, false, true, 0xF018u});
REGISTER_BLOCK({"frozen_crystal", Tile::FrozenCrystal, BlockKind::Solid, 140, 180, 220,
                S5, 0.04f, false, true, false, false, true, 0xF019u});
REGISTER_BLOCK({"vibrant_stone", Tile::VibrantStone, BlockKind::Solid, 90, 120, 180,
                S5, 0.03f, false, true, false, false, true, 0xF01Au});
REGISTER_BLOCK({"pale_crystal", Tile::PaleCrystal, BlockKind::Solid, 180, 200, 210,
                S5, 0.04f, false, true, false, false, true, 0xF01Bu});

} // namespace support
