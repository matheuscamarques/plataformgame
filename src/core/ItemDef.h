/**
 * @file src/core/ItemDef.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Definição estática de itens com tipo, raridade e efeitos.
 * @details Descreve ItemDef com pilha, dano, defesa, sprite e callbacks com Player, usado por Item, Inventory e Equipment.
 */

#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/sprite_from_ascii.h"
#include "core/EquipSlot.h"
#include "core/Material.h"
#include "core/Attributes.h"
#include "support/Effects/Throwable.h"

class Player;

namespace core {

// Tipo do item: dirige stacking, UI e o que "usar" significa.
enum class ItemType : uint8_t {
    Material,   // minério, madeira, gel — empilha, sem uso direto
    Consumable, // poção, comida — onUse consome 1
    Weapon,     // espada, machado — equipa no Equipment
    Armor,      // elmo, peitoral — equipa no Equipment
    Spell,      // magia: sintoniza (F8), conjura com G (não equipa)
    Key,        // progressão — não empilha, não descarta
    Quest,      // missão — idem
};

// Comportamento de conjuração (F8b). None = não conjurável.
enum class SpellKind : uint8_t { None, Arrow, Heal };

enum class ItemRarity : uint8_t {
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary,
};

// Definição estática (dado compartilhado, nunca muda em runtime).
// Efeitos são callbacks com Player (não herança): poção cura,
// arma equipa. Sem Player (teste), onUse/onEquip podem ser nulos.
struct ItemDef {
    std::string id;
    std::string name;
    std::string description;
    ItemType    type      = ItemType::Material;
    ItemRarity  rarity    = ItemRarity::Common;
    uint16_t    stackMax  = 99;
    float       weight    = 0.f; // reservado (sem peso no MVP)
    int         damage    = 0;   // arma: painel 4d (0 = não mostra)
    int         defense   = 0;   // armadura: idem
    EquipSlot   equipSlot = EquipSlot::None; // None = não equipável
    MaterialId  material  = MaterialId::Iron; // cor da peça no render
    // Scaling DS (F6): bônus por atributo + requisitos. Sem req = -50%.
    ScaleGrade  strScale = ScaleGrade::None;
    ScaleGrade  dexScale = ScaleGrade::None;
    ScaleGrade  intScale = ScaleGrade::None;
    ScaleGrade  faiScale = ScaleGrade::None;
    int         strReq = 0;
    int         dexReq = 0;
    int         intReq = 0; // magia (F8): INT mínima p/ sintonizar/conjurar
    int         faiReq = 0; // idem FÉ
    SpellKind   spellKind = SpellKind::None; // Arrow/Heal (F8b)
    // Bomba arremessável (J joga o slot ativo da hotbar): stats vão
    // direto p/ o Throwable (fonte única, sem switch por id).
    bool                 throwable   = false;
    support::ThrowKind   throwKind   = support::ThrowKind::Dynamite;
    float                fuse        = 1.0f;
    float                blastRadius = 0.f; // px (0 = sem explosão)
    int                  blastDamage = 0;
    int                  blastTiles  = 0;   // raio de quebra em tiles
    // Sprite ASCII 8×8 (mesmo idioma de makeSprite; nulo = rect fallback).
    const char* const* spriteRows = nullptr;
    int spriteW = 0;
    int spriteH = 0;
    const PaletteEntry* spritePal = nullptr;
    std::size_t spritePalCount = 0;
    std::function<void(Player&)> onUse   = nullptr;
    std::function<void(Player&)> onEquip = nullptr;
};

// Registry global (mesmo padrão de ArchetypeRegistry/WeaponRegistry).
// Defs entram via REGISTER_ITEM em ItemDefs.cpp (estático, 1× no boot).
class ItemRegistry {
public:
    static ItemRegistry& instance() {
        static ItemRegistry r;
        return r;
    }

    void add(const std::string& key, ItemDef def) {
        if (!has(key)) keys_.push_back(key);
        items_[key] = std::move(def);
    }

    bool has(const std::string& key) const { return items_.count(key) > 0; }

    const ItemDef* find(const std::string& key) const {
        auto it = items_.find(key);
        return it == items_.end() ? nullptr : &it->second;
    }

    const std::vector<std::string>& keys() const { return keys_; }

private:
    std::unordered_map<std::string, ItemDef> items_;
    std::vector<std::string> keys_; // ordem determinística de registro
};

} // namespace core

#define _REG_ITEM_CONCAT(a, b) a##b
#define _REG_ITEM_TYPE(line) _REG_ITEM_CONCAT(AutoRegItem_, line)
#define _REG_ITEM_INST(line) _REG_ITEM_CONCAT(autoRegItemInstance_, line)
#define REGISTER_ITEM(key, ...)                                               \
    namespace {                                                               \
    struct _REG_ITEM_TYPE(__LINE__) {                                         \
        _REG_ITEM_TYPE(__LINE__)() {                                          \
            ::core::ItemRegistry::instance().add(key, __VA_ARGS__);           \
        }                                                                     \
    };                                                                        \
    static _REG_ITEM_TYPE(__LINE__) _REG_ITEM_INST(__LINE__);                 \
    }
