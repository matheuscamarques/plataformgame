#pragma once
#include <cstdint>
#include <cstddef>

namespace support {

// IDs de tile. Ordem numérica é CONTRATO:
//   - gravados em chunks modificados (save/load)
//   - gravados em replay, se um dia houver
// Nunca reordena, nunca reutiliza ID.
// Adicionar: sempre no fim, antes de COUNT.
enum class Tile : uint8_t {
    Air            = 0,
    TundraTop      = 1,
    IslandPlatform = 2,
    TaigaTop       = 3,
    FallbackTop    = 4,   // mantido por compat; não usado hoje
    SavannaTop     = 5,
    Sand           = 6,
    // 7 = slot histórico vazio; NUNCA reutilizar
    Snow           = 8,
    Grass          = 9,
    Dirt           = 10,
    Stone          = 11,
    OreCopper      = 12,
    OreIron        = 13,
    OreGold        = 14,
    Clay           = 15,
    Gravel         = 16,
    Sandstone      = 17,
    Permafrost     = 18,
    Mud            = 19,
    TreeTrunk      = 20,
    TreeLeaf       = 21,
    Water          = 22,
    OreCoal        = 23,
    OreSilver      = 24,
    OreDiamond     = 25,
    MossStone      = 26,
    CrackedStone   = 27,
    Marble         = 28,
    Obsidian       = 29,
    Ice            = 30,
    Lava           = 31,
    Bedrock        = 32,
    PineTrunk      = 33,
    PineLeaf       = 34,
    CactusTrunk    = 35,
    DeadBush       = 36,
    Mushroom       = 37,
    Glowshroom     = 38,

    COUNT
};

inline constexpr std::size_t TILE_COUNT = static_cast<std::size_t>(Tile::COUNT);

} // namespace support
