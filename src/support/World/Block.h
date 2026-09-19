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
    /* CrackedStone   */ {{ 90,  90,  90,255}, "cracked_stone",   BlockKind::Solid},
    /* Marble         */ {{240, 240, 245,255}, "marble",          BlockKind::Solid},
    /* Obsidian       */ {{ 30,  20,  40,255}, "obsidian",        BlockKind::Solid},
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
