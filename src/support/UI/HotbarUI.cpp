/**
 * @file src/support/UI/HotbarUI.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Renderiza e troca os cinco slots rápidos do inventário.
 * @details Implementa handleInput para teclas 1 a 5 e render com fundo, seleção e ícones via ItemIcon, chamada todo frame pelo Game Renderer com inventário do Player.
 */

#include "support/UI/HotbarUI.h"

#include "core/ItemDef.h"
#include "support/Input/InputMap.h"
#include "support/UI/ItemIcon.h"
#include "support/SfString.h"

namespace support {

int HotbarUI::handleInput(const InputMap& input, int current) const {
    if (input.pressed(Action::Hotbar1)) return 0;
    if (input.pressed(Action::Hotbar2)) return 1;
    if (input.pressed(Action::Hotbar3)) return 2;
    if (input.pressed(Action::Hotbar4)) return 3;
    if (input.pressed(Action::Hotbar5)) return 4;
    return current;
}

bool HotbarUI::showsItem(const core::ItemDef* def) {
    if (!def) return false;
    return def->type == core::ItemType::Consumable || def->throwable;
}

std::vector<int> HotbarUI::filteredSlots(const core::Inventory& inv) {
    std::vector<int> out;
    for (int i = 0; i < core::Inventory::kCapacity &&
            static_cast<int>(out.size()) < kSlots;
         ++i) {
        const core::Item& item = inv.slot(i);
        if (item.isEmpty()) continue;
        if (showsItem(item.def())) out.push_back(i);
    }
    return out;
}

int HotbarUI::realSlot(const core::Inventory& inv, int active) {
    const std::vector<int> slots = filteredSlots(inv);
    if (active < 0 || active >= static_cast<int>(slots.size())) return -1;
    return slots[active];
}

void HotbarUI::render(sf::RenderTarget& target, const core::Inventory& inv,
                       int activeSlot, float screenW, float screenH,
                       const sf::Font& font) const {
    const std::vector<int> slots = filteredSlots(inv);
    for (int i = 0; i < kSlots; ++i) {
        const core::Vec2f p = slotPos(i, screenW, screenH);
        const bool active = (i == activeSlot);

        sf::RectangleShape bg({kSlotSize, kSlotSize});
        bg.setPosition(p.x, p.y);
        bg.setFillColor(active ? sf::Color(60, 60, 70, 220)
                               : sf::Color(30, 30, 40, 180));
        bg.setOutlineColor(active ? sf::Color(255, 220, 100)
                                  : sf::Color(80, 80, 90));
        bg.setOutlineThickness(active ? 2.f : 1.f);
        target.draw(bg);

        if (i >= static_cast<int>(slots.size())) continue;
        const core::Item& item = inv.slot(slots[i]);
        if (item.isEmpty()) continue;
        const core::ItemDef* def = item.def();
        if (!def) continue;

        sf::RectangleShape rb({kSlotSize - 4.f, kSlotSize - 4.f});
        rb.setPosition(p.x + 2.f, p.y + 2.f);
        rb.setFillColor(sf::Color::Transparent);
        rb.setOutlineColor(itemRarityColor(def->rarity));
        rb.setOutlineThickness(2.f);
        target.draw(rb);

        if (const sf::Texture* tex = itemIconFor(def)) {
            sf::Sprite spr(*tex);
            const float scale = (kSlotSize - 12.f) / def->spriteW;
            spr.setScale(scale, scale);
            spr.setPosition(p.x + 6.f, p.y + 6.f);
            target.draw(spr);
        }

        if (item.quantity > 1) {
            sf::Text qty;
            qty.setFont(font);
            qty.setString(support::utf8(std::to_string(item.quantity)));
            qty.setCharacterSize(12);
            qty.setFillColor(sf::Color::White);
            qty.setOutlineColor(sf::Color::Black);
            qty.setOutlineThickness(1.f);
            const auto b = qty.getLocalBounds();
            qty.setPosition(p.x + kSlotSize - b.width - 4.f,
                            p.y + kSlotSize - b.height - 6.f);
            target.draw(qty);
        }
    }
}

} // namespace support
