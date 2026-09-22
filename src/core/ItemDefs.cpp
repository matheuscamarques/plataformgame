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
    def.stackMax = 99;
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
    def.type = ItemType::Armor;
    def.rarity = ItemRarity::Common;
    def.stackMax = 1;
    def.defense = 4;
    def.equipSlot = core::EquipSlot::Legs;
    def.material = core::MaterialId::Iron;
    return def;
}())
