/**
 * @file src/world/Tile.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Enum estável de IDs de tile com contrato de save.
 * @details Lista Tile de Air a COUNT com ordem imutável e nunca reutiliza IDs, incluída por quase todo o módulo world para tipos de bloco.
 */

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
    OreCrystal     = 39, // estratos: veios por faixa absoluta de ty
    OrePlatinum    = 40,
    OreMithril     = 41,
    OreAdamant     = 42,
    // Commit 1 visual: 5 base rocks + 5 flavors (registry em BlockRegistry.h)
    Granite        = 43,
    CrystalRock    = 44,
    Basalt         = 45,
    VoidStone      = 46,
    BloodRock      = 47,
    PebbledStone   = 48,
    FungalStone    = 49,
    StreakedStone  = 50,
    Cobblestone    = 51,
    CrystalCluster = 52,
    // 2a: base do S9 (tira o temporário BloodRock)
    PlasmaStone    = 53,
    // 2b: 10 raros (2 gated: VolcanicBomb, SolarFlare)
    FossilStone    = 54,
    GlowCap        = 55,
    GeodeStone     = 56,
    AncientLamp    = 57,
    CrystalHeart   = 58,
    VolcanicBomb   = 59,
    VoidShard      = 60,
    BeatingHeart   = 61,
    SolarFlare     = 62,
    WorldEdge      = 63,
    // 2c flood: flavors por estrato (S11 do catálogo fundido no S10:
    // código tem 11 estratos 0..10, sem Abyss separado)
    // S1 (CrackedStone = 27 já existia)
    RootedDirt     = 64,
    // S2
    Mycelium       = 65,
    BioluminescentStone = 66,
    PetrifiedWood  = 67,
    SporeStone     = 68,
    MossyBoulder   = 69,
    // S3
    Schist         = 70,
    QuartzStone    = 71,
    IronBanded     = 72,
    PaleStone      = 73,
    MineralCrust   = 74,
    // S4
    Plank          = 75,
    BrokenBrick    = 76,
    CoalDust       = 77,
    RustedMetal    = 78,
    SupportBeam    = 79,
    // S5
    Geode          = 80,
    PrismaticStone = 81,
    FrozenCrystal  = 82,
    VibrantStone   = 83,
    PaleCrystal    = 84,
    // S6 (Obsidian = 29 já existia)
    CharredStone   = 85,
    MagmaRock      = 86,
    SulfurStone    = 87,
    ScorchedEarth  = 88,
    // S7
    VoidAsh        = 89,
    TwistedStone   = 90,
    NullStone      = 91,
    EchoStone      = 92,
    SilentStone    = 93,
    StarFragment   = 94,
    // S8
    VeinedStone    = 95,
    DriedBlood     = 96,
    Heartstone     = 97,
    ScarredRock    = 98,
    CrimsonCrystal = 99,
    FleshStone     = 100,
    // S9
    MoltenGoldStone = 101,
    AncientRune    = 102,
    SolarStone     = 103,
    CelestialRock  = 104,
    EmberCore      = 105,
    RunedPillar    = 106,
    // S10 (+ S11 fundido: fundo/void)
    Voidglass      = 107,
    Nothingness    = 108,
    AbyssStone     = 109,
    FinalStone     = 110,
    EnderRock      = 111,
    NullField      = 112,
    FracturedVoid  = 113,
    SilentAbyss    = 114,
    AbyssalCore    = 115,

    COUNT
};

inline constexpr std::size_t TILE_COUNT = static_cast<std::size_t>(Tile::COUNT);

// Minério de verdade (patienceOnMine corta mais fundo com minério).
inline bool isOreTile(Tile t) {
    switch (t) {
        case Tile::OreCopper:
        case Tile::OreIron:
        case Tile::OreGold:
        case Tile::OreCoal:
        case Tile::OreSilver:
        case Tile::OreDiamond:
        case Tile::OreCrystal:
        case Tile::OrePlatinum:
        case Tile::OreMithril:
        case Tile::OreAdamant:
            return true;
        default:
            return false;
    }
}

} // namespace support
