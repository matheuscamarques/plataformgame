#include "support/UI/InventoryUI.h"

#include <algorithm>
#include <string>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "core/ItemDef.h"
#include "support/Input/InputMap.h"
#include "support/UI/ItemIcon.h"

namespace support {

sf::Vector2f InventoryUI::gridOrigin(float w, float h) const {
    const float gw = kCols * kSlotSize + (kCols - 1) * kPad;
    const float gh = kRows * kSlotSize + (kRows - 1) * kPad;
    return {(w - gw) * 0.5f, (h - gh) * 0.5f};
}

sf::Vector2f InventoryUI::slotPos(int i, float w, float h) const {
    const int col = i % kCols;
    const int row = i / kCols;
    const sf::Vector2f o = gridOrigin(w, h);
    return {o.x + col * (kSlotSize + kPad), o.y + row * (kSlotSize + kPad)};
}

bool InventoryUI::handleInput(const InputMap& input, core::Inventory& inv) {
    if (!open_) return false;

    if (input.pressed(Action::ToggleInventory)) {
        close();
        return true;
    }
    if (input.pressed(Action::Pause)) {
        close();
        return true; // App também consome p/ não pausar por baixo
    }

    bool acted = false;
    if (input.pressed(Action::Left)) {
        cursor_ = (cursor_ + kSlots - 1) % kSlots;
        acted = true;
    }
    if (input.pressed(Action::Right)) {
        cursor_ = (cursor_ + 1) % kSlots;
        acted = true;
    }
    if (input.pressed(Action::Up)) {
        cursor_ = (cursor_ + kSlots - kCols) % kSlots;
        acted = true;
    }
    if (input.pressed(Action::Down)) {
        cursor_ = (cursor_ + kCols) % kSlots;
        acted = true;
    }

    if (input.pressed(Action::Interact)) {
        acted = true;
        if (heldSlot_ < 0) {
            if (!inv.slot(cursor_).isEmpty()) {
                heldItem_ = inv.slot(cursor_);
                inv.slot(cursor_) = core::Item{};
                heldSlot_ = cursor_;
            }
        } else {
            // Solta: troca com o cursor (o deslocado volta pra mão).
            // Mãos vazias = fim do arrasto; senão continua segurando.
            core::Item tmp = inv.slot(cursor_);
            inv.slot(cursor_) = heldItem_;
            heldItem_ = tmp;
            heldSlot_ = heldItem_.isEmpty() ? -1 : cursor_;
        }
    }
    return acted;
}

void InventoryUI::drawTooltip(sf::RenderTarget& target,
                              const core::ItemDef& def, int quantity,
                              sf::Vector2f anchor, float sw, float sh,
                              const sf::Font& font) const {
    sf::Text name, rarity, desc, quant;
    name.setFont(font);
    rarity.setFont(font);
    desc.setFont(font);
    quant.setFont(font);

    name.setString(def.name);
    name.setCharacterSize(16);
    name.setFillColor(sf::Color::White);
    rarity.setString(itemRarityName(def.rarity));
    rarity.setCharacterSize(12);
    rarity.setFillColor(itemRarityColor(def.rarity));
    desc.setString(def.description.empty() ? "--" : def.description);
    desc.setCharacterSize(12);
    desc.setFillColor(sf::Color(200, 200, 200));
    if (quantity > 1) {
        quant.setString("x" + std::to_string(quantity));
        quant.setCharacterSize(12);
        quant.setFillColor(sf::Color(220, 220, 220));
    }

    const auto bn = name.getLocalBounds();
    const auto br = rarity.getLocalBounds();
    const auto bd = desc.getLocalBounds();
    const auto bq = quant.getLocalBounds();
    const float w =
        std::max({bn.width, br.width, bd.width, bq.width}) + 16.f;
    const float h = bn.height + br.height + bd.height + bq.height + 24.f;

    sf::Vector2f p = anchor + sf::Vector2f(12.f, 12.f);
    if (p.x + w > sw) p.x = anchor.x - w - 12.f;
    if (p.y + h > sh) p.y = sh - h - 8.f;

    sf::RectangleShape bg({w, h});
    bg.setPosition(p);
    bg.setFillColor(sf::Color(20, 20, 30, 230));
    bg.setOutlineColor(sf::Color(90, 90, 110));
    bg.setOutlineThickness(1.f);
    target.draw(bg);

    float y = p.y + 8.f;
    name.setPosition(p.x + 8.f, y);
    target.draw(name);
    y += bn.height + 4.f;
    rarity.setPosition(p.x + 8.f, y);
    target.draw(rarity);
    y += br.height + 4.f;
    desc.setPosition(p.x + 8.f, y);
    target.draw(desc);
    if (quantity > 1) {
        y += bd.height + 4.f;
        quant.setPosition(p.x + 8.f, y);
        target.draw(quant);
    }
}

void InventoryUI::render(sf::RenderTarget& target, const core::Inventory& inv,
                         const sf::Font& font, float sw, float sh) const {
    if (!open_) return;

    sf::RectangleShape dim({sw, sh});
    dim.setFillColor(sf::Color(0, 0, 0, 140));
    target.draw(dim);

    // Cursor de teclado (sem mouse): highlight + tooltip no selecionado.
    for (int i = 0; i < kSlots; ++i) {
        const sf::Vector2f p = slotPos(i, sw, sh);
        const bool sel = (i == cursor_);

        sf::RectangleShape bg({kSlotSize, kSlotSize});
        bg.setPosition(p);
        bg.setFillColor(sel ? sf::Color(70, 70, 90, 240)
                            : sf::Color(40, 40, 50, 220));
        bg.setOutlineColor(sel ? sf::Color(255, 220, 100)
                               : sf::Color(90, 90, 100));
        bg.setOutlineThickness(sel ? 2.f : 1.f);
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
            sf::Text q;
            q.setFont(font);
            q.setString(std::to_string(item.quantity));
            q.setCharacterSize(12);
            q.setFillColor(sf::Color::White);
            q.setOutlineColor(sf::Color::Black);
            q.setOutlineThickness(1.f);
            q.setPosition(p.x + kSlotSize - 20.f, p.y + kSlotSize - 16.f);
            target.draw(q);
        }
    }

    if (heldSlot_ >= 0 && !heldItem_.isEmpty()) {
        if (const core::ItemDef* def = heldItem_.def()) {
            if (const sf::Texture* tex = itemIconFor(def)) {
                const sf::Vector2f p = slotPos(cursor_, sw, sh);
                sf::Sprite spr(*tex);
                const float scale = (kSlotSize - 12.f) / def->spriteW;
                spr.setScale(scale, scale);
                spr.setPosition(p.x + 6.f, p.y + 6.f);
                spr.setColor(sf::Color(255, 255, 255, 180));
                target.draw(spr);
            }
        }
    }

    if (heldSlot_ < 0) {
        const core::Item& item = inv.slot(cursor_);
        if (!item.isEmpty()) {
            if (const core::ItemDef* def = item.def()) {
                drawTooltip(target, *def, item.quantity,
                            slotPos(cursor_, sw, sh), sw, sh, font);
            }
        }
    }
}

} // namespace support
