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
#include "entities/Player/Player.h"
#include "core/ItemDef.h"
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

void HotbarUI::renderBelt(sf::RenderTarget& target, const ::Player& player,
                       int activeSlot, float screenW, float screenH,
                       const sf::Font& font) const {
    // Cinto DS abaixo do Souls/HP (topo-esquerda, zona visível
    // comprovada; fundo usa viewH_ fora da dobra em algumas views).
    const core::Inventory& inv = player.inventory;
    const float y0 = 200.f;
    const float x0 = 16.f;

    auto key = [&](const std::string& s, float x, float y) {
        sf::Text t;
        t.setFont(font);
        t.setString(support::utf8(s));
        t.setCharacterSize(12);
        t.setFillColor(sf::Color(160, 160, 160));
        t.setPosition(x, y);
        target.draw(t);
    };
    auto box = [&](float x, const core::ItemDef* def, bool active,
                   int qty, const std::string& sub) {
        sf::RectangleShape bg({kSlotSize, kSlotSize});
        bg.setPosition(x, y0);
        bg.setFillColor(active ? sf::Color(60, 60, 70, 220)
                               : sf::Color(30, 30, 40, 180));
        bg.setOutlineColor(active ? sf::Color(255, 220, 100)
                                  : sf::Color(80, 80, 90));
        bg.setOutlineThickness(active ? 2.f : 1.f);
        target.draw(bg);
        if (!def) return;
        sf::RectangleShape rb({kSlotSize - 4.f, kSlotSize - 4.f});
        rb.setPosition(x + 2.f, y0 + 2.f);
        rb.setFillColor(sf::Color::Transparent);
        rb.setOutlineColor(itemRarityColor(def->rarity));
        rb.setOutlineThickness(2.f);
        target.draw(rb);
        if (const sf::Texture* tex = itemIconFor(def)) {
            sf::Sprite spr(*tex);
            const float scale = (kSlotSize - 12.f) / def->spriteW;
            spr.setScale(scale, scale);
            spr.setPosition(x + 6.f, y0 + 6.f);
            target.draw(spr);
        }
        if (qty > 1 || !sub.empty()) {
            sf::Text t;
            t.setFont(font);
            t.setString(support::utf8(qty > 1 ? ("x" + std::to_string(qty) +
                                                 (sub.empty() ? "" : " " + sub))
                                              : sub));
            t.setCharacterSize(11);
            t.setFillColor(sf::Color::White);
            t.setOutlineColor(sf::Color::Black);
            t.setOutlineThickness(1.f);
            const auto b = t.getLocalBounds();
            t.setPosition(x + kSlotSize - b.width - 3.f,
                          y0 + kSlotSize - b.height - 4.f);
            target.draw(t);
        }
    };

    // [Z] mão esquerda.
    const core::Item& lh = player.equipment.get(core::EquipSlot::LeftHand);
    key("[Z]", x0, y0 - 16.f);
    box(x0, lh.isEmpty() ? nullptr : lh.def(), false, 0, "");
    // [X] mão direita (ativa: borda ouro).
    const core::Item& rh = player.equipment.get(core::EquipSlot::RightHand);
    key("[X]", x0 + (kSlotSize + kPad), y0 - 16.f);
    box(x0 + (kSlotSize + kPad), rh.isEmpty() ? nullptr : rh.def(), true, 0,
        "");
    // [C] magia sintonizada (attuned[0]).
    const core::ItemDef* spell = nullptr;
    if (!player.attuned.empty())
        spell = core::ItemRegistry::instance().find(player.attuned[0]);
    key("[C]", x0 + 2 * (kSlotSize + kPad), y0 - 16.f);
    box(x0 + 2 * (kSlotSize + kPad), spell, !player.attuned.empty(), 0, "");
    // [V] item ativo da hotbar (qtd + n/5).
    const int real = realSlot(inv, activeSlot);
    const core::ItemDef* idef = nullptr;
    int qty = 0;
    if (real >= 0) {
        const core::Item& it = inv.slot(real);
        idef = it.def();
        qty = it.quantity;
    }
    key("[V]", x0 + 3 * (kSlotSize + kPad), y0 - 16.f);
    box(x0 + 3 * (kSlotSize + kPad), idef, true, qty,
        std::to_string(activeSlot + 1) + "/5");

    (void)screenW;
}

} // namespace support
