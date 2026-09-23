/**
 * @file tests/test_inventory.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava inventário com stacking e registry de itens.
 * @details Cobre Inventory e ItemDef, roda com make test que compila em build/tests/test_inventory.
 */

#include <cassert>
#include <cstdio>

#include "core/Inventory.h"
#include "core/ItemDef.h"

// Inventário slot-based com stacking (fase 1). Headless (sem GL).
int main() {
    using namespace core;

    { // RegistryTemDefs (seed via REGISTER_ITEM em ItemDefs.cpp)
        assert(ItemRegistry::instance().has("dynamite"));
        assert(ItemRegistry::instance().has("stone"));
        assert(ItemRegistry::instance().find("nope") == nullptr);
        assert(ItemRegistry::instance().find("potion")->stackMax == 10);
    }
    { // AddEmpilha (mesmo tipo completa a stack antes de abrir nova)
        Inventory inv;
        assert(inv.add(Item{"stone", 60}) == 0);
        assert(inv.add(Item{"stone", 60}) == 0);
        assert(inv.count("stone") == 120);
        assert(inv.usedSlots() == 2); // 99 + 21
        assert(inv.slot(0).quantity == 99);
        assert(inv.slot(1).quantity == 21);
    }
    { // AddOverflow (39 stacks cheias + 1 livre: sobra 151 de 250)
        Inventory inv;
        for (int i = 0; i < 39; ++i) assert(inv.add(Item{"stone", 99}) == 0);
        assert(inv.usedSlots() == 39);
        assert(inv.add(Item{"stone", 250}) == 151);
        assert(inv.count("stone") == 39 * 99 + 99);
        assert(inv.usedSlots() == 40);
    }
    { // AddRecusaDesconhecido (def fora do registry não entra)
        Inventory inv;
        assert(inv.add(Item{"unobtainium", 5}) == 5);
        assert(inv.usedSlots() == 0);
        assert(inv.add(Item{}) == 0);
    }
    { // StackMaxUm (chave não empilha: 1 por slot)
        Inventory inv;
        assert(inv.add(Item{"rusty_key", 1}) == 0);
        assert(inv.add(Item{"rusty_key", 1}) == 0);
        assert(inv.count("rusty_key") == 2);
        assert(inv.usedSlots() == 2);
    }
    { // RemoveAtomico (falha não drena parcial)
        Inventory inv;
        assert(inv.add(Item{"stone", 120}) == 0); // 99 + 21
        assert(!inv.remove("stone", 121));
        assert(inv.count("stone") == 120); // intacto
        assert(inv.remove("stone", 100));
        assert(inv.count("stone") == 20);
        assert(inv.usedSlots() == 1); // slot esvaziado foi limpo
        assert(inv.remove("stone", 20));
        assert(inv.usedSlots() == 0);
    }
    { // SwapTrocaSlots (base do drag-drop da fase 4)
        Inventory inv;
        assert(inv.add(Item{"stone", 10}) == 0);
        assert(inv.add(Item{"wood", 5}) == 0);
        inv.swap(0, 1);
        assert(inv.slot(0).defId == "wood");
        assert(inv.slot(1).defId == "stone");
        inv.swap(0, 0); // no-op seguro
        inv.swap(-1, 99); // fora do range: ignora
        assert(inv.slot(0).defId == "wood");
    }
    { // Ouro soma (sem slot)
        Inventory inv;
        assert(inv.gold() == 0);
        inv.addGold(50);
        inv.addGold(25);
        assert(inv.gold() == 75);
    }

    std::printf("inventory test OK\n");
    return 0;
}
