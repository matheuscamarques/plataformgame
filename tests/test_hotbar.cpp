/**
 * @file tests/test_hotbar.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava hotbar com mapeamento e input sem textura.
 * @details Cobre Inventory e HotbarUI, roda com make test que compila em build/tests/test_hotbar.
 */

#include <cassert>
#include <cstdio>

#include "core/Inventory.h"
#include "core/ItemDef.h"
#include "support/Input/InputMap.h"
#include "support/UI/HotbarUI.h"

// Hotbar 4a: lógica testável headless (mapeamento + input).
// Render (texturas) só no jogo — padrão iconFor/makeSprite.
namespace {
sf::Event keyEvent(sf::Event::EventType t, sf::Keyboard::Key k) {
    sf::Event e{};
    e.type = t;
    e.key.code = k;
    return e;
}
} // namespace

int main() {
    using namespace support;

    { // LePrimeirosSlots (hotbar espelha slots 0..4, sem cópia)
        core::Inventory inv;
        inv.add(core::Item{"stone", 10});
        inv.add(core::Item{"wood", 5});
        assert(inv.slot(0).defId == "stone");
        assert(inv.slot(1).defId == "wood");
        assert(inv.slot(2).isEmpty());
        assert(inv.slot(4).isEmpty());
        assert(HotbarUI::kSlots == 5);
    }
    { // TeclasSelecionamSlot (1..5 via evento sintético, padrão inputmap)
        HotbarUI ui;
        InputMap in;
        int active = 0;
        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Num3));
        active = ui.handleInput(in, active);
        assert(active == 2);
        in.beginFrame();
        in.handleEvent(keyEvent(sf::Event::KeyPressed, sf::Keyboard::Num1));
        active = ui.handleInput(in, active);
        assert(active == 0);
        // Sem tecla: mantém.
        in.beginFrame();
        assert(ui.handleInput(in, active) == 0);
    }
    { // SpritesRegistrados (todo def jogável tem ASCII 8×8 válido)
        for (const char* id : {"dynamite", "slime_gel", "stone", "wood",
                               "iron_ore", "copper_ore", "gold_ore", "potion",
                               "rusty_key"}) {
            const core::ItemDef* d =
                core::ItemRegistry::instance().find(id);
            assert(d && d->spriteRows && d->spriteW == 8 && d->spriteH == 8);
            assert(d->spritePal && d->spritePalCount >= 2);
            assert(!d->description.empty());
        }
    }

    std::printf("hotbar test OK\n");
    return 0;
}
