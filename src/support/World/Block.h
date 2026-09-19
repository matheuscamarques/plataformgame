#pragma once
#include "Tile.h"
#include <SFML/Graphics/Color.hpp>
#include <array>
#include <string_view>

namespace support {

// Como o ChunkManager transforma este bloco em entidade (ou não).
enum class BlockKind : uint8_t {
    Air,     // nunca vira nada
    Solid,   // vira Entity COLIDE
    Liquid,  // vira Entity WATER
    Deco,    // vira Entity sem colisão (tronco, folha)
};

struct BlockDef {
    sf::Color   color;
    const char* name;
    BlockKind   kind;
};

// Uma entrada por Tile, NA ORDEM do enum. Índices fora de ordem =
// tabela errada silenciosamente (sem designated initializers em C++17).
inline const std::array<BlockDef, TILE_COUNT> BLOCKS = {{
    /* Air            */ {{  0,   0,   0,  0}, "air",             BlockKind::Air},
    /* TundraTop      */ {{ 60,  60,  60,255}, "tundra_top",      BlockKind::Solid},
    /* IslandPlatform */ {{146,  90,  43,255}, "island_platform", BlockKind::Solid},
    /* TaigaTop       */ {{120,  60,   0,255}, "taiga_top",       BlockKind::Solid},
    /* FallbackTop    */ {{159,  89,  30,255}, "fallback_top",    BlockKind::Solid},
    /* SavannaTop     */ {{150,  75,   0,255}, "savanna_top",     BlockKind::Solid},
    /* Sand           */ {{194, 178, 128,255}, "sand",            BlockKind::Solid},
    /* Unused7        */ {{  0,   0,   0,  0}, "unused_7",        BlockKind::Air},
    /* Snow           */ {{235, 235, 245,255}, "snow",            BlockKind::Solid},
    /* Grass          */ {{106, 190,  48,255}, "grass",           BlockKind::Solid},
    /* Dirt           */ {{139,  69,  19,255}, "dirt",            BlockKind::Solid},
    /* Stone          */ {{128, 128, 128,255}, "stone",           BlockKind::Solid},
    /* OreCopper      */ {{184, 115,  51,255}, "ore_copper",      BlockKind::Solid},
    /* OreIron        */ {{120, 120, 130,255}, "ore_iron",        BlockKind::Solid},
    /* OreGold        */ {{212, 175,  55,255}, "ore_gold",        BlockKind::Solid},
    /* Clay           */ {{150, 110,  90,255}, "clay",            BlockKind::Solid},
    /* Gravel         */ {{140, 140, 140,255}, "gravel",          BlockKind::Solid},
    /* Sandstone      */ {{210, 180, 120,255}, "sandstone",       BlockKind::Solid},
    /* Permafrost     */ {{170, 190, 200,255}, "permafrost",      BlockKind::Solid},
    /* Mud            */ {{ 90,  70,  50,255}, "mud",             BlockKind::Solid},
    /* TreeTrunk      */ {{101,  67,  33,255}, "tree_trunk",      BlockKind::Deco},
    /* TreeLeaf       */ {{ 34, 139,  34,255}, "tree_leaf",       BlockKind::Deco},
    /* Water          */ {{  0, 200, 255,255}, "water",           BlockKind::Liquid},
    /* OreCoal        */ {{ 40,  40,  40,255}, "ore_coal",        BlockKind::Solid},
    /* OreSilver      */ {{200, 200, 210,255}, "ore_silver",      BlockKind::Solid},
    /* OreDiamond     */ {{ 80, 220, 230,255}, "ore_diamond",     BlockKind::Solid},
    /* MossStone      */ {{110, 130, 100,255}, "moss_stone",      BlockKind::Solid},
    /* CrackedStone   */ {{105, 100,  95,255}, "cracked_stone",   BlockKind::Solid},
    /* Marble         */ {{240, 240, 245,255}, "marble",          BlockKind::Solid},
    /* Obsidian       */ {{ 25,  20,  30,255}, "obsidian",        BlockKind::Solid},
    /* Ice            */ {{180, 220, 255,180}, "ice",             BlockKind::Solid},
    /* Lava           */ {{255, 100,  20,255}, "lava",            BlockKind::Liquid},
    /* Bedrock        */ {{ 20,  20,  20,255}, "bedrock",         BlockKind::Solid},
    /* PineTrunk      */ {{ 70,  45,  25,255}, "pine_trunk",      BlockKind::Deco},
    /* PineLeaf       */ {{ 25,  90,  40,255}, "pine_leaf",       BlockKind::Deco},
    /* CactusTrunk    */ {{ 60, 130,  60,255}, "cactus_trunk",    BlockKind::Solid},
    /* DeadBush       */ {{120, 100,  60,255}, "dead_bush",       BlockKind::Deco},
    /* Mushroom       */ {{220, 100, 100,255}, "mushroom",        BlockKind::Deco},
    /* Glowshroom     */ {{100, 220, 220,255}, "glowshroom",      BlockKind::Deco},
    /* OreCrystal     */ {{120, 230, 230,255}, "ore_crystal",     BlockKind::Solid},
    /* OrePlatinum    */ {{225, 225, 235,255}, "ore_platinum",    BlockKind::Solid},
    /* OreMithril     */ {{110, 200, 130,255}, "ore_mithril",     BlockKind::Solid},
    /* OreAdamant     */ {{170,  40,  50,255}, "ore_adamant",     BlockKind::Solid},
    /* Granite        */ {{ 90, 100, 115,255}, "granite",         BlockKind::Solid},
    /* CrystalRock    */ {{ 85, 105, 120,255}, "crystal_rock",    BlockKind::Solid},
    /* Basalt         */ {{ 60,  55,  55,255}, "basalt",          BlockKind::Solid},
    /* VoidStone      */ {{ 45,  35,  60,255}, "void_stone",      BlockKind::Solid},
    /* BloodRock      */ {{ 85,  30,  40,255}, "blood_rock",      BlockKind::Solid},
    /* PebbledStone   */ {{100, 100, 105,255}, "pebbled_stone",   BlockKind::Solid},
    /* FungalStone    */ {{130,  90, 140,255}, "fungal_stone",    BlockKind::Solid},
    /* StreakedStone  */ {{120, 130, 145,255}, "streaked_stone",  BlockKind::Solid},
    /* Cobblestone    */ {{110, 105,  95,255}, "cobblestone",     BlockKind::Solid},
    /* CrystalCluster */ {{ 80, 220, 230,255}, "crystal_cluster", BlockKind::Solid},
    /* PlasmaStone    */ {{120,  90,  50,255}, "plasma_stone",    BlockKind::Solid},
    /* FossilStone    */ {{165, 155, 130,255}, "fossil_stone",    BlockKind::Solid},
    /* GlowCap        */ {{ 90, 200, 180,255}, "glow_cap",        BlockKind::Solid},
    /* GeodeStone     */ {{100, 130, 155,255}, "geode_stone",     BlockKind::Solid},
    /* AncientLamp    */ {{200, 160,  90,255}, "ancient_lamp",    BlockKind::Solid},
    /* CrystalHeart   */ {{120, 230, 240,255}, "crystal_heart",   BlockKind::Solid},
    /* VolcanicBomb   */ {{140,  60,  50,255}, "volcanic_bomb",   BlockKind::Solid},
    /* VoidShard      */ {{ 90,  60, 120,255}, "void_shard",      BlockKind::Solid},
    /* BeatingHeart   */ {{170,  50,  60,255}, "beating_heart",   BlockKind::Solid},
    /* SolarFlare     */ {{255, 220, 140,255}, "solar_flare",     BlockKind::Solid},
    /* WorldEdge      */ {{ 30,  30,  40,255}, "world_edge",      BlockKind::Solid},
    /* RootedDirt     */ {{ 95,  70,  50,255}, "rooted_dirt",     BlockKind::Solid},
    /* Mycelium       */ {{150, 150, 140,255}, "mycelium",        BlockKind::Solid},
    /* BioluminescentStone */ {{100, 150, 130,255}, "bioluminescent_stone", BlockKind::Solid},
    /* PetrifiedWood  */ {{110,  80,  55,255}, "petrified_wood",  BlockKind::Solid},
    /* SporeStone     */ {{100, 130,  90,255}, "spore_stone",     BlockKind::Solid},
    /* MossyBoulder   */ {{ 85, 115,  70,255}, "mossy_boulder",   BlockKind::Solid},
    /* Schist         */ {{ 85,  95, 110,255}, "schist",          BlockKind::Solid},
    /* QuartzStone    */ {{130, 140, 155,255}, "quartz_stone",    BlockKind::Solid},
    /* IronBanded     */ {{140,  95,  70,255}, "iron_banded",     BlockKind::Solid},
    /* PaleStone      */ {{140, 145, 150,255}, "pale_stone",      BlockKind::Solid},
    /* MineralCrust   */ {{110, 120, 130,255}, "mineral_crust",   BlockKind::Solid},
    /* Plank          */ {{105,  75,  45,255}, "plank",           BlockKind::Solid},
    /* BrokenBrick    */ {{120, 100,  90,255}, "broken_brick",    BlockKind::Solid},
    /* CoalDust       */ {{ 60,  60,  65,255}, "coal_dust",       BlockKind::Solid},
    /* RustedMetal    */ {{130,  70,  50,255}, "rusted_metal",    BlockKind::Solid},
    /* SupportBeam    */ {{ 95,  70,  45,255}, "support_beam",    BlockKind::Solid},
    /* Geode          */ {{100, 130, 155,255}, "geode",           BlockKind::Solid},
    /* PrismaticStone */ {{150, 140, 180,255}, "prismatic_stone", BlockKind::Solid},
    /* FrozenCrystal  */ {{140, 180, 220,255}, "frozen_crystal",  BlockKind::Solid},
    /* VibrantStone   */ {{ 90, 120, 180,255}, "vibrant_stone",   BlockKind::Solid},
    /* PaleCrystal    */ {{180, 200, 210,255}, "pale_crystal",    BlockKind::Solid},
    /* CharredStone   */ {{ 55,  45,  40,255}, "charred_stone",   BlockKind::Solid},
    /* MagmaRock      */ {{120,  50,  30,255}, "magma_rock",      BlockKind::Solid},
    /* SulfurStone    */ {{140, 130,  60,255}, "sulfur_stone",    BlockKind::Solid},
    /* ScorchedEarth  */ {{ 80,  60,  45,255}, "scorched_earth",  BlockKind::Solid},
    /* VoidAsh        */ {{ 60,  55,  70,255}, "void_ash",        BlockKind::Solid},
    /* TwistedStone   */ {{ 70,  55,  90,255}, "twisted_stone",   BlockKind::Solid},
    /* NullStone      */ {{ 35,  30,  50,255}, "null_stone",      BlockKind::Solid},
    /* EchoStone      */ {{ 55,  45,  75,255}, "echo_stone",      BlockKind::Solid},
    /* SilentStone    */ {{ 40,  35,  55,255}, "silent_stone",    BlockKind::Solid},
    /* StarFragment   */ {{ 60,  50,  90,255}, "star_fragment",   BlockKind::Solid},
    /* VeinedStone    */ {{100,  35,  45,255}, "veined_stone",    BlockKind::Solid},
    /* DriedBlood     */ {{ 90,  50,  45,255}, "dried_blood",     BlockKind::Solid},
    /* Heartstone     */ {{130,  40,  55,255}, "heartstone",      BlockKind::Solid},
    /* ScarredRock    */ {{105,  55,  60,255}, "scarred_rock",    BlockKind::Solid},
    /* CrimsonCrystal */ {{150,  45,  55,255}, "crimson_crystal", BlockKind::Solid},
    /* FleshStone     */ {{120,  70,  65,255}, "flesh_stone",     BlockKind::Solid},
    /* MoltenGoldStone */ {{180, 140,  60,255}, "molten_gold_stone", BlockKind::Solid},
    /* AncientRune    */ {{120, 100,  50,255}, "ancient_rune",    BlockKind::Solid},
    /* SolarStone     */ {{200, 180, 100,255}, "solar_stone",     BlockKind::Solid},
    /* CelestialRock  */ {{140, 120,  70,255}, "celestial_rock",  BlockKind::Solid},
    /* EmberCore      */ {{160,  90,  40,255}, "ember_core",      BlockKind::Solid},
    /* RunedPillar    */ {{110,  90,  55,255}, "runed_pillar",    BlockKind::Solid},
    /* Voidglass      */ {{ 40,  45,  60,255}, "voidglass",       BlockKind::Solid},
    /* Nothingness    */ {{ 20,  20,  25,255}, "nothingness",     BlockKind::Solid},
    /* AbyssStone     */ {{ 30,  35,  50,255}, "abyss_stone",     BlockKind::Solid},
    /* FinalStone     */ {{ 15,  15,  20,255}, "final_stone",     BlockKind::Solid},
    /* EnderRock      */ {{ 50,  40,  70,255}, "ender_rock",      BlockKind::Solid},
    /* NullField      */ {{ 25,  25,  35,255}, "null_field",      BlockKind::Solid},
    /* FracturedVoid  */ {{ 30,  25,  40,255}, "fractured_void",  BlockKind::Solid},
    /* SilentAbyss    */ {{ 18,  18,  25,255}, "silent_abyss",    BlockKind::Solid},
    /* AbyssalCore    */ {{ 35,  30,  50,255}, "abyssal_core",    BlockKind::Solid},
}};

inline const BlockDef& blockDef(Tile t) {
    auto i = static_cast<std::size_t>(t);
    return BLOCKS[i < TILE_COUNT ? i : 0];   // fallback: Air
}

inline const BlockDef* blockDefByName(std::string_view name) {
    for (const auto& b : BLOCKS) {
        if (name == b.name) return &b;
    }
    return nullptr;
}

inline bool isSolid(Tile t)   { return blockDef(t).kind == BlockKind::Solid; }
inline bool isLiquid(Tile t)  { return blockDef(t).kind == BlockKind::Liquid; }
inline bool isDeco(Tile t)    { return blockDef(t).kind == BlockKind::Deco; }

} // namespace support
