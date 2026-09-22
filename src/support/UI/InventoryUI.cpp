#include "support/UI/InventoryUI.h"

#include <string>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "core/ItemDef.h"
#include "entities/Player/Player.h"
#include "support/Input/InputMap.h"
#include "support/UI/ItemIcon.h"

namespace support {

const char* InventoryUI::tabName(int tab) {
    switch (tab) {
        case TabMaterial:   return "Material";
        case TabConsumable: return "Uso";
        case TabWeapon:     return "Arma";
        case TabArmor:      return "Armadura";
        case TabKey:        return "Chave";
        default:            return "Tudo";
    }
}

bool InventoryUI::matchesTab(const core::ItemDef* def, int tab) {
    if (tab == TabAll) return true;
    if (!def) return true; // vazio casa com toda aba (permite soltar)
    switch (tab) {
        case TabMaterial:   return def->type == core::ItemType::Material;
        case TabConsumable: return def->type == core::ItemType::Consumable;
        case TabWeapon:     return def->type == core::ItemType::Weapon;
        case TabArmor:      return def->type == core::ItemType::Armor;
        case TabKey:
            return def->type == core::ItemType::Key ||
                   def->type == core::ItemType::Quest;
        default: return true;
    }
}

bool InventoryUI::matchesSlot(const core::Inventory& inv, int index) const {
    if (index < 0 || index >= kSlots) return false;
    const core::Item& item = inv.slot(index);
    if (item.isEmpty()) return true;
    return matchesTab(item.def(), tab_);
}

void InventoryUI::stepCursor(int delta, const core::Inventory& inv) {
    for (int i = 0; i < kSlots; ++i) {
        cursor_ = (cursor_ + delta + kSlots) % kSlots;
        if (matchesSlot(inv, cursor_)) return;
    }
}

void InventoryUI::clampCursorToTab(const core::Inventory& inv) {
    if (matchesSlot(inv, cursor_)) return;
    for (int i = 0; i < kSlots; ++i) {
        if (matchesSlot(inv, i)) {
            cursor_ = i;
            return;
        }
    }
}

sf::Vector2f InventoryUI::gridOrigin(float w, float h) const {
    const float gw = kCols * kSlotSize + (kCols - 1) * kPad;
    const float gh = kRows * kSlotSize + (kRows - 1) * kPad;
    // Painel fixo à direita (~232px): desloca o grid p/ a esquerda.
    return {(w - gw) * 0.5f - 120.f, (h - gh) * 0.5f + 8.f};
}

sf::Vector2f InventoryUI::slotPos(int i, float w, float h) const {
    const int col = i % kCols;
    const int row = i / kCols;
    const sf::Vector2f o = gridOrigin(w, h);
    return {o.x + col * (kSlotSize + kPad), o.y + row * (kSlotSize + kPad)};
}

static const char* itemTypeName(core::ItemType t) {
    switch (t) {
        case core::ItemType::Material:   return "Material";
        case core::ItemType::Consumable: return "Consumivel";
        case core::ItemType::Weapon:     return "Arma";
        case core::ItemType::Armor:      return "Armadura";
        case core::ItemType::Key:        return "Chave";
        case core::ItemType::Quest:      return "Missao";
        default:                         return "?";
    }
}

bool InventoryUI::handleInput(const InputMap& input, core::Inventory& inv,
                              ::Player* player) {
    if (!open_) return false;

    if (input.pressed(Action::ToggleInventory)) {
        close();
        return true;
    }
    if (input.pressed(Action::Pause)) {
        close();
        return true; // App também consome p/ não pausar por baixo
    }

    // Abas (4c): consomem mesmo sem efeito (nada vaza p/ o jogo).
    if (input.pressed(Action::TabPrev)) {
        tab_ = (tab_ + TabCount - 1) % TabCount;
        clampCursorToTab(inv);
        return true;
    }
    if (input.pressed(Action::TabNext)) {
        tab_ = (tab_ + 1) % TabCount;
        clampCursorToTab(inv);
        return true;
    }
    // Reordena (4e): sort() da fase 1; cursor fica no índice.
    if (input.pressed(Action::ArrangeAll)) {
        if (heldSlot_ < 0) inv.sort();
        return true;
    }
    // Descarta (4e): Key/Quest bloqueiam; mão tem prioridade.
    if (input.pressed(Action::DropItem)) {
        if (heldSlot_ >= 0 && !heldItem_.isEmpty()) {
            pendingDrop_ = {heldItem_.defId, heldItem_.quantity};
            heldItem_ = core::Item{};
            heldSlot_ = -1;
        } else {
            core::Item& slot = inv.slot(cursor_);
            if (!slot.isEmpty()) {
                const core::ItemDef* def = slot.def();
                const bool locked =
                    def && (def->type == core::ItemType::Key ||
                            def->type == core::ItemType::Quest);
                if (!locked) {
                    pendingDrop_ = {slot.defId, slot.quantity};
                    slot = core::Item{};
                }
            }
        }
        return true;
    }
    // Usa/equipa (4e): consomível chama onUse e consome 1; arma/armadura
    // chama onEquip sem consumir. Sem player (teste puro): no-op consumido.
    if (input.pressed(Action::UseItem)) {
        if (heldSlot_ < 0 && player) {
            core::Item& slot = inv.slot(cursor_);
            if (!slot.isEmpty()) {
                if (const core::ItemDef* def = slot.def()) {
                    if (def->type == core::ItemType::Consumable && def->onUse) {
                        def->onUse(*player);
                        inv.remove(slot.defId, 1);
                    } else if ((def->type == core::ItemType::Weapon ||
                                def->type == core::ItemType::Armor) &&
                               def->onEquip) {
                        def->onEquip(*player);
                    }
                }
            }
        }
        return true;
    }

    bool acted = false;
    if (input.pressed(Action::Left)) {
        stepCursor(-1, inv);
        acted = true;
    }
    if (input.pressed(Action::Right)) {
        stepCursor(1, inv);
        acted = true;
    }
    if (input.pressed(Action::Up)) {
        stepCursor(-kCols, inv);
        acted = true;
    }
    if (input.pressed(Action::Down)) {
        stepCursor(kCols, inv);
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

void InventoryUI::drawTabBar(sf::RenderTarget& target, const sf::Font& font,
                             float sw, float sh) const {
    const sf::Vector2f o = gridOrigin(sw, sh);
    float x = o.x;
    const float y = o.y - 30.f;
    for (int t = 0; t < TabCount; ++t) {
        const bool sel = (t == tab_);
        sf::Text label;
        label.setFont(font);
        label.setString(tabName(t));
        label.setCharacterSize(13);
        label.setFillColor(sel ? sf::Color(255, 220, 100) : sf::Color(160, 160, 170));
        const float w = label.getLocalBounds().width + 16.f;
        sf::RectangleShape bg({w, 22.f});
        bg.setPosition(x, y);
        bg.setFillColor(sel ? sf::Color(70, 70, 90, 240)
                            : sf::Color(30, 30, 40, 220));
        bg.setOutlineColor(sel ? sf::Color(255, 220, 100)
                               : sf::Color(90, 90, 100));
        bg.setOutlineThickness(1.f);
        target.draw(bg);
        label.setPosition(x + 8.f, y + 2.f);
        target.draw(label);
        x += w + 4.f;
    }
}

void InventoryUI::drawDetailPanel(sf::RenderTarget& target,
                                  const core::Inventory& inv,
                                  const sf::Font& font, float sw,
                                  float sh) const {
    // Fixo à direita (4d): item do cursor (ou o da mão, se arrastando).
    const core::Item* shown = (heldSlot_ >= 0 && !heldItem_.isEmpty())
                                  ? &heldItem_
                                  : &inv.slot(cursor_);
    const core::ItemDef* def = shown->isEmpty() ? nullptr : shown->def();

    const float pw = 232.f;
    const float ph = 250.f;
    const float px = sw - pw - 16.f;
    const float py = (sh - ph) * 0.5f;

    sf::RectangleShape bg({pw, ph});
    bg.setPosition(px, py);
    bg.setFillColor(sf::Color(20, 20, 30, 235));
    bg.setOutlineColor(sf::Color(90, 90, 110));
    bg.setOutlineThickness(1.f);
    target.draw(bg);

    auto line = [&](const std::string& s, int size, sf::Color c, float& y) {
        sf::Text t;
        t.setFont(font);
        t.setString(s);
        t.setCharacterSize(static_cast<unsigned>(size));
        t.setFillColor(c);
        t.setPosition(px + 10.f, y);
        target.draw(t);
        y += t.getLocalBounds().height + static_cast<float>(size) * 0.9f;
    };

    float y = py + 10.f;
    if (!def) {
        line("-- vazio --", 14, sf::Color(150, 150, 160), y);
    } else {
        line(def->name, 16, sf::Color::White, y);
        line(itemRarityName(def->rarity), 12,
             itemRarityColor(def->rarity), y);
        line(itemTypeName(def->type), 12, sf::Color(170, 170, 180), y);
        if (def->damage > 0)
            line("Dano: " + std::to_string(def->damage), 13,
                 sf::Color(255, 150, 120), y);
        if (def->defense > 0)
            line("Defesa: " + std::to_string(def->defense), 13,
                 sf::Color(150, 200, 255), y);
        line(def->description.empty() ? "--" : def->description, 12,
             sf::Color(200, 200, 200), y);
        if (shown->quantity > 1)
            line("x" + std::to_string(shown->quantity), 12,
                 sf::Color(220, 220, 220), y);
    }
    y += 6.f;
    line("Q/Tab abas  R solta", 11, sf::Color(140, 140, 150), y);
    line("T organiza  U usa", 11, sf::Color(140, 140, 150), y);
}

void InventoryUI::render(sf::RenderTarget& target, const core::Inventory& inv,
                         const sf::Font& font, float sw, float sh) const {
    if (!open_) return;

    sf::RectangleShape dim({sw, sh});
    dim.setFillColor(sf::Color(0, 0, 0, 140));
    target.draw(dim);

    drawTabBar(target, font, sw, sh);

    // Cursor de teclado (sem mouse): highlight no selecionado; fora da
    // aba ativa desenha apagado (mas continua no slot — hotbar é índice).
    for (int i = 0; i < kSlots; ++i) {
        const sf::Vector2f p = slotPos(i, sw, sh);
        const bool sel = (i == cursor_);
        const bool inTab = matchesSlot(inv, i);

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
        sf::Color edge = itemRarityColor(def->rarity);
        if (!inTab) edge.a = 80;
        rb.setOutlineColor(edge);
        rb.setOutlineThickness(2.f);
        target.draw(rb);

        if (const sf::Texture* tex = itemIconFor(def)) {
            sf::Sprite spr(*tex);
            const float scale = (kSlotSize - 12.f) / def->spriteW;
            spr.setScale(scale, scale);
            spr.setPosition(p.x + 6.f, p.y + 6.f);
            if (!inTab) spr.setColor(sf::Color(255, 255, 255, 90));
            target.draw(spr);
        }

        if (item.quantity > 1) {
            sf::Text q;
            q.setFont(font);
            q.setString(std::to_string(item.quantity));
            q.setCharacterSize(12);
            q.setFillColor(inTab ? sf::Color::White
                                 : sf::Color(255, 255, 255, 90));
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

    drawDetailPanel(target, inv, font, sw, sh);
}

} // namespace support
