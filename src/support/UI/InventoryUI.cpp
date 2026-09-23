/**
 * @file src/support/UI/InventoryUI.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa navegação, abas, menu e render do inventário.
 * @details Implementa nomes de abas, filtros, cursor, compareStats e render de grid 8x5, equipamento e menus, manipulada por Game tick e render com Player e DropSystem.
 */

#include "support/UI/InventoryUI.h"

#include <algorithm>
#include <array>
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
    feedback_.clear();
    snapCursor();
}

void InventoryUI::close() { state_ = UIState::Closed; }

void InventoryUI::toggle() {
    if (state_ == UIState::Closed) open();
    else close();
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
    if (!slotMatches(cursor_)) return out; // F só em item visível
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

const core::Item* InventoryUI::selectedItem() const {
    if (!inv_) return nullptr;
    if (mainTab_ == MainTab::Equipment) {
        if (!equipment_) return nullptr;
        const auto s = static_cast<core::EquipSlot>(equipCursor_ + 1);
        const core::Item& i = equipment_->get(s);
        return i.isEmpty() ? nullptr : &i;
    }
    if (cursor_ < 0 || cursor_ >= kSlots) return nullptr;
    const core::Item& i = inv_->slot(cursor_);
    return i.isEmpty() ? nullptr : &i;
}

InventoryUI::StatCompare InventoryUI::compareStats() const {
    StatCompare out;
    if (mainTab_ != MainTab::Inventory || !equipment_) return out;
    const core::Item* sel = selectedItem();
    if (!sel) return out;
    const core::ItemDef* def = sel->def();
    if (!def) return out;
    const bool isWeapon = (def->type == core::ItemType::Weapon);
    const bool isArmor = (def->type == core::ItemType::Armor);
    if (!isWeapon && !isArmor) return out;
    if (def->equipSlot == core::EquipSlot::None) return out;
    const core::Item& cur = equipment_->get(def->equipSlot);
    if (cur.isEmpty() || cur.defId == sel->defId) return out;
    const core::ItemDef* curDef = cur.def();
    if (!curDef) return out;
    out.show = true;
    out.equipped = isWeapon ? curDef->damage : curDef->defense;
    out.diff = (isWeapon ? def->damage : def->defense) - out.equipped;
    return out;
}

std::string InventoryUI::confirmText() const {
    const core::Item* sel = selectedItem();
    const core::ItemDef* def = sel ? sel->def() : nullptr;
    if (!sel || !def)
        return "Soltar?  [F] Sim  [Esc] Nao";
    return "Soltar " + std::to_string(sel->quantity) + "x " + def->name +
           "?  [F] Sim  [Esc] Nao";
}

bool InventoryUI::executeAction(MenuAction action) {
    if (!inv_) return false;

    if (action == MenuAction::Unequip) {
        if (mainTab_ != MainTab::Equipment || !equipment_) return false;
        const auto s = static_cast<core::EquipSlot>(equipCursor_ + 1);
        core::Item removed = equipment_->unequip(s);
        if (removed.isEmpty()) return false;
        const core::ItemDef* def = removed.def();
        const int leftover = inv_->add(removed);
        if (leftover > 0) {
            removed.quantity = static_cast<uint16_t>(leftover);
            equipment_->equip(removed); // grid cheio: devolve
            feedback_ = "Inventário cheio!";
            return false;
        }
        feedback_ = std::string("Desequipado: ") + (def ? def->name : "?");
        return true;
    }

    if (mainTab_ != MainTab::Inventory) return false;
    if (action == MenuAction::Arrange) {
        inv_->sort(); // global: não precisa de cursor válido
        feedback_ = "Organizado por tipo";
        return true;
    }
    if (!slotMatches(cursor_)) return false;
    core::Item& item = inv_->slot(cursor_);
    if (item.isEmpty()) return false;
    const core::ItemDef* def = item.def();
    if (!def) return false;

    switch (action) {
        case MenuAction::Use:
            if (def->onUse && player_) {
                def->onUse(*player_);
                inv_->remove(def->id, 1);
                feedback_ = std::string("Usou: ") + def->name;
                return true;
            }
            return false;
        case MenuAction::Equip: {
            if (!equipment_) return false;
            if (def->equipSlot == core::EquipSlot::None) return false;
            core::Item old;
            if (equipment_->equip(item, &old)) {
                item = old; // cursor recebe o antigo (ou esvazia)
                feedback_ = std::string("Equipado: ") + def->name;
                if (!old.isEmpty()) {
                    const core::ItemDef* oldDef = old.def();
                    feedback_ += std::string(" (trocou com ") +
                                 (oldDef ? oldDef->name : "?") + ")";
                }
                return true;
            }
            return false;
        }
        case MenuAction::Drop: {
            using T = core::ItemType;
            if (!drops_ || !player_) return false;
            if (def->type == T::Key || def->type == T::Quest) return false;
            drops_->spawnItem(def->id, item.quantity,
                              {player_->getCenterX(), player_->getCenterY()});
            feedback_ = "Soltou " + std::to_string(item.quantity) + "x " +
                        def->name;
            item = core::Item{};
            return true;
        }
        default:
            return false;
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
    if (item.isEmpty()) return false; // vazio não navega (padrão DS)
    return matchesSubTab(item.def(), subTab_);
}

int InventoryUI::firstValid() const {
    for (int i = 0; i < kSlots; ++i)
        if (slotMatches(i)) return i;
    return -1;
}

int InventoryUI::lastValid() const {
    for (int i = kSlots - 1; i >= 0; --i)
        if (slotMatches(i)) return i;
    return -1;
}

void InventoryUI::snapCursor() {
    if (slotMatches(cursor_)) return;
    const int v = firstValid();
    if (v >= 0) cursor_ = v; // sem válido: fica (grade vazia/filtrada)
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
    snapCursor();
}

void InventoryUI::cycleSubTab(int delta) {
    constexpr int n = static_cast<int>(SubTab::COUNT);
    subTab_ = static_cast<SubTab>(
        (static_cast<int>(subTab_) + delta + n) % n);
    cursor_ = 0;
    snapCursor();
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
            equipCursor_ = (equipCursor_ + 4) % 5;
        if (input.pressed(Action::Down) || input.pressed(Action::Right))
            equipCursor_ = (equipCursor_ + 1) % 5;
        if (input.pressed(Action::Interact)) openActionMenu();
        return;
    }

    // Grid: atalhos + Home/End + setas + F.
    if (input.pressed(Action::FirstSlot)) {
        cursor_ = 0;
        snapCursor();
        return;
    }
    if (input.pressed(Action::LastSlot)) {
        const int v = lastValid();
        cursor_ = (v >= 0) ? v : kSlots - 1;
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
    // Bloco grid + vão + painel centralizado, com respiro mínimo de 24px
    // (em 800px o centrado daria 19px — apertado).
    const float totalW = gw + 16.f + kDetailW;
    (void)sh;
    return {std::max((sw - totalW) * 0.5f, 24.f), 110.f};
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

    // Visibilidade 1× por frame (DS: fora da aba some, não esmaece).
    std::array<bool, kSlots> vis{};
    bool any = false;
    for (int i = 0; i < kSlots; ++i) {
        vis[i] = slotMatches(i);
        any = any || vis[i];
    }
    if (!any) {
        sf::Text msg;
        msg.setFont(font);
        msg.setString(inv_->usedSlots() == 0 ? "Inventário vazio"
                                             : "Nenhum item nesta categoria");
        msg.setCharacterSize(14);
        msg.setFillColor(sf::Color(150, 150, 150));
        const sf::Vector2f o = gridOrigin(sw, sh);
        msg.setPosition(o.x + 40.f, o.y + 120.f);
        t.draw(msg);
    }

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
        t.draw(bg);

        if (!vis[i]) continue; // fora da aba: some (grade esparsa)
        const core::Item& item = inv_->slot(i);
        const core::ItemDef* def = item.def();
        if (!def) continue;

        sf::RectangleShape rb({kSlotSize - 4.f, kSlotSize - 4.f});
        rb.setPosition(p.x + 2.f, p.y + 2.f);
        rb.setFillColor(sf::Color::Transparent);
        rb.setOutlineColor(itemRarityColor(def->rarity));
        rb.setOutlineThickness(2.f);
        t.draw(rb);

        if (const sf::Texture* tex = itemIconFor(def)) {
            sf::Sprite spr(*tex);
            const float scale = (kSlotSize - 12.f) / def->spriteW;
            spr.setScale(scale, scale);
            spr.setPosition(p.x + 6.f, p.y + 6.f);
            t.draw(spr);
        }

        if (item.quantity > 1) {
            sf::Text q;
            q.setFont(font);
            q.setString(std::to_string(item.quantity));
            q.setCharacterSize(13);
            q.setFillColor(sf::Color::White);
            q.setOutlineColor(sf::Color::Black);
            q.setOutlineThickness(1.f);
            q.setPosition(p.x + kSlotSize - 22.f, p.y + kSlotSize - 18.f);
            t.draw(q);
        }
    }
}

void InventoryUI::renderEquipTab(sf::RenderTarget& t, float sw, float sh,
                                 const sf::Font& font) const {
    // 2 colunas (Arms | Armor), 3 linhas à direita. Tudo derivado de
    // kEquipSlotSize/kEquipPad: bloco 216px centralizado na área do grid.
    const float ss = kEquipSlotSize;
    const float step = ss + kEquipPad; // 120
    const sf::Vector2f o = gridOrigin(sw, sh);
    const float gw = kCols * kSlotSize + (kCols - 1) * kPad;
    const float cx = o.x + gw * 0.5f;
    const float cy = o.y + 150.f; // centro da linha do meio
    const float x0 = cx - (ss + kEquipPad + ss) * 0.5f;
    const float yMid = cy - ss * 0.5f; // topo da linha do meio
    const sf::Vector2f pos[5] = {
        {x0, yMid - step},          // RightHand (Arms, alinha Head)
        {x0 + step, yMid - 2 * step}, // Head
        {x0 + step, yMid - step},   // Chest
        {x0 + step, yMid},          // Legs
        {x0 + step, yMid + step},   // Boots
    };
    auto header = [&](const std::string& s, float x, float y) {
        sf::Text h;
        h.setFont(font);
        h.setString(s);
        h.setCharacterSize(13);
        h.setFillColor(sf::Color(200, 180, 120));
        h.setPosition(x, y);
        t.draw(h);
    };
    header("Arms", x0, yMid - step - 22.f);
    header("Armor", x0 + step, yMid - 2 * step - 22.f);
    for (int i = 0; i < 5; ++i) {
        const bool sel = (i == equipCursor_);
        sf::RectangleShape bg({ss, ss});
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
                const float scale = (ss - 16.f) / def->spriteW;
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
    const core::Item* sel = selectedItem();
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
        y += static_cast<float>(size) + 4.f; // passo fixo, sem drift
    };

    if (!sel) {
        line("Slot vazio", 14, sf::Color(150, 150, 150));
        return;
    }
    const core::ItemDef* def = sel->def();
    if (!def) {
        line("Slot vazio", 14, sf::Color(150, 150, 150));
        return;
    }

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

    // Descrição com wrap por palavra, teto de 8 linhas ("..." se cortar).
    static constexpr int kDescMaxLines = 8;
    std::istringstream iss(def->description);
    std::string word, cur;
    std::vector<std::string> wrapped;
    while (iss >> word) {
        const std::string test = cur.empty() ? word : cur + " " + word;
        sf::Text tmp;
        tmp.setFont(font);
        tmp.setString(test);
        tmp.setCharacterSize(12);
        if (tmp.getLocalBounds().width > maxW) {
            if (!cur.empty()) wrapped.push_back(cur);
            cur = word;
        } else {
            cur = test;
        }
    }
    if (!cur.empty()) wrapped.push_back(cur);
    for (std::size_t li = 0;
         li < wrapped.size() && li < static_cast<std::size_t>(kDescMaxLines);
         ++li) {
        sf::Text tt;
        tt.setFont(font);
        tt.setString(wrapped[li]);
        tt.setCharacterSize(12);
        tt.setFillColor(sf::Color(200, 200, 200));
        tt.setPosition(x, y);
        t.draw(tt);
        y += 12.f + 4.f;
    }
    if (wrapped.size() > static_cast<std::size_t>(kDescMaxLines)) {
        sf::Text more;
        more.setFont(font);
        more.setString("...");
        more.setCharacterSize(12);
        more.setFillColor(sf::Color(150, 150, 150));
        more.setPosition(x, y);
        t.draw(more);
        y += 12.f + 4.f;
    }
    y += 6.f;

    // Stats (só quando aplicável) + material.
    if (def->damage > 0)
        line("DMG: " + std::to_string(def->damage), 14,
             sf::Color(255, 200, 100));
    if (def->defense > 0)
        line("DEF: " + std::to_string(def->defense), 14,
             sf::Color(120, 180, 255));
    // Comparação com o equipado ("Atual: 12 (+6)").
    const StatCompare cmp = compareStats();
    if (cmp.show) {
        const std::string sign = cmp.diff > 0 ? "+" : "";
        const sf::Color c = cmp.diff > 0   ? sf::Color(120, 220, 120)
                            : cmp.diff < 0 ? sf::Color(240, 120, 120)
                                           : sf::Color(170, 170, 180);
        line("Atual: " + std::to_string(cmp.equipped) + " (" + sign +
                 std::to_string(cmp.diff) + ")",
             13, c);
    }
    // Marcador de equipado (mesmo defId no slot natural).
    if (equipment_ && def->equipSlot != core::EquipSlot::None &&
        equipment_->get(def->equipSlot).defId == sel->defId)
        line("(equipado)", 12, sf::Color(240, 220, 160));
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
    tt.setString(confirmText());
    tt.setCharacterSize(15);
    tt.setFillColor(sf::Color::White);
    const float tw = tt.getLocalBounds().width;
    tt.setPosition(mx + (mw - tw) * 0.5f, my + 22.f);
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
    // Feedback da última ação (some na próxima ação/abertura).
    if (!feedback_.empty()) {
        sf::Text fb;
        fb.setFont(font);
        fb.setString(feedback_);
        fb.setCharacterSize(13);
        fb.setFillColor(sf::Color(240, 220, 160));
        const float fw = fb.getLocalBounds().width;
        fb.setPosition((sw - fw) * 0.5f, sh - 34.f);
        t.draw(fb);
    }
    hints.setString("[Q][Tab] Tab  [A][D] Sub  [F] Acao  [E] Fechar");
    hints.setCharacterSize(12);
    hints.setFillColor(sf::Color(150, 150, 150));
    const float w = hints.getLocalBounds().width;
    hints.setPosition(sw - w - 24.f, sh - 32.f);
    t.draw(hints);
}

} // namespace support
