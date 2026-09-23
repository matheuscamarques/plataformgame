/**
 * @file src/core/ItemDefs.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registro inicial dos itens jogáveis via macro no boot.
 * @details Cria defs de TNT, gel, minérios, madeira e poção com sprites ASCII, carregado uma vez no ItemRegistry.
 */

// Defs iniciais (estático, 1× no boot via REGISTER_ITEM).
// Conjunto mínimo jogável: TNT da pilha, gel do slime, minérios,
// madeira/pedra de construção e poção. Mesmo padrão de
// REGISTER_ENEMY_ARCHETYPE (lambda que retorna o def).
#include "core/ItemDef.h"
#include "entities/Player/Player.h"

namespace {

using core::ItemRarity;
using core::ItemType;

void heal30(Player& p) {
    p.hp = std::min(p.hpMax, p.hp + 30);
}

} // namespace


// ---- Sprites ASCII 8x8 dos itens (fase 4a; mesmo idioma de makeSprite)
inline const core::PaletteEntry kItemDynamitePal[] = {
    {'.', {0, 0, 0, 0}},
    {'R', {220, 60, 60}},
    {'r', {150, 40, 40}},
    {'F', {120, 80, 40}},
};
inline constexpr std::size_t kItemDynamitePalCount = 4;
inline const char* const kItemDynamiteSprite[] = {
    "...FF...",
    "...FF...",
    "..RRRR..",
    "..RRRR..",
    "..RrRR..",
    "..RRRR..",
    "..RRRR..",
    "........",
};

inline const core::PaletteEntry kItemGelPal[] = {
    {'.', {0, 0, 0, 0}},
    {'G', {80, 220, 100}},
    {'g', {50, 150, 70}},
    {'W', {230, 255, 230}},
};
inline constexpr std::size_t kItemGelPalCount = 4;
inline const char* const kItemGelSprite[] = {
    "..GGGG..",
    ".GGGGGG.",
    "GGWGGGGg",
    "GGGGGGGg",
    ".GGGGGgg",
    "..GGggg.",
    "........",
    "........",
};

inline const core::PaletteEntry kItemStonePal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {120, 120, 120}},
    {'s', {90, 90, 90}},
};
inline constexpr std::size_t kItemStonePalCount = 3;
inline const char* const kItemStoneSprite[] = {
    "..SSSs..",
    ".SSSSSs.",
    "SSSSSssS",
    "SSSSSssS",
    "sSSSSsss",
    "ssSSSsss",
    ".ssssss.",
    "..ssss..",
};

inline const core::PaletteEntry kItemWoodPal[] = {
    {'.', {0, 0, 0, 0}},
    {'W', {110, 75, 45}},
    {'w', {80, 55, 35}},
};
inline constexpr std::size_t kItemWoodPalCount = 3;
inline const char* const kItemWoodSprite[] = {
    "wwwwwwww",
    "WwwwwwwW",
    "WWwwwwWW",
    "WWWwwWWW",
    "WWWwwWWW",
    "WWwwwwWW",
    "WwwwwwwW",
    "wwwwwwww",
};

inline const core::PaletteEntry kItemIronPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {120, 120, 130}},
    {'s', {85, 85, 95}},
    {'O', {220, 130, 70}},
};
inline constexpr std::size_t kItemIronPalCount = 4;
inline const char* const kItemIronSprite[] = {
    "..SSSs..",
    ".SSOSSS.",
    "SSSSSssS",
    "SSOSSSsS",
    "sSSSSSss",
    "ssSSOsSS",
    ".ssssss.",
    "..ssss..",
};

inline const core::PaletteEntry kItemCopperPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {120, 120, 120}},
    {'s', {90, 90, 90}},
    {'C', {200, 120, 70}},
};
inline constexpr std::size_t kItemCopperPalCount = 4;
inline const char* const kItemCopperSprite[] = {
    "..SSSs..",
    ".SCSSSS.",
    "SSSSSssS",
    "SSCSSSsS",
    "sSSSSSCs",
    "ssSSCsSS",
    ".ssssss.",
    "..ssss..",
};

inline const core::PaletteEntry kItemGoldPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {120, 120, 120}},
    {'s', {90, 90, 90}},
    {'Y', {240, 200, 80}},
};
inline constexpr std::size_t kItemGoldPalCount = 4;
inline const char* const kItemGoldSprite[] = {
    "..SSSs..",
    ".SYSSSS.",
    "SSSSSssS",
    "SSYSSSsS",
    "sSSSSSYs",
    "ssSSYsSS",
    ".ssssss.",
    "..ssss..",
};

inline const core::PaletteEntry kItemPotionPal[] = {
    {'.', {0, 0, 0, 0}},
    {'G', {200, 200, 210}},
    {'R', {220, 60, 60}},
    {'g', {150, 150, 160}},
};
inline constexpr std::size_t kItemPotionPalCount = 4;
inline const char* const kItemPotionSprite[] = {
    "..ggg...",
    "..GGG...",
    "..GGG...",
    ".GGGGG..",
    "GRRRRRG.",
    "GRRRRRG.",
    "GRRRRRG.",
    ".GGGGG..",
};

inline const core::PaletteEntry kItemKeyPal[] = {
    {'.', {0, 0, 0, 0}},
    {'K', {220, 180, 90}},
    {'k', {150, 120, 60}},
};
inline constexpr std::size_t kItemKeyPalCount = 3;
inline const char* const kItemKeySprite[] = {
    "..KKK...",
    "...K....",
    "...K....",
    "...KK...",
    "...K....",
    "...K....",
    "..KKK...",
    "........",
};

// Fase 4d: 1 arma + 1 armadura p/ exercitar o painel (dano/defesa)
// e as abas Arma/Armadura. Sem onEquip: equipa via verbo futuro.
inline const core::PaletteEntry kItemSwordPal[] = {
    {'.', {0, 0, 0, 0}},
    {'B', {200, 205, 215}},
    {'b', {140, 145, 155}},
    {'H', {110, 75, 45}},
};
inline constexpr std::size_t kItemSwordPalCount = 4;
inline const char* const kItemSwordSprite[] = {
    "...BB...",
    "...BB...",
    "...BB...",
    "...Bb...",
    "...Bb...",
    ".HHHHH..",
    "...H....",
    "...H....",
};

inline const core::PaletteEntry kItemHelmPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {150, 155, 165}},
    {'s', {105, 110, 120}},
    {'D', {60, 65, 75}},
};
inline constexpr std::size_t kItemHelmPalCount = 4;
inline const char* const kItemHelmSprite[] = {
    "..SSSS..",
    ".SSSSSS.",
    "SSSSDSSS",
    "SSSDDSSS",
    "SSSSSSSS",
    ".ssssss.",
    "........",
    "........",
};

// Fase 4f: machado + peitoral + perneiras (mesmo padrão: sprite 8×8,
// stackMax 1, slot próprio). Completam os 4 slots do Equipment.
inline const core::PaletteEntry kItemAxePal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {150, 155, 165}},
    {'s', {105, 110, 120}},
    {'H', {110, 75, 45}},
};
inline constexpr std::size_t kItemAxePalCount = 4;
inline const char* const kItemAxeSprite[] = {
    "..SSSS..",
    "..SSSs..",
    "...SS...",
    "...SS...",
    "...HS...",
    "..HH....",
    "..H.....",
    ".HH.....",
};

inline const core::PaletteEntry kItemChestPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {150, 155, 165}},
    {'s', {105, 110, 120}},
    {'D', {60, 65, 75}},
};
inline constexpr std::size_t kItemChestPalCount = 4;
inline const char* const kItemChestSprite[] = {
    ".SSSSSS.",
    "SSSSSSSS",
    "SSSDDSSS",
    "SSSSSSSS",
    "sSSSSSSs",
    "sSSSSSSs",
    ".ssssss.",
    "........",
};

inline const core::PaletteEntry kItemLegsPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {150, 155, 165}},
    {'s', {105, 110, 120}},
};
inline constexpr std::size_t kItemLegsPalCount = 3;
inline const char* const kItemLegsSprite[] = {
    "SSSSSSSS",
    "SSS..SSS",
    "SS....SS",
    "SS....SS",
    "SS....SS",
    "ss....ss",
    "ss....ss",
    "........",
};

// Set de couro: mesmas formas do ferro, paleta marrom (reusa as rows;
// só a paleta troca). Defesa menor, mesmo padrão de slots.
inline const core::PaletteEntry kItemLeatherHelmPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {160, 110, 70}},
    {'s', {100, 70, 45}},
    {'D', {60, 65, 75}},
};
inline constexpr std::size_t kItemLeatherHelmPalCount = 4;

inline const core::PaletteEntry kItemLeatherChestPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {160, 110, 70}},
    {'s', {100, 70, 45}},
    {'D', {60, 65, 75}},
};
inline constexpr std::size_t kItemLeatherChestPalCount = 4;

inline const core::PaletteEntry kItemLeatherLegsPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {160, 110, 70}},
    {'s', {100, 70, 45}},
};
inline constexpr std::size_t kItemLeatherLegsPalCount = 3;

// Matriz completa (falta do review): ouro e diamante no set inteiro +
// armas de couro. Mesmas formas do ferro, paleta por material.
inline const core::PaletteEntry kItemGoldSwordPal[] = {
    {'.', {0, 0, 0, 0}},
    {'B', {240, 200, 80}},
    {'b', {170, 140, 50}},
    {'H', {110, 75, 45}},
};
inline constexpr std::size_t kItemGoldSwordPalCount = 4;
inline const core::PaletteEntry kItemDiamondSwordPal[] = {
    {'.', {0, 0, 0, 0}},
    {'B', {180, 220, 240}},
    {'b', {100, 150, 190}},
    {'H', {60, 65, 75}},
};
inline constexpr std::size_t kItemDiamondSwordPalCount = 4;
inline const core::PaletteEntry kItemLeatherSwordPal[] = {
    {'.', {0, 0, 0, 0}},
    {'B', {160, 110, 70}},
    {'b', {100, 70, 45}},
    {'H', {80, 55, 35}},
};
inline constexpr std::size_t kItemLeatherSwordPalCount = 4;

inline const core::PaletteEntry kItemGoldAxePal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {240, 200, 80}},
    {'s', {170, 140, 50}},
    {'H', {110, 75, 45}},
};
inline constexpr std::size_t kItemGoldAxePalCount = 4;
inline const core::PaletteEntry kItemDiamondAxePal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {180, 220, 240}},
    {'s', {100, 150, 190}},
    {'H', {60, 65, 75}},
};
inline constexpr std::size_t kItemDiamondAxePalCount = 4;
inline const core::PaletteEntry kItemLeatherAxePal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {160, 110, 70}},
    {'s', {100, 70, 45}},
    {'H', {80, 55, 35}},
};
inline constexpr std::size_t kItemLeatherAxePalCount = 4;

inline const core::PaletteEntry kItemGoldArmorPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {220, 190, 90}},
    {'s', {170, 140, 50}},
    {'D', {255, 230, 130}},
};
inline constexpr std::size_t kItemGoldArmorPalCount = 4;
inline const core::PaletteEntry kItemDiamondArmorPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {180, 220, 240}},
    {'s', {100, 150, 190}},
    {'D', {220, 240, 255}},
};
inline constexpr std::size_t kItemDiamondArmorPalCount = 4;


REGISTER_ITEM("dynamite", [] {
    core::ItemDef def;
    def.id = "dynamite";
    def.description = "Explosivo arremessável (J).";
    def.spriteRows = kItemDynamiteSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDynamitePal;
    def.spritePalCount = kItemDynamitePalCount;
    def.name = "Dinamite";
    def.type = ItemType::Consumable; // arremesso consome da pilha (verbo)
    def.rarity = ItemRarity::Common;
    def.stackMax = 999; // pilha única: 999 em 1 slot (kit cabe em 40)
    def.throwable = true; // J joga (legado); stats = padrão do Throwable
    def.throwKind = support::ThrowKind::Dynamite;
    def.fuse = 1.0f;
    def.blastRadius = 40.f;
    def.blastDamage = 25;
    def.blastTiles = 3;
    return def;
}())

REGISTER_ITEM("slime_gel", [] {
    core::ItemDef def;
    def.id = "slime_gel";
    def.description = "Gosma de slime. Gruda em tudo.";
    def.spriteRows = kItemGelSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGelPal;
    def.spritePalCount = kItemGelPalCount;
    def.name = "Gel de Slime";
    def.type = ItemType::Material;
    def.stackMax = 99;
    return def;
}())

REGISTER_ITEM("stone", [] {
    core::ItemDef def;
    def.id = "stone";
    def.description = "Pedra comum de construção.";
    def.spriteRows = kItemStoneSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemStonePal;
    def.spritePalCount = kItemStonePalCount;
    def.name = "Pedra";
    def.type = ItemType::Material;
    def.stackMax = 99;
    return def;
}())

REGISTER_ITEM("wood", [] {
    core::ItemDef def;
    def.id = "wood";
    def.description = "Madeira de tronco.";
    def.spriteRows = kItemWoodSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemWoodPal;
    def.spritePalCount = kItemWoodPalCount;
    def.name = "Madeira";
    def.type = ItemType::Material;
    def.stackMax = 99;
    return def;
}())

REGISTER_ITEM("iron_ore", [] {
    core::ItemDef def;
    def.id = "iron_ore";
    def.description = "Ferro bruto. Cheira a progresso.";
    def.spriteRows = kItemIronSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemIronPal;
    def.spritePalCount = kItemIronPalCount;
    def.name = "Minério de Ferro";
    def.type = ItemType::Material;
    def.rarity = ItemRarity::Uncommon;
    def.stackMax = 99;
    return def;
}())

REGISTER_ITEM("copper_ore", [] {
    core::ItemDef def;
    def.id = "copper_ore";
    def.description = "Cobre bruto, avermelhado.";
    def.spriteRows = kItemCopperSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemCopperPal;
    def.spritePalCount = kItemCopperPalCount;
    def.name = "Minério de Cobre";
    def.type = ItemType::Material;
    def.stackMax = 99;
    return def;
}())

REGISTER_ITEM("gold_ore", [] {
    core::ItemDef def;
    def.id = "gold_ore";
    def.description = "Ouro bruto. Brilha no escuro.";
    def.spriteRows = kItemGoldSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGoldPal;
    def.spritePalCount = kItemGoldPalCount;
    def.name = "Minério de Ouro";
    def.type = ItemType::Material;
    def.rarity = ItemRarity::Uncommon;
    def.stackMax = 99;
    return def;
}())

REGISTER_ITEM("potion", [] {
    core::ItemDef def;
    def.id = "potion";
    def.description = "Restaura 30 de HP ao usar.";
    def.spriteRows = kItemPotionSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemPotionPal;
    def.spritePalCount = kItemPotionPalCount;
    def.name = "Poção de Cura";
    def.type = ItemType::Consumable;
    def.rarity = ItemRarity::Uncommon;
    def.stackMax = 10;
    def.onUse = heal30;
    return def;
}())

REGISTER_ITEM("rusty_key", [] {
    core::ItemDef def;
    def.id = "rusty_key";
    def.description = "Abre algo enferrujado.";
    def.spriteRows = kItemKeySprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemKeyPal;
    def.spritePalCount = kItemKeyPalCount;
    def.name = "Chave Enferrujada";
    def.type = ItemType::Key;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 1; // não empilha: exercita o ramo stackMax == 1
    return def;
}())

REGISTER_ITEM("iron_sword", [] {
    core::ItemDef def;
    def.id = "iron_sword";
    def.description = "Espada de ferro. Corta slime.";
    def.spriteRows = kItemSwordSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemSwordPal;
    def.spritePalCount = kItemSwordPalCount;
    def.name = "Espada de Ferro";
    def.weight = 6.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.damage = 12;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Iron;
    return def;
}())

REGISTER_ITEM("iron_axe", [] {
    core::ItemDef def;
    def.id = "iron_axe";
    def.description = "Machado de ferro. Lento e brutal.";
    def.spriteRows = kItemAxeSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemAxePal;
    def.spritePalCount = kItemAxePalCount;
    def.name = "Machado de Ferro";
    def.weight = 8.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Uncommon;
    def.stackMax = 1;
    def.damage = 18;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Iron;
    return def;
}())

REGISTER_ITEM("iron_helm", [] {
    core::ItemDef def;
    def.id = "iron_helm";
    def.description = "Elmo de ferro. Protege a cuca.";
    def.spriteRows = kItemHelmSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemHelmPal;
    def.spritePalCount = kItemHelmPalCount;
    def.name = "Elmo de Ferro";
    def.weight = 4.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 4;
    def.equipSlot = core::EquipSlot::Head;
    def.material = core::MaterialId::Iron;
    return def;
}())

REGISTER_ITEM("iron_chest", [] {
    core::ItemDef def;
    def.id = "iron_chest";
    def.description = "Peitoral de ferro. Aguenta porrada.";
    def.spriteRows = kItemChestSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemChestPal;
    def.spritePalCount = kItemChestPalCount;
    def.name = "Peitoral de Ferro";
    def.weight = 9.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 6;
    def.equipSlot = core::EquipSlot::Chest;
    def.material = core::MaterialId::Iron;
    return def;
}())

REGISTER_ITEM("iron_legs", [] {
    core::ItemDef def;
    def.id = "iron_legs";
    def.description = "Perneiras de ferro. Correr cansa.";
    def.spriteRows = kItemLegsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemLegsPal;
    def.spritePalCount = kItemLegsPalCount;
    def.name = "Perneiras de Ferro";
    def.weight = 6.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 4;
    def.equipSlot = core::EquipSlot::Legs;
    def.material = core::MaterialId::Iron;
    return def;
}())

REGISTER_ITEM("leather_helm", [] {
    core::ItemDef def;
    def.id = "leather_helm";
    def.description = "Elmo de couro. Leve e barato.";
    def.spriteRows = kItemHelmSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemLeatherHelmPal;
    def.spritePalCount = kItemLeatherHelmPalCount;
    def.name = "Elmo de Couro";
    def.weight = 2.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 2;
    def.equipSlot = core::EquipSlot::Head;
    def.material = core::MaterialId::Leather;
    return def;
}())

REGISTER_ITEM("leather_chest", [] {
    core::ItemDef def;
    def.id = "leather_chest";
    def.description = "Peitoral de couro. Flexivel.";
    def.spriteRows = kItemChestSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemLeatherChestPal;
    def.spritePalCount = kItemLeatherChestPalCount;
    def.name = "Peitoral de Couro";
    def.weight = 5.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 4;
    def.equipSlot = core::EquipSlot::Chest;
    def.material = core::MaterialId::Leather;
    return def;
}())

REGISTER_ITEM("leather_legs", [] {
    core::ItemDef def;
    def.id = "leather_legs";
    def.description = "Perneiras de couro. Silenciosas.";
    def.spriteRows = kItemLegsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemLeatherLegsPal;
    def.spritePalCount = kItemLeatherLegsPalCount;
    def.name = "Perneiras de Couro";
    def.weight = 3.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 2;
    def.equipSlot = core::EquipSlot::Legs;
    def.material = core::MaterialId::Leather;
    return def;
}())

// ---- Matriz ouro/diamante + armas de couro (mesmo padrão) ----
REGISTER_ITEM("gold_sword", [] {
    core::ItemDef def;
    def.id = "gold_sword";
    def.description = "Espada de ouro. Brilha mais do que corta.";
    def.spriteRows = kItemSwordSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGoldSwordPal;
    def.spritePalCount = kItemGoldSwordPalCount;
    def.name = "Espada de Ouro";
    def.weight = 8.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 1;
    def.damage = 14;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Gold;
    return def;
}())

REGISTER_ITEM("gold_axe", [] {
    core::ItemDef def;
    def.id = "gold_axe";
    def.description = "Machado de ouro. Pesado e vistoso.";
    def.spriteRows = kItemAxeSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGoldAxePal;
    def.spritePalCount = kItemGoldAxePalCount;
    def.name = "Machado de Ouro";
    def.weight = 10.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 1;
    def.damage = 21;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Gold;
    return def;
}())

REGISTER_ITEM("gold_helm", [] {
    core::ItemDef def;
    def.id = "gold_helm";
    def.description = "Elmo de ouro. Chama atenção (inclusive de slime).";
    def.spriteRows = kItemHelmSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGoldArmorPal;
    def.spritePalCount = kItemGoldArmorPalCount;
    def.name = "Elmo de Ouro";
    def.weight = 5.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 1;
    def.defense = 5;
    def.equipSlot = core::EquipSlot::Head;
    def.material = core::MaterialId::Gold;
    return def;
}())

REGISTER_ITEM("gold_chest", [] {
    core::ItemDef def;
    def.id = "gold_chest";
    def.description = "Peitoral de ouro. Reluzente.";
    def.spriteRows = kItemChestSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGoldArmorPal;
    def.spritePalCount = kItemGoldArmorPalCount;
    def.name = "Peitoral de Ouro";
    def.weight = 12.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 1;
    def.defense = 8;
    def.equipSlot = core::EquipSlot::Chest;
    def.material = core::MaterialId::Gold;
    return def;
}())

REGISTER_ITEM("gold_legs", [] {
    core::ItemDef def;
    def.id = "gold_legs";
    def.description = "Perneiras de ouro. Barulhentas.";
    def.spriteRows = kItemLegsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGoldArmorPal;
    def.spritePalCount = kItemGoldArmorPalCount;
    def.name = "Perneiras de Ouro";
    def.weight = 8.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 1;
    def.defense = 5;
    def.equipSlot = core::EquipSlot::Legs;
    def.material = core::MaterialId::Gold;
    return def;
}())

REGISTER_ITEM("diamond_sword", [] {
    core::ItemDef def;
    def.id = "diamond_sword";
    def.description = "Espada de diamante. Corta quase tudo.";
    def.spriteRows = kItemSwordSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDiamondSwordPal;
    def.spritePalCount = kItemDiamondSwordPalCount;
    def.name = "Espada de Diamante";
    def.weight = 4.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 1;
    def.damage = 20;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Diamond;
    return def;
}())

REGISTER_ITEM("diamond_axe", [] {
    core::ItemDef def;
    def.id = "diamond_axe";
    def.description = "Machado de diamante. Fim de conversa.";
    def.spriteRows = kItemAxeSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDiamondAxePal;
    def.spritePalCount = kItemDiamondAxePalCount;
    def.name = "Machado de Diamante";
    def.weight = 6.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 1;
    def.damage = 30;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Diamond;
    return def;
}())

REGISTER_ITEM("diamond_helm", [] {
    core::ItemDef def;
    def.id = "diamond_helm";
    def.description = "Elmo de diamante. Praticamente eterno.";
    def.spriteRows = kItemHelmSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDiamondArmorPal;
    def.spritePalCount = kItemDiamondArmorPalCount;
    def.name = "Elmo de Diamante";
    def.weight = 3.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 1;
    def.defense = 8;
    def.equipSlot = core::EquipSlot::Head;
    def.material = core::MaterialId::Diamond;
    return def;
}())

REGISTER_ITEM("diamond_chest", [] {
    core::ItemDef def;
    def.id = "diamond_chest";
    def.description = "Peitoral de diamante. Muralha vestível.";
    def.spriteRows = kItemChestSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDiamondArmorPal;
    def.spritePalCount = kItemDiamondArmorPalCount;
    def.name = "Peitoral de Diamante";
    def.weight = 7.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 1;
    def.defense = 12;
    def.equipSlot = core::EquipSlot::Chest;
    def.material = core::MaterialId::Diamond;
    return def;
}())

REGISTER_ITEM("diamond_legs", [] {
    core::ItemDef def;
    def.id = "diamond_legs";
    def.description = "Perneiras de diamante. Inquebráveis.";
    def.spriteRows = kItemLegsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDiamondArmorPal;
    def.spritePalCount = kItemDiamondArmorPalCount;
    def.name = "Perneiras de Diamante";
    def.weight = 5.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 1;
    def.defense = 8;
    def.equipSlot = core::EquipSlot::Legs;
    def.material = core::MaterialId::Diamond;
    return def;
}())

REGISTER_ITEM("leather_sword", [] {
    core::ItemDef def;
    def.id = "leather_sword";
    def.description = "Espada de treino em couro. Não corta, ensina.";
    def.spriteRows = kItemSwordSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemLeatherSwordPal;
    def.spritePalCount = kItemLeatherSwordPalCount;
    def.name = "Espada de Couro";
    def.weight = 3.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.damage = 6;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Leather;
    return def;
}())

REGISTER_ITEM("leather_axe", [] {
    core::ItemDef def;
    def.id = "leather_axe";
    def.description = "Machado de treino em couro. Pesa pouco.";
    def.spriteRows = kItemAxeSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemLeatherAxePal;
    def.spritePalCount = kItemLeatherAxePalCount;
    def.name = "Machado de Couro";
    def.weight = 4.f;
    def.type = ItemType::Weapon;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.damage = 9;
    def.equipSlot = core::EquipSlot::RightHand;
    def.material = core::MaterialId::Leather;
    return def;
}())

// ---- Escada de bombas (mundo real, destruição crescente) ----
inline const core::PaletteEntry kItemTntPal[] = {
    {'.', {0, 0, 0, 0}},
    {'R', {220, 60, 60}},
    {'r', {150, 40, 40}},
    {'Y', {240, 200, 80}},
};
inline constexpr std::size_t kItemTntPalCount = 4;
inline const char* const kItemTntSprite[] = {
    "..RRRR..",
    "..RRRR..",
    "..YYYY..",
    "..RRRR..",
    "..RRRR..",
    "..YYYY..",
    "..RRRR..",
    "........",
};

inline const core::PaletteEntry kItemC4Pal[] = {
    {'.', {0, 0, 0, 0}},
    {'T', {200, 180, 130}},
    {'t', {140, 125, 90}},
    {'W', {150, 150, 150}},
};
inline constexpr std::size_t kItemC4PalCount = 4;
inline const char* const kItemC4Sprite[] = {
    "........",
    ".TTTTTT.",
    ".TTTTTT.",
    ".TTTTttT",
    ".WWWWW..",
    "..W.....",
    "..W.....",
    "........",
};

inline const core::PaletteEntry kItemMoabPal[] = {
    {'.', {0, 0, 0, 0}},
    {'O', {110, 120, 70}},
    {'o', {75, 85, 50}},
    {'F', {150, 150, 150}},
};
inline constexpr std::size_t kItemMoabPalCount = 4;
inline const char* const kItemMoabSprite[] = {
    "...OO...",
    "...OO...",
    "..OOOO..",
    "..OOOO..",
    ".FOOOOF.",
    ".FOOOOF.",
    "..o..o..",
    "........",
};

REGISTER_ITEM("tnt", [] {
    core::ItemDef def;
    def.id = "tnt";
    def.description = "Trinitrotolueno: 1kg equivale a 1kg de si mesmo.";
    def.spriteRows = kItemTntSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemTntPal;
    def.spritePalCount = kItemTntPalCount;
    def.name = "TNT";
    def.type = ItemType::Consumable;
    def.rarity = ItemRarity::Uncommon;
    def.stackMax = 999;
    def.throwable = true;
    def.throwKind = support::ThrowKind::Tnt;
    def.fuse = 1.0f;
    def.blastRadius = 70.f;
    def.blastDamage = 45;
    def.blastTiles = 5;
    return def;
}())

REGISTER_ITEM("c4", [] {
    core::ItemDef def;
    def.id = "c4";
    def.description = "C4 militar: plástico estável, boom ~1.3x a TNT.";
    def.spriteRows = kItemC4Sprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemC4Pal;
    def.spritePalCount = kItemC4PalCount;
    def.name = "Carga C4";
    def.type = ItemType::Consumable;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 999;
    def.throwable = true;
    def.throwKind = support::ThrowKind::C4;
    def.fuse = 0.8f;
    def.blastRadius = 110.f;
    def.blastDamage = 80;
    def.blastTiles = 7;
    return def;
}())

// ---- Degrau entre C4 e MOAB: BLU-82 Daisy Cutter ----
inline const core::PaletteEntry kItemDaisyPal[] = {
    {'.', {0, 0, 0, 0}},
    {'D', {190, 170, 90}},
    {'d', {130, 115, 60}},
    {'F', {150, 150, 150}},
};
inline constexpr std::size_t kItemDaisyPalCount = 4;
inline const char* const kItemDaisySprite[] = {
    "...DD...",
    "...DD...",
    "..DDDD..",
    "..DDDD..",
    "..DDDD..",
    "..DDDD..",
    "...dd...",
    "........",
};

REGISTER_ITEM("daisy", [] {
    core::ItemDef def;
    def.id = "daisy";
    def.description = "BLU-82 Daisy Cutter: abria clareiras na selva. Mãe da MOAB.";
    def.spriteRows = kItemDaisySprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDaisyPal;
    def.spritePalCount = kItemDaisyPalCount;
    def.name = "Daisy Cutter";
    def.type = ItemType::Consumable;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 999;
    def.throwable = true;
    def.throwKind = support::ThrowKind::Daisy;
    def.fuse = 1.2f;
    def.blastRadius = 400.f;
    def.blastDamage = 110;
    def.blastTiles = 12;
    return def;
}())

REGISTER_ITEM("moab", [] {
    core::ItemDef def;
    def.id = "moab";
    def.description = "MOAB, a 'mãe de todas as bombas'. Apaga um chunk inteiro do mapa.";
    def.spriteRows = kItemMoabSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemMoabPal;
    def.spritePalCount = kItemMoabPalCount;
    def.name = "MOAB";
    def.type = ItemType::Consumable;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 999;
    def.throwable = true;
    def.throwKind = support::ThrowKind::Moab;
    def.fuse = 1.5f;
    def.blastRadius = 800.f; // 16 tiles: engole um chunk inteiro (16×16)
    def.blastDamage = 140;
    def.blastTiles = 16;
    return def;
}())

// ---- Botas (slot Boots): mesma forma, paleta por material ----
inline const char* const kItemBootsSprite[] = {
    "........",
    "........",
    ".SS..SS.",
    ".SS..SS.",
    ".SS..SS.",
    ".SS..SS.",
    ".SSSSSSS",
    "........",
};

inline const core::PaletteEntry kItemIronBootsPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {150, 155, 165}},
    {'s', {105, 110, 120}},
};
inline constexpr std::size_t kItemIronBootsPalCount = 3;
inline const core::PaletteEntry kItemLeatherBootsPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {160, 110, 70}},
    {'s', {100, 70, 45}},
};
inline constexpr std::size_t kItemLeatherBootsPalCount = 3;
inline const core::PaletteEntry kItemGoldBootsPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {220, 190, 90}},
    {'s', {170, 140, 50}},
};
inline constexpr std::size_t kItemGoldBootsPalCount = 3;
inline const core::PaletteEntry kItemDiamondBootsPal[] = {
    {'.', {0, 0, 0, 0}},
    {'S', {180, 220, 240}},
    {'s', {100, 150, 190}},
};
inline constexpr std::size_t kItemDiamondBootsPalCount = 3;

REGISTER_ITEM("iron_boots", [] {
    core::ItemDef def;
    def.id = "iron_boots";
    def.description = "Botas de ferro. Firmes no chão.";
    def.spriteRows = kItemBootsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemIronBootsPal;
    def.spritePalCount = kItemIronBootsPalCount;
    def.name = "Botas de Ferro";
    def.weight = 3.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 3;
    def.equipSlot = core::EquipSlot::Boots;
    def.material = core::MaterialId::Iron;
    return def;
}())

REGISTER_ITEM("leather_boots", [] {
    core::ItemDef def;
    def.id = "leather_boots";
    def.description = "Botas de couro. Silenciosas.";
    def.spriteRows = kItemBootsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemLeatherBootsPal;
    def.spritePalCount = kItemLeatherBootsPalCount;
    def.name = "Botas de Couro";
    def.weight = 2.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 1;
    def.equipSlot = core::EquipSlot::Boots;
    def.material = core::MaterialId::Leather;
    return def;
}())

REGISTER_ITEM("gold_boots", [] {
    core::ItemDef def;
    def.id = "gold_boots";
    def.description = "Botas de ouro. Pesadas e vistosas.";
    def.spriteRows = kItemBootsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemGoldBootsPal;
    def.spritePalCount = kItemGoldBootsPalCount;
    def.name = "Botas de Ouro";
    def.weight = 4.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Rare;
    def.stackMax = 1;
    def.defense = 4;
    def.equipSlot = core::EquipSlot::Boots;
    def.material = core::MaterialId::Gold;
    return def;
}())

REGISTER_ITEM("diamond_boots", [] {
    core::ItemDef def;
    def.id = "diamond_boots";
    def.description = "Botas de diamante. Inquebráveis.";
    def.spriteRows = kItemBootsSprite;
    def.spriteW = 8;
    def.spriteH = 8;
    def.spritePal = kItemDiamondBootsPal;
    def.spritePalCount = kItemDiamondBootsPalCount;
    def.name = "Botas de Diamante";
    def.weight = 3.f;
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Epic;
    def.stackMax = 1;
    def.defense = 6;
    def.equipSlot = core::EquipSlot::Boots;
    def.material = core::MaterialId::Diamond;
    return def;
}())
