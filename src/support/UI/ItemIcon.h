/**
 * @file src/support/UI/ItemIcon.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Fornece ícone, cor e nome por raridade para UIs.
 * @details Define itemIconFor com cache de texturas geradas de spriteRows mais itemRarityColor e itemRarityName, usada por HotbarUI e InventoryUI só no render com GL.
 */

#pragma once
#include <string>
#include <unordered_map>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "core/ItemDef.h"
#include "core/sprite_from_ascii.h"

// Ícone + cor/nome de raridade compartilhados (HotbarUI, InventoryUI).
// Texturas geradas 1× sob demanda (GL só no render, nunca em teste).
namespace support {

inline const sf::Texture* itemIconFor(const core::ItemDef* def) {
    static std::unordered_map<std::string, sf::Texture> cache;
    if (!def || !def->spriteRows) return nullptr;
    auto it = cache.find(def->id);
    if (it != cache.end()) return &it->second;
    sf::Texture t = core::makeSprite(def->spriteRows, def->spriteW,
                                     def->spriteH, def->spritePal,
                                     def->spritePalCount);
    auto [ins, _] = cache.emplace(def->id, std::move(t));
    return &ins->second;
}

inline sf::Color itemRarityColor(core::ItemRarity r) {
    using core::ItemRarity;
    switch (r) {
        case ItemRarity::Common:    return {160, 160, 160};
        case ItemRarity::Uncommon:  return {80, 200, 80};
        case ItemRarity::Rare:      return {80, 160, 240};
        case ItemRarity::Epic:      return {200, 80, 240};
        case ItemRarity::Legendary: return {255, 180, 60};
    }
    return {160, 160, 160};
}

inline const char* itemRarityName(core::ItemRarity r) {
    using core::ItemRarity;
    switch (r) {
        case ItemRarity::Common:    return "Common";
        case ItemRarity::Uncommon:  return "Uncommon";
        case ItemRarity::Rare:      return "Rare";
        case ItemRarity::Epic:      return "Epic";
        case ItemRarity::Legendary: return "Legendary";
    }
    return "?";
}

} // namespace support
