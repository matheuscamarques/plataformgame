#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/sprite_from_ascii.h"

class Player;

namespace core {

// Tipo do item: dirige stacking, UI e o que "usar" significa.
enum class ItemType : uint8_t {
    Material,   // minério, madeira, gel — empilha, sem uso direto
    Consumable, // poção, comida — onUse consome 1
    Weapon,     // espada, machado — equipa no loadout
    Armor,      // elmo, peitoral — equipa no loadout
    Key,        // progressão — não empilha, não descarta
    Quest,      // missão — idem
};

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
