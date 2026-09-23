/**
 * @file tests/test_inventory_ui.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava lógica Dark Souls sem render no jogo.
 * @details Cobre InventoryUI e Equipment, roda com make test que compila em build/tests/test_inventory_ui.
 */

#include <cassert>
#include <cstdio>

#include "core/Inventory.h"
#include "core/Equipment.h"
#include "core/ItemDef.h"
#include "entities/Player/Player.h"
#include "support/Input/InputMap.h"
#include "support/Progression/DropSystem.h"
#include "support/UI/InventoryUI.h"

// Lógica da UI Dark Souls — headless. Render fica no jogo.
namespace {
sf::Event keyEvent(sf::Event::EventType t, sf::Keyboard::Key k) {
    sf::Event e{};
    e.type = t;
    e.key.code = k;
    return e;
}

void press(support::InputMap& in, sf::Keyboard::Key k) {
    in.beginFrame();
    in.handleEvent(keyEvent(sf::Event::KeyPressed, k));
}

void release(support::InputMap& in, sf::Keyboard::Key k) {
    in.handleEvent(keyEvent(sf::Event::KeyReleased, k));
    in.onTickEnd(); // frame teve tick: expira o latch no próximo frame
    in.beginFrame();
}

bool hasAction(const support::InventoryUI& ui,
               support::InventoryUI::MenuAction a) {
    for (auto x : ui.menuActions())
        if (x == a) return true;
    return false;
}
} // namespace

int main() {
    using namespace support;
    using MA = InventoryUI::MenuAction;

    { // OpenCloseState (fechado não consome; E/Esc fecham no Browse)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        ui.setInventory(&inv);
        assert(!ui.isOpen());
        in.beginFrame();
        assert(!ui.handleInput(in)); // fechado: nada
        ui.open();
        assert(ui.isOpen());
        assert(ui.state() == InventoryUI::UIState::Browse);
        in.beginFrame();
        assert(ui.handleInput(in)); // aberto, sem tecla: consome
        press(in, sf::Keyboard::E);
        assert(ui.handleInput(in)); // E fecha
        release(in, sf::Keyboard::E);
        assert(!ui.isOpen());
        ui.open();
        press(in, sf::Keyboard::Escape);
        assert(ui.handleInput(in)); // Esc no Browse fecha
        release(in, sf::Keyboard::Escape);
        assert(!ui.isOpen());
    }
    { // EscBacksOutOfMenu (no ActionMenu, Esc volta — não fecha)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"potion", 3});
        ui.setInventory(&inv);
        ui.open();
        press(in, sf::Keyboard::F);
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::ActionMenu);
        press(in, sf::Keyboard::Escape);
        assert(ui.handleInput(in));
        release(in, sf::Keyboard::Escape);
        assert(ui.state() == InventoryUI::UIState::Browse);
        assert(ui.isOpen());
    }
    { // MainTabSwitch (Tab/Q ciclam 4 abas, cursor reseta)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        ui.setInventory(&inv);
        ui.open();
        ui.setCursor(7);
        press(in, sf::Keyboard::Tab);
        ui.handleInput(in);
        release(in, sf::Keyboard::Tab);
        assert(ui.mainTab() == InventoryUI::MainTab::Equipment);
        assert(ui.cursor() == 0);
        press(in, sf::Keyboard::Q);
        ui.handleInput(in);
        release(in, sf::Keyboard::Q);
        assert(ui.mainTab() == InventoryUI::MainTab::Inventory);
        press(in, sf::Keyboard::Tab);
        ui.handleInput(in);
        release(in, sf::Keyboard::Tab);
        assert(ui.mainTab() == InventoryUI::MainTab::Equipment);
        press(in, sf::Keyboard::Tab);
        ui.handleInput(in);
        release(in, sf::Keyboard::Tab);
        assert(ui.mainTab() == InventoryUI::MainTab::Status);
        press(in, sf::Keyboard::Tab);
        ui.handleInput(in);
        release(in, sf::Keyboard::Tab);
        assert(ui.mainTab() == InventoryUI::MainTab::System);
        press(in, sf::Keyboard::Tab);
        ui.handleInput(in);
        release(in, sf::Keyboard::Tab);
        assert(ui.mainTab() == InventoryUI::MainTab::Inventory); // wrap
    }
    { // StatusTab (HP/arma/DEF/ouro do player; zeros sem player)
        InventoryUI ui;
        Player p; // seed ferro: espada 12, defesas 4+6+4+3
        ui.setPlayer(&p);
        ui.setEquipment(&p.equipment);
        ui.setInventory(&p.inventory);
        const auto st = ui.status();
        assert(st.hp == 10000 && st.hpMax == 10000);
        assert(st.weaponName == "Espada de Ferro" && st.damage == 12);
        assert(st.defense == 4 + 6 + 4 + 3);
        assert(st.gold == 0);
        InventoryUI bare;
        const auto z = bare.status();
        assert(z.hp == 0 && z.damage == 0 && z.defense == 0);
        assert(z.weaponName == "Soco");
    }
    { // SystemTab (volume A/D, Save em breve, Sair pede quit)
        InventoryUI ui;
        InputMap in;
        ui.open();
        ui.setMainTab(InventoryUI::MainTab::System);
        assert(ui.volumePct() == 70 && ui.sysCursor() == 0);
        press(in, sf::Keyboard::D);
        ui.handleInput(in);
        release(in, sf::Keyboard::D);
        assert(ui.volumePct() == 80);
        press(in, sf::Keyboard::A);
        ui.handleInput(in);
        release(in, sf::Keyboard::A);
        press(in, sf::Keyboard::A);
        ui.handleInput(in);
        release(in, sf::Keyboard::A);
        assert(ui.volumePct() == 60);
        press(in, sf::Keyboard::Down);
        ui.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(ui.sysCursor() == 1);
        press(in, sf::Keyboard::F);
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.feedback() == "Save em breve");
        assert(!ui.consumeQuitRequest());
        press(in, sf::Keyboard::Down);
        ui.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(ui.sysCursor() == 2);
        press(in, sf::Keyboard::F);
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.consumeQuitRequest()); // 1 disparo
        assert(!ui.consumeQuitRequest());
    }
    { // SubTabFilterNav (A/D filtram; setas pulam fora da aba)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"stone", 10});     // slot 0: Material
        inv.add(core::Item{"potion", 3});     // slot 1: Consumable
        inv.add(core::Item{"iron_sword", 1}); // slot 2: Weapon
        ui.setInventory(&inv);
        ui.open();
        press(in, sf::Keyboard::D);
        ui.handleInput(in);
        release(in, sf::Keyboard::D);
        assert(ui.subTab() == InventoryUI::SubTab::Materials);
        assert(ui.cursor() == 0); // troca reseta
        const auto slots = ui.filteredSlots();
        assert(slots.size() == 1u && slots[0] == 0);
        press(in, sf::Keyboard::Right);
        ui.handleInput(in);
        release(in, sf::Keyboard::Right);
        assert(ui.cursor() == 0); // único da aba: volta p/ si
        press(in, sf::Keyboard::A);
        ui.handleInput(in);
        release(in, sf::Keyboard::A);
        assert(ui.subTab() == InventoryUI::SubTab::All);
        assert(ui.filteredSlots().size() == 3u);
    }
    { // EmptyCategory (só pedra; aba Wpn filtra tudo)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"stone", 10});
        ui.setInventory(&inv);
        ui.open();
        ui.setSubTab(InventoryUI::SubTab::Weapons);
        assert(ui.filteredSlots().empty());
    }
    { // WrapAndHomeEnd (vazios não navegam; wrap entre válidos)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        for (int i = 0; i < 17; ++i)
            inv.add(core::Item{"rusty_key", 1}); // stackMax 1: slots 0..16
        ui.setInventory(&inv);
        ui.open();
        assert(ui.cursor() == 0);
        press(in, sf::Keyboard::Right);
        ui.handleInput(in);
        release(in, sf::Keyboard::Right);
        assert(ui.cursor() == 1);
        press(in, sf::Keyboard::Down);
        ui.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(ui.cursor() == 9); // 1+8, pula vazios
        press(in, sf::Keyboard::Left);
        ui.handleInput(in);
        release(in, sf::Keyboard::Left);
        assert(ui.cursor() == 8);
        press(in, sf::Keyboard::Up);
        ui.handleInput(in);
        release(in, sf::Keyboard::Up);
        assert(ui.cursor() == 0);
        press(in, sf::Keyboard::Left);
        ui.handleInput(in);
        release(in, sf::Keyboard::Left);
        assert(ui.cursor() == 16); // wrap reverso: último válido
        press(in, sf::Keyboard::End);
        ui.handleInput(in);
        release(in, sf::Keyboard::End);
        assert(ui.cursor() == 16); // Last = último válido
        press(in, sf::Keyboard::Home);
        ui.handleInput(in);
        release(in, sf::Keyboard::Home);
        assert(ui.cursor() == 0);
    }
    { // EmptyNotNavigable (grade vazia: cursor parado, sem crash)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        ui.setInventory(&inv);
        ui.open();
        press(in, sf::Keyboard::Right);
        ui.handleInput(in);
        release(in, sf::Keyboard::Right);
        assert(ui.cursor() == 0);
        assert(ui.filteredSlots().empty());
        assert(ui.selectedItem() == nullptr);
    }
    { // SnapToValid (abertura e trocas caem no 1º válido)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"wood", 5}); // slot 0
        inv.add(core::Item{"rusty_key", 1}); // slot 1 (stackMax 1)
        inv.add(core::Item{"rusty_key", 1}); // slot 2
        ui.setInventory(&inv);
        ui.open();
        assert(ui.cursor() == 0);
        ui.setCursor(30); // vazio: volta p/ o 1º válido
        assert(ui.cursor() == 0);
        ui.setSubTab(InventoryUI::SubTab::Weapons); // nada casa
        assert(ui.filteredSlots().empty());
        assert(ui.menuActions().empty()); // F sem efeito em inválido
    }
    { // MenuUse (F abre, F executa: onUse + consome 1)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        Player p;
        p.hp = 9000;
        inv.add(core::Item{"potion", 3});
        ui.setInventory(&inv);
        ui.setPlayer(&p);
        ui.open();
        assert(hasAction(ui, MA::Use));
        press(in, sf::Keyboard::F);
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::ActionMenu);
        press(in, sf::Keyboard::F);
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::Browse);
        assert(p.hp == 9030 && inv.count("potion") == 2);
    }
    { // MenuArrangeShortcut (T ordena por tipo+id)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"potion", 3});
        inv.add(core::Item{"stone", 10});
        ui.setInventory(&inv);
        ui.open();
        press(in, sf::Keyboard::T);
        ui.handleInput(in);
        release(in, sf::Keyboard::T);
        assert(inv.slot(0).defId == "stone");
        assert(inv.slot(1).defId == "potion");
    }
    { // MenuDropConfirm (F→Drop→F confirma; Esc cancela antes)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        Player p;
        DropSystem drops;
        inv.add(core::Item{"stone", 10});
        ui.setInventory(&inv);
        ui.setPlayer(&p);
        ui.setDrops(&drops);
        ui.open();
        assert(hasAction(ui, MA::Drop));
        press(in, sf::Keyboard::F); // abre menu
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        press(in, sf::Keyboard::Down); // Use -> Arrange? (lista: Drop, Arrange)
        ui.handleInput(in);
        release(in, sf::Keyboard::Down);
        press(in, sf::Keyboard::Up);
        ui.handleInput(in);
        release(in, sf::Keyboard::Up);
        press(in, sf::Keyboard::F); // escolhe Drop -> Confirm
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::ConfirmDrop);
        press(in, sf::Keyboard::Escape); // cancela: nada sai
        ui.handleInput(in);
        release(in, sf::Keyboard::Escape);
        assert(ui.state() == InventoryUI::UIState::Browse);
        assert(inv.count("stone") == 10);
        press(in, sf::Keyboard::F); // menu de novo
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        press(in, sf::Keyboard::F); // Drop (primeiro) -> Confirm
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        press(in, sf::Keyboard::F); // confirma
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::Browse);
        assert(inv.slot(0).isEmpty()); // pilha saiu p/ orbe
        assert(drops.activeItemCount() == 1u);
    }
    { // KeyHasNoDrop (chave: menu sem Drop; Exec Drop direto bloqueia)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        Player p;
        DropSystem drops;
        inv.add(core::Item{"rusty_key", 1});
        ui.setInventory(&inv);
        ui.setPlayer(&p);
        ui.setDrops(&drops);
        ui.open();
        assert(!hasAction(ui, MA::Drop));
        assert(hasAction(ui, MA::Arrange));
        assert(!ui.executeAction(MA::Drop)); // via código: bloqueado
        assert(inv.slot(0).defId == "rusty_key");
        assert(drops.activeItemCount() == 0u);
    }
    { // FeedbackAndBool (executeAction retorna; rodapé registra)
        InventoryUI ui;
        core::Inventory inv;
        core::Equipment eq;
        InputMap in;
        (void)in;
        inv.add(core::Item{"iron_sword", 1});
        ui.setInventory(&inv);
        ui.setEquipment(&eq);
        ui.open();
        assert(ui.executeAction(MA::Equip));
        assert(ui.feedback() == "Equipado: Espada de Ferro");
        assert(ui.executeAction(MA::Arrange));
        assert(ui.feedback() == "Organizado por tipo");
        assert(!ui.executeAction(MA::Drop)); // sem player/drops
        assert(!ui.executeAction(MA::Unequip)); // aba errada
    }
    { // CompareStats (machado vs espada: +6; igual/sem ref: esconde)
        InventoryUI ui;
        core::Inventory inv;
        core::Equipment eq;
        eq.equip(core::Item{"iron_sword", 1}); // DMG 12
        inv.add(core::Item{"iron_axe", 1});    // DMG 18, slot 0
        ui.setInventory(&inv);
        ui.setEquipment(&eq);
        ui.open();
        const auto cmp = ui.compareStats();
        assert(cmp.show && cmp.equipped == 12 && cmp.diff == 6);
        inv.add(core::Item{"iron_helm", 1}); // DEF 4, slot 1
        ui.setCursor(1);                     // Head vazio: sem referência
        assert(!ui.compareStats().show);
        eq.equip(core::Item{"iron_helm", 1});
        ui.setCursor(0);
        eq.equip(core::Item{"iron_axe", 1}); // mesmo id: sem comparação
        assert(!ui.compareStats().show);
    }
    { // ConfirmText (quantidade + nome; genérico sem item)
        InventoryUI ui;
        core::Inventory inv;
        inv.add(core::Item{"stone", 10});
        ui.setInventory(&inv);
        ui.open();
        assert(ui.confirmText() == "Soltar 10x Pedra?  [F] Sim  [Esc] Nao");
        assert(ui.selectedItem() != nullptr);
        assert(ui.selectedItem()->quantity == 10);
        InventoryUI empty;
        core::Inventory noInv;
        empty.setInventory(&noInv);
        empty.open();
        assert(empty.selectedItem() == nullptr);
        assert(empty.confirmText() == "Soltar?  [F] Sim  [Esc] Nao");
    }
    { // NoEquipmentNoEquipAction (sem equipment_: menu sem Equip)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"iron_sword", 1});
        ui.setInventory(&inv);
        ui.open();
        assert(!hasAction(ui, MA::Equip));
        assert(hasAction(ui, MA::Drop));
    }
    { // NullSafety (sem dependências: sem crash, menu vazio)
        InventoryUI ui;
        InputMap in;
        ui.open();
        in.beginFrame();
        assert(ui.handleInput(in));
        press(in, sf::Keyboard::F);
        assert(ui.handleInput(in));
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::Browse);
        assert(ui.filteredSlots().empty());
        assert(ui.menuActions().empty());
        ui.executeAction(MA::Arrange); // no-op
        ui.executeAction(MA::Drop);    // no-op
    }
    { // GridCobreInventario (constantes batem com o inventário)
        static_assert(InventoryUI::kSlots == core::Inventory::kCapacity,
                      "grid cobre o inventário inteiro");
        assert(InventoryUI::kCols == 8 && InventoryUI::kRows == 10);
    }
    { // OpenTransition (ease-out 0→1 em 0.15s; offset 14→0)
        InventoryUI ui;
        ui.open();
        const float t0 = ui.openTime();
        assert(ui.openT(t0) == 0.f);
        const float mid = ui.openT(t0 + 0.075f);
        assert(mid > 0.8f && mid < 0.95f); // ease-out: 0.875 em u=0.5
        assert(ui.openT(t0 + 0.15f) == 1.f);
        assert(ui.openT(t0 + 10.f) == 1.f);
        assert(ui.openOffset(t0) == 14.f);
        assert(ui.openOffset(t0 + 10.f) == 0.f);
        assert(ui.flashSlot() == -1); // sem equip ainda
    }
    { // WeaponDefs (dano/defesa/slots p/ o painel e o menu)
        const core::ItemDef* sword =
            core::ItemRegistry::instance().find("iron_sword");
        const core::ItemDef* helm =
            core::ItemRegistry::instance().find("iron_helm");
        assert(sword && sword->type == core::ItemType::Weapon);
        assert(sword->damage == 12 && sword->stackMax == 1);
        assert(sword->equipSlot == core::EquipSlot::RightHand);
        assert(helm && helm->type == core::ItemType::Armor);
        assert(helm->defense == 4 && helm->stackMax == 1);
        assert(helm->equipSlot == core::EquipSlot::Head);
        assert(InventoryUI::matchesSubTab(sword,
                                          InventoryUI::SubTab::Weapons));
        assert(InventoryUI::matchesSubTab(helm, InventoryUI::SubTab::Armor));
        assert(!InventoryUI::matchesSubTab(sword, InventoryUI::SubTab::Armor));
        const core::ItemDef* lhelm =
            core::ItemRegistry::instance().find("leather_helm");
        const core::ItemDef* lchest =
            core::ItemRegistry::instance().find("leather_chest");
        const core::ItemDef* llegs =
            core::ItemRegistry::instance().find("leather_legs");
        assert(lhelm && lhelm->equipSlot == core::EquipSlot::Head);
        assert(lhelm->material == core::MaterialId::Leather);
        assert(lhelm->defense == 2);
        assert(lchest && lchest->equipSlot == core::EquipSlot::Chest);
        assert(lchest->defense == 4);
        assert(llegs && llegs->equipSlot == core::EquipSlot::Legs);
        assert(InventoryUI::matchesSubTab(lhelm, InventoryUI::SubTab::Armor));
    }
    { // EquipMovesToSlot (inventário -> RightHand)
        core::Equipment eq;
        assert(eq.isEmpty());
        core::Item old;
        assert(eq.equip(core::Item{"iron_sword", 1}, &old));
        assert(old.isEmpty());
        assert(eq.get(core::EquipSlot::RightHand).defId == "iron_sword");
        assert(!eq.isEmpty() && eq.isOccupied(core::EquipSlot::RightHand));
        assert(!eq.isOccupied(core::EquipSlot::Head));
    }
    { // EquipSwapsOldBack (axe entra, sword volta no outOld)
        core::Equipment eq;
        assert(eq.equip(core::Item{"iron_sword", 1}));
        core::Item old;
        assert(eq.equip(core::Item{"iron_axe", 1}, &old));
        assert(eq.get(core::EquipSlot::RightHand).defId == "iron_axe");
        assert(old.defId == "iron_sword" && old.quantity == 1);
    }
    { // EquipSlotSeparation (elmo não mexe na arma)
        core::Equipment eq;
        assert(eq.equip(core::Item{"iron_sword", 1}));
        assert(eq.equip(core::Item{"iron_helm", 1}));
        assert(eq.equip(core::Item{"iron_chest", 1}));
        assert(eq.equip(core::Item{"iron_legs", 1}));
        assert(eq.get(core::EquipSlot::RightHand).defId == "iron_sword");
        assert(eq.get(core::EquipSlot::Head).defId == "iron_helm");
        assert(eq.get(core::EquipSlot::Chest).defId == "iron_chest");
        assert(eq.get(core::EquipSlot::Legs).defId == "iron_legs");
        assert(eq.equip(core::Item{"iron_boots", 1}));
        assert(eq.get(core::EquipSlot::Boots).defId == "iron_boots");
    }
    { // UnequipReturnsItem (slot esvazia, item volta)
        core::Equipment eq;
        assert(eq.equip(core::Item{"iron_sword", 1}));
        core::Item removed = eq.unequip(core::EquipSlot::RightHand);
        assert(removed.defId == "iron_sword");
        assert(eq.get(core::EquipSlot::RightHand).isEmpty());
        assert(eq.unequip(core::EquipSlot::RightHand).isEmpty()); // 2× ok
    }
    { // NonEquippableFails (pedra e def desconhecido não equipam)
        core::Equipment eq;
        assert(!eq.equip(core::Item{"stone", 5})); // Material: None
        assert(!eq.equip(core::Item{"nao_existe", 1})); // sem def
        assert(!eq.equip(core::Item{})); // vazio
        assert(eq.isEmpty());
    }
    { // EquipToSlot (alvo explícito; esquerda só arma)
        core::Equipment eq;
        assert(eq.equipTo(core::EquipSlot::LeftHand,
                          core::Item{"iron_sword", 1}));
        assert(eq.get(core::EquipSlot::LeftHand).defId == "iron_sword");
        assert(!eq.equipTo(core::EquipSlot::LeftHand,
                           core::Item{"iron_helm", 1})); // armadura não
        assert(!eq.equipTo(core::EquipSlot::Head,
                           core::Item{"iron_sword", 1})); // espada no elmo não
        assert(!eq.equipTo(core::EquipSlot::None,
                           core::Item{"iron_sword", 1}));
    }
    { // OffHandBonus (esquerda vazia no seed; espada soma +12)
        Player p;
        assert(p.equipment.get(core::EquipSlot::LeftHand).isEmpty());
        assert(p.offHandDef() == nullptr);
        const int base = p.meleeDamage();
        assert(p.equipment.equipTo(core::EquipSlot::LeftHand,
                                   core::Item{"iron_sword", 1}));
        assert(p.offHandDef() && p.offHandDef()->damage == 12);
        assert(p.meleeDamage() == base + 12);
    }
    { // EquipLoad (soma pesos; ferro 28, ouro 37, couro 15)
        core::Equipment iron;
        assert(iron.equip(core::Item{"iron_sword", 1}));
        assert(iron.equip(core::Item{"iron_helm", 1}));
        assert(iron.equip(core::Item{"iron_chest", 1}));
        assert(iron.equip(core::Item{"iron_legs", 1}));
        assert(iron.equip(core::Item{"iron_boots", 1}));
        assert(iron.weight() == 6 + 4 + 9 + 6 + 3);
        core::Equipment gold;
        assert(gold.equip(core::Item{"gold_sword", 1}));
        assert(gold.equip(core::Item{"gold_helm", 1}));
        assert(gold.equip(core::Item{"gold_chest", 1}));
        assert(gold.equip(core::Item{"gold_legs", 1}));
        assert(gold.equip(core::Item{"gold_boots", 1}));
        assert(gold.weight() == 8 + 5 + 12 + 8 + 4);
        core::Equipment bare;
        assert(bare.weight() == 0.f);
    }
    { // HeavilyLoaded (seed ferro corre; ouro não)
        Player seed; // set ferro: 28/60 = leve
        assert(seed.equipLoad() == 28.f);
        assert(!seed.heavilyLoaded());
        Player heavy;
        heavy.equipment = core::Equipment{};
        assert(heavy.equipment.equip(core::Item{"gold_sword", 1}));
        assert(heavy.equipment.equip(core::Item{"gold_helm", 1}));
        assert(heavy.equipment.equip(core::Item{"gold_chest", 1}));
        assert(heavy.equipment.equip(core::Item{"gold_legs", 1}));
        assert(heavy.equipment.equip(core::Item{"gold_boots", 1}));
        assert(heavy.equipLoad() == 37.f);
        assert(heavy.heavilyLoaded()); // >30: sem correr
    }
    { // SeedEquipped (player nasce com o set de ferro)
        Player p;
        assert(p.equipment.get(core::EquipSlot::RightHand).defId ==
               "iron_sword");
        assert(p.equipment.get(core::EquipSlot::Head).defId == "iron_helm");
        assert(p.equipment.get(core::EquipSlot::Chest).defId ==
               "iron_chest");
        assert(p.equipment.get(core::EquipSlot::Legs).defId == "iron_legs");
        assert(p.equipment.get(core::EquipSlot::Boots).defId == "iron_boots");
    }
    { // StarterKit (1 pilha cheia de cada item do registry)
        Player p;
        assert(p.inventory.count("dynamite") == 999);
        assert(p.inventory.count("slime_gel") == 99);
        assert(p.inventory.count("stone") == 99);
        assert(p.inventory.count("wood") == 99);
        assert(p.inventory.count("iron_ore") == 99);
        assert(p.inventory.count("copper_ore") == 99);
        assert(p.inventory.count("gold_ore") == 99);
        assert(p.inventory.count("potion") == 10);
        assert(p.inventory.count("rusty_key") == 1);
        assert(p.inventory.count("iron_sword") == 1);
        assert(p.inventory.count("iron_axe") == 1);
        assert(p.inventory.count("iron_helm") == 1);
        assert(p.inventory.count("iron_chest") == 1);
        assert(p.inventory.count("iron_legs") == 1);
        assert(p.inventory.count("leather_helm") == 1);
        assert(p.inventory.count("leather_chest") == 1);
        assert(p.inventory.count("leather_legs") == 1);
        assert(p.inventory.count("leather_sword") == 1);
        assert(p.inventory.count("leather_axe") == 1);
        assert(p.inventory.count("gold_sword") == 1);
        assert(p.inventory.count("gold_axe") == 1);
        assert(p.inventory.count("gold_helm") == 1);
        assert(p.inventory.count("gold_chest") == 1);
        assert(p.inventory.count("gold_legs") == 1);
        assert(p.inventory.count("diamond_sword") == 1);
        assert(p.inventory.count("diamond_axe") == 1);
        assert(p.inventory.count("diamond_helm") == 1);
        assert(p.inventory.count("diamond_chest") == 1);
        assert(p.inventory.count("diamond_legs") == 1);
        assert(p.inventory.count("iron_boots") == 1);
        assert(p.inventory.count("leather_boots") == 1);
        assert(p.inventory.count("gold_boots") == 1);
        assert(p.inventory.count("diamond_boots") == 1);
        assert(p.inventory.count("tnt") == 999);
        assert(p.inventory.count("c4") == 999);
        assert(p.inventory.count("daisy") == 999);
        assert(p.inventory.count("moab") == 999);
        // Kit inteiro: 36 defs em 1 slot cada + dinamite 999 em 1 só.
        assert(p.inventory.usedSlots() == 37);
    }
    { // EquipViaMenu (F→Equip: direita livre, esq, depois troca)
        InventoryUI ui;
        core::Inventory inv;
        core::Equipment eq;
        InputMap in;
        eq.equip(core::Item{"iron_axe", 1}); // ocupando a direita
        inv.add(core::Item{"iron_sword", 1}); // slot 0
        inv.add(core::Item{"iron_sword", 1}); // slot 1 (stackMax 1)
        ui.setInventory(&inv);
        ui.setEquipment(&eq);
        ui.open();
        assert(hasAction(ui, MA::Equip));
        press(in, sf::Keyboard::F);
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::ActionMenu);
        press(in, sf::Keyboard::F); // Equip: direita ocupada → esquerda
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(eq.get(core::EquipSlot::RightHand).defId == "iron_axe");
        assert(eq.get(core::EquipSlot::LeftHand).defId == "iron_sword");
        assert(inv.slot(0).isEmpty() && !inv.slot(1).isEmpty());
        assert(ui.flashTab() == InventoryUI::MainTab::Inventory);
        assert(ui.flashSlot() == 0); // marca o slot de origem
    }
    { // UnequipViaMenu (aba Equipment: F→Unequip volta p/ grid)
        InventoryUI ui;
        core::Inventory inv;
        core::Equipment eq;
        InputMap in;
        eq.equip(core::Item{"iron_sword", 1});
        ui.setInventory(&inv);
        ui.setEquipment(&eq);
        ui.open();
        press(in, sf::Keyboard::Tab); // aba Equipment
        ui.handleInput(in);
        release(in, sf::Keyboard::Tab);
        assert(ui.mainTab() == InventoryUI::MainTab::Equipment);
        assert(hasAction(ui, MA::Unequip));
        press(in, sf::Keyboard::F);
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(ui.state() == InventoryUI::UIState::ActionMenu);
        press(in, sf::Keyboard::F); // única opção: Unequip
        ui.handleInput(in);
        release(in, sf::Keyboard::F);
        assert(eq.get(core::EquipSlot::RightHand).isEmpty());
        assert(inv.count("iron_sword") == 1);
        assert(ui.flashTab() == InventoryUI::MainTab::Equipment);
        assert(ui.flashSlot() == 0);
    }
    { // UnequipFullCancel (grid cheio: fica equipado)
        InventoryUI ui;
        core::Inventory inv;
        core::Equipment eq;
        eq.equip(core::Item{"iron_sword", 1});
        inv.add(core::Item{"stone", 80 * 99}); // 80 slots cheios
        assert(inv.usedSlots() == 80);
        ui.setInventory(&inv);
        ui.setEquipment(&eq);
        ui.open();
        ui.setMainTab(InventoryUI::MainTab::Equipment);
        ui.executeAction(MA::Unequip);
        assert(eq.get(core::EquipSlot::RightHand).defId == "iron_sword");
        assert(inv.count("stone") == 80 * 99);
    }
    { // EquipTabSixSlots (Down circula 0..5, Boots=5)
        InventoryUI ui;
        core::Inventory inv;
        core::Equipment eq;
        InputMap in;
        ui.setInventory(&inv);
        ui.setEquipment(&eq);
        ui.open();
        ui.setMainTab(InventoryUI::MainTab::Equipment);
        for (int i = 0; i < 5; ++i) {
            press(in, sf::Keyboard::Down);
            ui.handleInput(in);
            release(in, sf::Keyboard::Down);
        }
        assert(ui.equipCursor() == 5);
        press(in, sf::Keyboard::Down);
        ui.handleInput(in);
        release(in, sf::Keyboard::Down);
        assert(ui.equipCursor() == 0); // wrap
    }

    std::printf("inventory_ui test OK\n");
    return 0;
}
