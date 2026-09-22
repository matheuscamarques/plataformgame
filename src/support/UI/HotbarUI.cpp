#include "support/UI/HotbarUI.h"

#include "core/ItemDef.h"
#include "support/Input/InputMap.h"
#include "support/UI/ItemIcon.h"

namespace support {

int HotbarUI::handleInput(const InputMap& input, int current) const {
    if (input.pressed(Action::Hotbar1)) return 0;
    if (input.pressed(Action::Hotbar2)) return 1;
    if (input.pressed(Action::Hotbar3)) return 2;
    if (input.pressed(Action::Hotbar4)) return 3;
    if (input.pressed(Action::Hotbar5)) return 4;
    return current;
}

void HotbarUI::render(sf::RenderTarget& target, const core::Inventory& inv,
                      int activeSlot, float screenW, float screenH,
                      const sf::Font& font) const {
    for (int i = 0; i < kSlots; ++i) {
        const sf::Vector2f p = slotPos(i, screenW, screenH);
        const bool active = (i == activeSlot);

        sf::RectangleShape bg({kSlotSize, kSlotSize});
        bg.setPosition(p);
        bg.setFillColor(active ? sf::Color(60, 60, 70, 220)
                               : sf::Color(30, 30, 40, 180));
        bg.setOutlineColor(active ? sf::Color(255, 220, 100)
                                  : sf::Color(80, 80, 90));
        bg.setOutlineThickness(active ? 2.f : 1.f);
        target.draw(bg);

        const core::Item& item = inv.slot(i);
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
            qty.setString(std::to_string(item.quantity));
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
