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
    static const sf::Color kColors[] = {
        {160, 160, 160}, // Common
        {80, 200, 80},   // Uncommon
        {80, 160, 240},  // Rare
        {200, 80, 240},  // Epic
        {255, 180, 60},  // Legendary
    };
    static_assert(sizeof(kColors) / sizeof(kColors[0]) ==
                      static_cast<std::size_t>(core::kItemRarityCount),
                  "itemRarityColor: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(r);
    if (i < 0 || i >= core::kItemRarityCount) return {160, 160, 160};
    return kColors[i];
}

inline const char* itemRarityName(core::ItemRarity r) {
    static constexpr const char* kNames[] = {
        "Common", "Uncommon", "Rare", "Epic", "Legendary",
    };
    static_assert(sizeof(kNames) / sizeof(kNames[0]) ==
                      static_cast<std::size_t>(core::kItemRarityCount),
                  "itemRarityName: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(r);
    if (i < 0 || i >= core::kItemRarityCount) return "?";
    return kNames[i];
}

} // namespace support
