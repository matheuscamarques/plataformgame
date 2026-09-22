#include <cassert>
#include <cstdio>

#include "core/Inventory.h"
#include "core/ItemDef.h"
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

    std::printf("inventory_ui test OK\n");
    return 0;
}
