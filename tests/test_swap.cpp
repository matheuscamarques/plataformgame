/**
 * @file tests/test_swap.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless da troca rápida DS (Z/X/C/V).
 * @details Cobre cycleHand (dir/esq, gates, mão vazia), cycleSpell (roda attuned) e canQuickSwap, roda com make test que compila em build/tests/test_swap.
 */

#include <cassert>
#include <cstdio>
#include <string>

#include "entities/Player/Player.h"

int main() {
    { // CycleRightHand (espada -> machado e volta)
        Player p; // seed: iron_sword na direita, iron_axe no kit
        assert(p.equipment.get(core::EquipSlot::RightHand).defId ==
               "iron_sword");
        std::string name;
        assert(p.cycleHand(core::EquipSlot::RightHand, &name));
        assert(p.equipment.get(core::EquipSlot::RightHand).defId ==
               "iron_axe");
        assert(name == "Machado de Ferro");
        assert(p.inventory.count("iron_sword") >= 1); // devolvida
        assert(p.cycleHand(core::EquipSlot::RightHand, &name));
        assert(p.equipment.get(core::EquipSlot::RightHand).defId ==
               "iron_sword");
    }
    { // SingleWeaponNoSwap (1 arma total = false)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand);
        p.equipment.unequip(core::EquipSlot::LeftHand);
        // Tira todas as armas do kit menos 1
        for (int i = 0; i < core::Inventory::kCapacity; ++i) {
            core::Item &it = p.inventory.slot(i);
            if (it.isEmpty()) continue;
            const core::ItemDef *d = it.def();
            if (d && d->type == core::ItemType::Weapon &&
                it.defId != "iron_sword")
                it = core::Item{};
        }
        assert(!p.cycleHand(core::EquipSlot::RightHand));
    }
    { // GatedWhenActiveOrDead (golpe Active e morte barram)
        Player p;
        assert(p.startSwing());
        while (p.meleePhase == MeleePhase::Windup)
            p.updateMelee(1.f / 30.f);
        assert(p.meleePhase == MeleePhase::Active);
        assert(!p.canQuickSwap());
        assert(!p.cycleHand(core::EquipSlot::RightHand));
        Player d;
        d.hp = 0;
        assert(!d.canQuickSwap());
        assert(!d.cycleSpell());
    }
    { // CycleSpellRotates (0 vai p/ o fim)
        Player p;
        int souls = 1000000000;
        for (int i = 0; i < 4; ++i)
            assert(p.attrs.buy(core::Attr::Intelligence, souls));
        for (int i = 0; i < 4; ++i)
            assert(p.attrs.buy(core::Attr::Faith, souls));
        for (int i = 0; i < 2; ++i)
            assert(p.attrs.buy(core::Attr::Attunement, souls));
        for (int i = 0; i < 6; ++i)
            assert(p.attrs.buy(core::Attr::Attunement, souls)); // ATT 18: 2 slots
        p.refreshDerived();
        assert(p.attune("soul_arrow"));
        assert(p.attune("heal_light"));
        std::string name;
        assert(p.cycleSpell(&name));
        assert(p.attuned[0] == "heal_light");
        assert(name == "Luz Curativa");
        assert(p.cycleSpell());
        assert(p.attuned[0] == "soul_arrow");
        Player solo;
        assert(!solo.cycleSpell()); // <2 = false
    }

    std::printf("swap test OK\n");
    return 0;
}
