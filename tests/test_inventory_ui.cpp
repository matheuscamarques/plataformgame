#include <cassert>
#include <cstdio>

#include "core/Inventory.h"
#include "core/Equipment.h"
#include "core/ItemDef.h"
#include "entities/Player/Player.h"
#include "support/Input/InputMap.h"
#include "support/UI/InventoryUI.h"

// Lógica da UI grid (fase 4b) — headless. Render fica no jogo.
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
    in.beginFrame();
}
} // namespace

int main() {
    using namespace support;

    { // AbreFechaConsome (E alterna; aberto consome E/Esc, resto passa)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        assert(!ui.isOpen());
        in.beginFrame();
        assert(!ui.handleInput(in, inv)); // fechado: não consome nada
        press(in, sf::Keyboard::E);
        ui.toggle(); // App faria isso no edge (aqui direto, sem input)
        release(in, sf::Keyboard::E);
        assert(ui.isOpen());
        in.beginFrame();
        assert(!ui.handleInput(in, inv)); // aberto, sem tecla: não consome
        press(in, sf::Keyboard::E);
        assert(ui.handleInput(in, inv)); // E fecha + consome
        release(in, sf::Keyboard::E);
        assert(!ui.isOpen());
        ui.toggle();
        press(in, sf::Keyboard::Escape);
        assert(ui.handleInput(in, inv)); // Esc fecha + consome
        release(in, sf::Keyboard::Escape);
        assert(!ui.isOpen());
    }
    { // NavegacaoCircular (setas andam o cursor com wrap)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        ui.toggle();
        assert(ui.cursor() == 0);
        press(in, sf::Keyboard::Right);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Right);
        assert(ui.cursor() == 1);
        press(in, sf::Keyboard::Down);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Down);
        assert(ui.cursor() == 1 + 8);
        press(in, sf::Keyboard::Left);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Left);
        assert(ui.cursor() == 8);
        press(in, sf::Keyboard::Up);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Up);
        assert(ui.cursor() == 0);
        press(in, sf::Keyboard::Left);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Left);
        assert(ui.cursor() == 39); // wrap
    }
    { // PickPlaceSwap (F pega, F solta trocando com o cursor)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"stone", 10});
        inv.add(core::Item{"wood", 5});
        ui.toggle();
        // F no slot 0 (pedra): pega, buraco fica.
        press(in, sf::Keyboard::F);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::F);
        assert(inv.slot(0).isEmpty());
        // Anda ao slot 1 (madeira), F: troca — pedra entra, madeira na mão.
        press(in, sf::Keyboard::Right);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Right);
        press(in, sf::Keyboard::F);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::F);
        assert(inv.slot(1).defId == "stone");
        assert(inv.slot(1).quantity == 10);
        // F de novo no mesmo cursor: devolve a madeira, pega a pedra.
        press(in, sf::Keyboard::F);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::F);
        assert(inv.slot(1).defId == "wood");
        assert(inv.slot(1).quantity == 5);
        // Anda ao slot 2 (vazio), F: solta a pedra, mãos vazias.
        press(in, sf::Keyboard::Right);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Right);
        press(in, sf::Keyboard::F);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::F);
        assert(inv.slot(2).defId == "stone");
        assert(inv.slot(2).quantity == 10);
        // F no slot vazio e sem nada na mão: no-op.
        press(in, sf::Keyboard::Right);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Right);
        press(in, sf::Keyboard::F);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::F);
        assert(inv.slot(3).isEmpty());
        // Conservação: nada sumiu nem duplicou.
        assert(inv.count("stone") == 10);
        assert(inv.count("wood") == 5);
    }
    { // GridTem40 (constantes batem com o inventário)
        static_assert(InventoryUI::kSlots == core::Inventory::kCapacity,
                      "grid cobre o inventário inteiro");
        assert(InventoryUI::kCols == 8 && InventoryUI::kRows == 5);
    }
    { // TabsCycleFilter (Q/Tab ciclam; setas pulam fora da aba)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"stone", 10});      // slot 0: Material
        inv.add(core::Item{"potion", 3});      // slot 1: Consumable
        inv.add(core::Item{"iron_sword", 1});  // slot 2: Weapon
        ui.toggle();
        assert(ui.tab() == InventoryUI::TabAll && ui.cursor() == 0);
        press(in, sf::Keyboard::Tab);
        assert(ui.handleInput(in, inv)); // consome mesmo com efeito
        release(in, sf::Keyboard::Tab);
        assert(ui.tab() == InventoryUI::TabMaterial);
        assert(ui.cursor() == 0); // pedra casa: fica
        press(in, sf::Keyboard::Right);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Right);
        assert(ui.cursor() == 3); // pula poção+espada, cai no vazio
        press(in, sf::Keyboard::Q);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Q);
        assert(ui.tab() == InventoryUI::TabAll);
        press(in, sf::Keyboard::Q);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Q);
        assert(ui.tab() == InventoryUI::TabKey); // wrap p/ trás
        press(in, sf::Keyboard::Tab);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Tab);
        assert(ui.tab() == InventoryUI::TabAll); // wrap p/ frente
    }
    { // ArrangeCallsSort (T reordena por tipo+id; consome o edge)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"potion", 3}); // slot 0 primeiro (fora de ordem)
        inv.add(core::Item{"stone", 10}); // slot 1
        ui.toggle();
        press(in, sf::Keyboard::T);
        assert(ui.handleInput(in, inv));
        release(in, sf::Keyboard::T);
        assert(inv.slot(0).defId == "stone");
        assert(inv.slot(1).defId == "potion");
    }
    { // DropPendingAndKeyBlock (R gera pendente; chave não sai)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        inv.add(core::Item{"stone", 10});    // slot 0
        inv.add(core::Item{"rusty_key", 1}); // slot 1: Key, travada
        ui.toggle();
        press(in, sf::Keyboard::R);
        assert(ui.handleInput(in, inv)); // consome (App drena + anti-restart)
        release(in, sf::Keyboard::R);
        assert(inv.slot(0).isEmpty());
        assert(ui.hasPendingDrop());
        const auto pd = ui.takePendingDrop();
        assert(pd.defId == "stone" && pd.qty == 10);
        assert(!ui.hasPendingDrop());
        press(in, sf::Keyboard::Right);
        ui.handleInput(in, inv);
        release(in, sf::Keyboard::Right);
        assert(ui.cursor() == 1);
        press(in, sf::Keyboard::R);
        assert(ui.handleInput(in, inv)); // consome mesmo bloqueando
        release(in, sf::Keyboard::R);
        assert(inv.slot(1).defId == "rusty_key"); // Key fica
        assert(!ui.hasPendingDrop());
    }
    { // UsePotionHeals (U chama onUse e consome 1; sem player é no-op)
        InventoryUI ui;
        core::Inventory inv;
        InputMap in;
        Player p;
        p.hp = 9000;
        inv.add(core::Item{"potion", 3}); // slot 0, cursor 0
        ui.toggle();
        press(in, sf::Keyboard::U);
        assert(ui.handleInput(in, inv, &p));
        release(in, sf::Keyboard::U);
        assert(p.hp == 9030);
        assert(inv.count("potion") == 2);
        press(in, sf::Keyboard::U);
        assert(ui.handleInput(in, inv, nullptr)); // sem player: consome, nada faz
        release(in, sf::Keyboard::U);
        assert(p.hp == 9030 && inv.count("potion") == 2);
    }
    { // WeaponDefs (4d: dano/defesa populados p/ o painel)
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
        assert(InventoryUI::matchesTab(sword, InventoryUI::TabWeapon));
        assert(InventoryUI::matchesTab(helm, InventoryUI::TabArmor));
        assert(!InventoryUI::matchesTab(sword, InventoryUI::TabArmor));
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

    std::printf("inventory_ui test OK\n");
    return 0;
}
