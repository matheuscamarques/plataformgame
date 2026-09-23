#include "support/UI/InventoryUI.h"

#include <algorithm>
#include <sstream>
#include <string>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "core/ItemDef.h"
#include "core/Material.h"
#include "entities/Player/Player.h"
#include "support/Input/InputMap.h"
#include "support/Progression/DropSystem.h"
#include "support/UI/ItemIcon.h"

namespace support {

// ─── Nomes / filtro ──────────────────────────────────────────

const char* InventoryUI::mainTabName(MainTab t) {
    return t == MainTab::Equipment ? "Equipment" : "Inventory";
}

const char* InventoryUI::subTabName(SubTab s) {
    switch (s) {
        case SubTab::Materials:   return "Mat";
        case SubTab::Consumables: return "Cons";
        case SubTab::Weapons:     return "Wpn";
        case SubTab::Armor:       return "Arm";
        case SubTab::Keys:        return "Key";
        default:                  return "All";
    }
}

const char* InventoryUI::menuActionName(MenuAction a) {
    switch (a) {
        case MenuAction::Use:     return "Use";
        case MenuAction::Equip:   return "Equip";
        case MenuAction::Unequip: return "Unequip";
        case MenuAction::Drop:    return "Drop";
        case MenuAction::Arrange: return "Arrange";
        default:                  return "?";
    }
}

bool InventoryUI::matchesSubTab(const core::ItemDef* def, SubTab sub) {
    if (sub == SubTab::All) return true;
    if (!def) return false;
    using T = core::ItemType;
    switch (sub) {
        case SubTab::Materials:   return def->type == T::Material;
        case SubTab::Consumables: return def->type == T::Consumable;
        case SubTab::Weapons:     return def->type == T::Weapon;
        case SubTab::Armor:       return def->type == T::Armor;
        case SubTab::Keys:
            return def->type == T::Key || def->type == T::Quest;
        default: return true;
    }
}

// ─── Estado ──────────────────────────────────────────────────

void InventoryUI::open() {
    state_ = UIState::Browse;
    mainTab_ = MainTab::Inventory;
    subTab_ = SubTab::All;
    cursor_ = 0;
    equipCursor_ = 0;
}

void InventoryUI::close() { state_ = UIState::Closed; }

void InventoryUI::toggle() {
    if (state_ == UIState::Closed) open();
    else close();
}

void InventoryUI::setCursor(int i) {
    cursor_ = std::clamp(i, 0, kSlots - 1);
}

std::vector<int> InventoryUI::filteredSlots() const {
    std::vector<int> out;
    if (!inv_) return out;
    for (int i = 0; i < core::Inventory::kCapacity; ++i) {
        const core::Item& item = inv_->slot(i);
        if (item.isEmpty()) continue;
        if (matchesSubTab(item.def(), subTab_)) out.push_back(i);
    }
    return out;
}

std::vector<InventoryUI::MenuAction> InventoryUI::menuActions() const {
    std::vector<MenuAction> out;
    if (!inv_) return out;
    if (mainTab_ == MainTab::Equipment) {
        // F sobre item equipado: Unequip como única opção.
        if (equipment_) {
            const auto s = static_cast<core::EquipSlot>(equipCursor_ + 1);
            if (!equipment_->get(s).isEmpty()) out.push_back(MenuAction::Unequip);
        }
        return out;
    }
    if (cursor_ < 0 || cursor_ >= kSlots) return out;
    const core::Item& item = inv_->slot(cursor_);
    if (item.isEmpty()) return out;
    const core::ItemDef* def = item.def();
    if (!def) return out;
    using T = core::ItemType;
    if (def->onUse) out.push_back(MenuAction::Use);
    if (equipment_ && def->equipSlot != core::EquipSlot::None)
        out.push_back(MenuAction::Equip);
    if (def->type != T::Key && def->type != T::Quest)
        out.push_back(MenuAction::Drop);
    out.push_back(MenuAction::Arrange); // sempre disponível
    return out;
}

// ─── Ações ───────────────────────────────────────────────────

void InventoryUI::executeAction(MenuAction action) {
    if (!inv_) return;

    if (action == MenuAction::Unequip) {
        if (mainTab_ != MainTab::Equipment || !equipment_) return;
        const auto s = static_cast<core::EquipSlot>(equipCursor_ + 1);
        core::Item removed = equipment_->unequip(s);
        if (removed.isEmpty()) return;
        const int leftover = inv_->add(removed);
        if (leftover > 0) {
            removed.quantity = static_cast<uint16_t>(leftover);
            equipment_->equip(removed); // grid cheio: devolve
        }
        return;
    }

    if (mainTab_ != MainTab::Inventory) return;
    if (cursor_ < 0 || cursor_ >= kSlots) return;
    core::Item& item = inv_->slot(cursor_);
    if (item.isEmpty()) return;
    const core::ItemDef* def = item.def();
    if (!def) return;

    switch (action) {
        case MenuAction::Use:
            if (def->onUse && player_) {
                def->onUse(*player_);
                inv_->remove(def->id, 1);
            }
            break;
        case MenuAction::Equip: {
            if (!equipment_) return;
            if (def->equipSlot == core::EquipSlot::None) return;
            core::Item old;
            if (equipment_->equip(item, &old)) {
                item = old; // cursor recebe o antigo (ou esvazia)
            }
            break;
        }
        case MenuAction::Drop: {
            using T = core::ItemType;
            if (!drops_ || !player_) return;
            if (def->type == T::Key || def->type == T::Quest) return;
            drops_->spawnItem(def->id, item.quantity,
                              {player_->getCenterX(), player_->getCenterY()});
            item = core::Item{};
            break;
        }
        case MenuAction::Arrange:
            inv_->sort();
            break;
        default:
            break;
    }
}

// ─── Input ───────────────────────────────────────────────────

bool InventoryUI::handleInput(const InputMap& input) {
    if (state_ == UIState::Closed) return false;
    switch (state_) {
        case UIState::Browse:      handleBrowse(input); break;
        case UIState::ActionMenu:  handleActionMenu(input); break;
        case UIState::ConfirmDrop: handleConfirmDrop(input); break;
        default: break;
    }
    return true; // aberto: sempre consome (App faz o consume)
}

bool InventoryUI::slotMatches(int index) const {
    if (!inv_ || index < 0 || index >= kSlots) return false;
    const core::Item& item = inv_->slot(index);
    if (item.isEmpty()) return true; // vazio navega em toda aba
    return matchesSubTab(item.def(), subTab_);
}

void InventoryUI::stepCursor(int delta) {
    for (int i = 0; i < kSlots; ++i) {
        cursor_ = (cursor_ + delta + kSlots) % kSlots;
        if (slotMatches(cursor_)) return;
    }
}

void InventoryUI::cycleMainTab(int delta) {
    constexpr int n = static_cast<int>(MainTab::COUNT);
    mainTab_ = static_cast<MainTab>(
        (static_cast<int>(mainTab_) + delta + n) % n);
    cursor_ = 0;
    equipCursor_ = 0;
}

void InventoryUI::cycleSubTab(int delta) {
    constexpr int n = static_cast<int>(SubTab::COUNT);
    subTab_ = static_cast<SubTab>(
        (static_cast<int>(subTab_) + delta + n) % n);
    cursor_ = 0;
}

void InventoryUI::openActionMenu() {
    if (menuActions().empty()) return;
    actionCursor_ = 0;
    state_ = UIState::ActionMenu;
}

void InventoryUI::handleBrowse(const InputMap& input) {
    // Fechar (App consome o edge antes do RunManager; aqui é p/ teste).
    if (input.pressed(Action::ToggleInventory) || input.pressed(Action::Pause)) {
        close();
        return;
    }
    // Tabs principais.
    if (input.pressed(Action::TabLeft)) {
        cycleMainTab(-1);
        return;
    }
    if (input.pressed(Action::TabRight)) {
        cycleMainTab(1);
        return;
    }
    // Sub-tabs (só em Inventory; antes das setas: A/D também movem).
    if (mainTab_ == MainTab::Inventory) {
        if (input.pressed(Action::SubTabLeft)) {
            cycleSubTab(-1);
            return;
        }
        if (input.pressed(Action::SubTabRight)) {
            cycleSubTab(1);
            return;
        }
    }

    if (mainTab_ == MainTab::Equipment) {
        if (input.pressed(Action::Up) || input.pressed(Action::Left))
            equipCursor_ = (equipCursor_ + 3) % 4;
        if (input.pressed(Action::Down) || input.pressed(Action::Right))
            equipCursor_ = (equipCursor_ + 1) % 4;
        if (input.pressed(Action::Interact)) openActionMenu();
        return;
    }

    // Grid: atalhos + Home/End + setas + F.
    if (input.pressed(Action::FirstSlot)) {
        cursor_ = 0;
        return;
    }
    if (input.pressed(Action::LastSlot)) {
        cursor_ = kSlots - 1;
        return;
    }
    if (input.pressed(Action::ArrangeAll)) {
        if (inv_) inv_->sort();
        return;
    }
    if (input.pressed(Action::UseItem)) {
        executeAction(MenuAction::Use);
        return;
    }
    if (input.pressed(Action::Left)) stepCursor(-1);
    if (input.pressed(Action::Right)) stepCursor(1);
    if (input.pressed(Action::Up)) stepCursor(-kCols);
    if (input.pressed(Action::Down)) stepCursor(kCols);
    if (input.pressed(Action::Interact)) openActionMenu();
}

void InventoryUI::handleActionMenu(const InputMap& input) {
    if (input.pressed(Action::Pause)) {
        state_ = UIState::Browse; // Esc volta, não fecha
        return;
    }
    const auto actions = menuActions();
    if (actions.empty()) {
        state_ = UIState::Browse;
        return;
    }
    const int n = static_cast<int>(actions.size());
    if (input.pressed(Action::Up))
        actionCursor_ = (actionCursor_ + n - 1) % n;
    if (input.pressed(Action::Down))
        actionCursor_ = (actionCursor_ + 1) % n;
    if (input.pressed(Action::Interact)) {
        if (actions[actionCursor_] == MenuAction::Drop) {
            state_ = UIState::ConfirmDrop;
            return;
        }
        executeAction(actions[actionCursor_]);
        state_ = UIState::Browse;
    }
}

void InventoryUI::handleConfirmDrop(const InputMap& input) {
    if (input.pressed(Action::Pause)) {
        state_ = UIState::Browse; // Esc cancela, não fecha
        return;
    }
    if (input.pressed(Action::Interact)) {
        executeAction(MenuAction::Drop);
        state_ = UIState::Browse;
    }
}

// ─── Layout ──────────────────────────────────────────────────

sf::Vector2f InventoryUI::gridOrigin(float sw, float sh) const {
    const float gw = kCols * kSlotSize + (kCols - 1) * kPad;
    const float gh = kRows * kSlotSize + (kRows - 1) * kPad;
    // Bloco grid + vão + painel centralizado; grade sobe p/ tabs.
    const float totalW = gw + 16.f + kDetailW;
    (void)sh;
    return {(sw - totalW) * 0.5f, 110.f};
}

sf::Vector2f InventoryUI::slotPos(int i, float sw, float sh) const {
    const int col = i % kCols;
    const int row = i / kCols;
    const sf::Vector2f o = gridOrigin(sw, sh);
    return {o.x + col * (kSlotSize + kPad), o.y + row * (kSlotSize + kPad)};
}

sf::Vector2f InventoryUI::detailOrigin(float sw, float sh) const {
    const sf::Vector2f o = gridOrigin(sw, sh);
    const float gw = kCols * kSlotSize + (kCols - 1) * kPad;
    return {o.x + gw + 16.f, 110.f};
}

// ─── Render ──────────────────────────────────────────────────

void InventoryUI::render(sf::RenderTarget& target, const sf::Font& font,
                         float sw, float sh) {
    if (state_ == UIState::Closed) return;

    sf::RectangleShape dim({sw, sh});
    dim.setFillColor(sf::Color(0, 0, 0, 180));
    target.draw(dim);

    renderMainTabs(target, sw, font);
    if (mainTab_ == MainTab::Inventory) {
        renderSubTabs(target, sw, font);
        renderGrid(target, sw, sh, font);
    } else {
        renderEquipTab(target, sw, sh, font);
    }
    renderDetailPanel(target, sw, sh, font);
    renderFooter(target, sw, sh, font);
    if (state_ == UIState::ActionMenu) renderActionMenu(target, sw, sh, font);
    if (state_ == UIState::ConfirmDrop)
        renderConfirmDrop(target, sw, sh, font);
}

void InventoryUI::renderMainTabs(sf::RenderTarget& t, float sw,
                                 const sf::Font& font) const {
    constexpr int n = static_cast<int>(MainTab::COUNT);
    const float tabW = 150.f, tabH = 36.f, gap = 4.f;
    const float x0 = (sw - (n * tabW + (n - 1) * gap)) * 0.5f;
    for (int i = 0; i < n; ++i) {
        const bool active = (static_cast<int>(mainTab_) == i);
        sf::RectangleShape bg({tabW, tabH});
        bg.setPosition(x0 + i * (tabW + gap), 16.f);
        bg.setFillColor(active ? sf::Color(80, 70, 50, 240)
                               : sf::Color(30, 30, 40, 220));
        bg.setOutlineColor(active ? sf::Color(255, 220, 100)
                                  : sf::Color(80, 80, 90));
        bg.setOutlineThickness(active ? 2.f : 1.f);
        t.draw(bg);
        sf::Text txt;
        txt.setFont(font);
        txt.setString(mainTabName(static_cast<MainTab>(i)));
        txt.setCharacterSize(16);
        txt.setFillColor(active ? sf::Color(255, 240, 200)
                                : sf::Color(160, 160, 160));
        const auto b = txt.getLocalBounds();
        txt.setPosition(x0 + i * (tabW + gap) + (tabW - b.width) * 0.5f,
                        16.f + (tabH - b.height) * 0.5f - 4.f);
        t.draw(txt);
    }
}

void InventoryUI::renderSubTabs(sf::RenderTarget& t, float sw,
                                const sf::Font& font) const {
    constexpr int n = static_cast<int>(SubTab::COUNT);
    const float tabW = 110.f, tabH = 28.f, gap = 4.f;
    const float x0 = (sw - (n * tabW + (n - 1) * gap)) * 0.5f;
    for (int i = 0; i < n; ++i) {
        const bool active = (static_cast<int>(subTab_) == i);
        sf::RectangleShape bg({tabW, tabH});
        bg.setPosition(x0 + i * (tabW + gap), 64.f);
        bg.setFillColor(active ? sf::Color(60, 60, 80, 240)
                               : sf::Color(25, 25, 35, 200));
        bg.setOutlineColor(active ? sf::Color(200, 180, 100)
                                  : sf::Color(70, 70, 80));
        bg.setOutlineThickness(1.f);
        t.draw(bg);
        sf::Text txt;
        txt.setFont(font);
        txt.setString(subTabName(static_cast<SubTab>(i)));
        txt.setCharacterSize(13);
        txt.setFillColor(active ? sf::Color(240, 220, 160)
                                : sf::Color(150, 150, 150));
        const auto b = txt.getLocalBounds();
        txt.setPosition(x0 + i * (tabW + gap) + (tabW - b.width) * 0.5f,
                        64.f + (tabH - b.height) * 0.5f - 3.f);
        t.draw(txt);
    }
}

void InventoryUI::renderGrid(sf::RenderTarget& t, float sw, float sh,
                             const sf::Font& font) const {
    if (!inv_) return;

    if (filteredSlots().empty()) {
        sf::Text msg;
        msg.setFont(font);
        msg.setString("Nenhum item nesta categoria");
        msg.setCharacterSize(14);
        msg.setFillColor(sf::Color(150, 150, 150));
        const sf::Vector2f o = gridOrigin(sw, sh);
        msg.setPosition(o.x + 40.f, o.y + 120.f);
        t.draw(msg);
    }

    for (int i = 0; i < kSlots; ++i) {
        const sf::Vector2f p = slotPos(i, sw, sh);
        const bool sel = (i == cursor_);
        const bool inTab = slotMatches(i);

        sf::RectangleShape bg({kSlotSize, kSlotSize});
        bg.setPosition(p);
        bg.setFillColor(sel ? sf::Color(70, 70, 90, 240)
                            : sf::Color(40, 40, 50, 220));
        bg.setOutlineColor(sel ? sf::Color(255, 220, 100)
                               : sf::Color(90, 90, 100));
        bg.setOutlineThickness(sel ? 2.f : 1.f);
        t.draw(bg);

        const core::Item& item = inv_->slot(i);
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
        t.draw(rb);

        if (const sf::Texture* tex = itemIconFor(def)) {
            sf::Sprite spr(*tex);
            const float scale = (kSlotSize - 12.f) / def->spriteW;
            spr.setScale(scale, scale);
            spr.setPosition(p.x + 6.f, p.y + 6.f);
            if (!inTab) spr.setColor(sf::Color(255, 255, 255, 90));
            t.draw(spr);
        }

        if (item.quantity > 1) {
            sf::Text q;
            q.setFont(font);
            q.setString(std::to_string(item.quantity));
            q.setCharacterSize(13);
            q.setFillColor(inTab ? sf::Color::White
                                 : sf::Color(255, 255, 255, 90));
            q.setOutlineColor(sf::Color::Black);
            q.setOutlineThickness(1.f);
            q.setPosition(p.x + kSlotSize - 22.f, p.y + kSlotSize - 18.f);
            t.draw(q);
        }
    }
}

void InventoryUI::renderEquipTab(sf::RenderTarget& t, float sw, float sh,
                                 const sf::Font& font) const {
    // 4 slots de 96px: RightHand à esquerda, Head/Chest/Legs em coluna.
    const sf::Vector2f o = gridOrigin(sw, sh);
    const float gw = kCols * kSlotSize + (kCols - 1) * kPad;
    const float cx = o.x + gw * 0.5f;
    const float cy = o.y + 150.f;
    const sf::Vector2f pos[4] = {
        {cx - 140.f, cy - 48.f},
        {cx + 40.f, cy - 168.f},
        {cx + 40.f, cy - 48.f},
        {cx + 40.f, cy + 72.f},
    };
    for (int i = 0; i < 4; ++i) {
        const bool sel = (i == equipCursor_);
        sf::RectangleShape bg({96.f, 96.f});
        bg.setPosition(pos[i]);
        bg.setFillColor(sel ? sf::Color(70, 70, 90, 240)
                            : sf::Color(40, 40, 50, 220));
        bg.setOutlineColor(sel ? sf::Color(255, 220, 100)
                               : sf::Color(90, 90, 100));
        bg.setOutlineThickness(sel ? 2.f : 1.f);
        t.draw(bg);

        const auto s = static_cast<core::EquipSlot>(i + 1);
        sf::Text label;
        label.setFont(font);
        label.setString(core::equipSlotName(s));
        label.setCharacterSize(12);
        label.setFillColor(sf::Color(160, 160, 160));
        label.setPosition(pos[i].x, pos[i].y - 18.f);
        t.draw(label);

        if (!equipment_) continue;
        const core::Item& item = equipment_->get(s);
        if (item.isEmpty()) continue;
        if (const core::ItemDef* def = item.def()) {
            if (const sf::Texture* tex = itemIconFor(def)) {
                sf::Sprite spr(*tex);
                const float scale = (96.f - 16.f) / def->spriteW;
                spr.setScale(scale, scale);
                spr.setPosition(pos[i].x + 8.f, pos[i].y + 8.f);
                t.draw(spr);
            }
        }
    }
}

void InventoryUI::renderDetailPanel(sf::RenderTarget& t, float sw, float sh,
                                    const sf::Font& font) const {
    const sf::Vector2f dp = detailOrigin(sw, sh);
    const float ph = std::max(220.f, sh - dp.y - 60.f);

    sf::RectangleShape bg({kDetailW, ph});
    bg.setPosition(dp);
    bg.setFillColor(sf::Color(15, 15, 25, 230));
    bg.setOutlineColor(sf::Color(90, 90, 110));
    bg.setOutlineThickness(1.f);
    t.draw(bg);

    // Item selecionado (grid ou slot de equipamento).
    const core::Item* sel = nullptr;
    if (mainTab_ == MainTab::Inventory) {
        if (inv_ && cursor_ >= 0 && cursor_ < kSlots) {
            const core::Item& i = inv_->slot(cursor_);
            if (!i.isEmpty()) sel = &i;
        }
    } else if (equipment_) {
        const auto s = static_cast<core::EquipSlot>(equipCursor_ + 1);
        const core::Item& i = equipment_->get(s);
        if (!i.isEmpty()) sel = &i;
    }
    if (!sel) return;
    const core::ItemDef* def = sel->def();
    if (!def) return;

    const float x = dp.x + 14.f;
    const float maxW = kDetailW - 28.f;
    float y = dp.y + 12.f;

    auto line = [&](const std::string& s, int size, sf::Color c) {
        sf::Text tt;
        tt.setFont(font);
        tt.setString(s);
        tt.setCharacterSize(static_cast<unsigned>(size));
        tt.setFillColor(c);
        tt.setPosition(x, y);
        t.draw(tt);
        y += tt.getLocalBounds().height + static_cast<float>(size) * 0.9f;
    };

    sf::Text name;
    name.setFont(font);
    name.setString(def->name);
    name.setCharacterSize(18);
    name.setFillColor(sf::Color(230, 230, 230));
    name.setPosition(x, y);
    t.draw(name);
    y += 30.f;

    line(itemRarityName(def->rarity), 12, itemRarityColor(def->rarity));

    sf::RectangleShape sep({maxW, 1.f});
    sep.setPosition(x, y);
    sep.setFillColor(sf::Color(60, 60, 70));
    t.draw(sep);
    y += 10.f;

    // Descrição com wrap por palavra.
    std::istringstream iss(def->description);
    std::string word, cur;
    auto flush = [&](const std::string& s) {
        if (s.empty()) return;
        sf::Text tt;
        tt.setFont(font);
        tt.setString(s);
        tt.setCharacterSize(12);
        tt.setFillColor(sf::Color(200, 200, 200));
        tt.setPosition(x, y);
        t.draw(tt);
        y += 17.f;
    };
    while (iss >> word) {
        const std::string test = cur.empty() ? word : cur + " " + word;
        sf::Text tmp;
        tmp.setFont(font);
        tmp.setString(test);
        tmp.setCharacterSize(12);
        if (tmp.getLocalBounds().width > maxW) {
            flush(cur);
            cur = word;
        } else {
            cur = test;
        }
    }
    flush(cur);
    y += 6.f;

    // Stats (só quando aplicável) + material.
    if (def->damage > 0)
        line("DMG: " + std::to_string(def->damage), 14,
             sf::Color(255, 200, 100));
    if (def->defense > 0)
        line("DEF: " + std::to_string(def->defense), 14,
             sf::Color(120, 180, 255));
    if (def->type == core::ItemType::Weapon ||
        def->type == core::ItemType::Armor)
        line(std::string("MAT: ") + core::materialName(def->material), 12,
             sf::Color(170, 170, 180));
    if (sel->quantity > 1)
        line("x" + std::to_string(sel->quantity), 12,
             sf::Color(220, 220, 220));
    y += 4.f;

    // Ações contextuais (dica; execução via [F]).
    using T = core::ItemType;
    if (mainTab_ == MainTab::Equipment) {
        line("[F] Unequip", 12, sf::Color(240, 220, 160));
    } else if (def->type == T::Key || def->type == T::Quest) {
        line("guardado (nao dropavel)", 12, sf::Color(150, 150, 150));
    } else if (def->type == T::Consumable) {
        line("[F] Use / Drop", 12, sf::Color(240, 220, 160));
    } else if (def->type == T::Material) {
        line("[F] Drop", 12, sf::Color(240, 220, 160));
    } else if (equipment_) {
        line("[F] Equip / Drop", 12, sf::Color(240, 220, 160));
    } else {
        line("[F] Drop", 12, sf::Color(240, 220, 160));
    }
}

void InventoryUI::renderActionMenu(sf::RenderTarget& t, float sw, float sh,
                                   const sf::Font& font) const {
    const auto actions = menuActions();
    if (actions.empty()) return;
    const int n = static_cast<int>(actions.size());
    const float mw = 200.f;
    const float mh = n * 30.f + 16.f;
    const float mx = sw * 0.5f - mw * 0.5f;
    const float my = sh * 0.5f - mh * 0.5f;

    sf::RectangleShape bg({mw, mh});
    bg.setPosition(mx, my);
    bg.setFillColor(sf::Color(20, 20, 30, 250));
    bg.setOutlineColor(sf::Color(200, 160, 80));
    bg.setOutlineThickness(2.f);
    t.draw(bg);

    for (int i = 0; i < n; ++i) {
        const bool sel = (i == actionCursor_);
        sf::Text tt;
        tt.setFont(font);
        tt.setString(menuActionName(actions[i]));
        tt.setCharacterSize(15);
        tt.setFillColor(sel ? sf::Color(255, 220, 100)
                            : sf::Color(200, 200, 200));
        tt.setPosition(mx + 20.f, my + 10.f + i * 30.f);
        t.draw(tt);
    }
}

void InventoryUI::renderConfirmDrop(sf::RenderTarget& t, float sw, float sh,
                                    const sf::Font& font) const {
    const float mw = 300.f, mh = 70.f;
    const float mx = sw * 0.5f - mw * 0.5f;
    const float my = sh * 0.5f - mh * 0.5f;
    sf::RectangleShape bg({mw, mh});
    bg.setPosition(mx, my);
    bg.setFillColor(sf::Color(20, 20, 30, 250));
    bg.setOutlineColor(sf::Color(200, 160, 80));
    bg.setOutlineThickness(2.f);
    t.draw(bg);
    sf::Text tt;
    tt.setFont(font);
    tt.setString("Soltar tudo?  [F] Sim  [Esc] Nao");
    tt.setCharacterSize(15);
    tt.setFillColor(sf::Color::White);
    tt.setPosition(mx + 20.f, my + 22.f);
    t.draw(tt);
}

void InventoryUI::renderFooter(sf::RenderTarget& t, float sw, float sh,
                               const sf::Font& font) const {
    sf::Text gold, hints;
    gold.setFont(font);
    hints.setFont(font);
    gold.setString("Ouro: " + std::to_string(inv_ ? inv_->gold() : 0));
    gold.setCharacterSize(14);
    gold.setFillColor(sf::Color(240, 220, 140));
    gold.setPosition(24.f, sh - 34.f);
    t.draw(gold);
    hints.setString("[Q][Tab] Tab  [A][D] Sub  [F] Acao  [E] Fechar");
    hints.setCharacterSize(12);
    hints.setFillColor(sf::Color(150, 150, 150));
    const float w = hints.getLocalBounds().width;
    hints.setPosition(sw - w - 24.f, sh - 32.f);
    t.draw(hints);
}

} // namespace support
