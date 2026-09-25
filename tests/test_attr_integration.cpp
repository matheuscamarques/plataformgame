/**
 * @file tests/test_attr_integration.cpp
 * @brief Testes de integração buy → refreshDerived → derivado.
 */

#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"

int main() {
    using namespace core;

    { // test_buy_str_changes_damage
        Player p;
        p.equipment.equip(Item{"iron_sword",1});
        int dmgBefore = p.meleeDamage();
        int souls = 1000000;
        // 1 ponto rende +0.12 (trunca): compra 10 p/ ganho visível
        for (int i = 0; i < 10; ++i)
            assert(p.attrs.buy(Attr::Strength, souls));
        p.refreshDerived();
        int dmgAfter = p.meleeDamage();
        assert(dmgAfter > dmgBefore);
        std::printf("test_buy_str_changes_damage OK\n");
    }
    { // test_buy_vit_changes_hp
        Player p;
        int hpBefore = p.hpMax;
        int souls = 1000000;
        assert(p.attrs.buy(Attr::Vitality, souls));
        p.refreshDerived();
        assert(p.hpMax > hpBefore);
        std::printf("test_buy_vit_changes_hp OK\n");
    }
    { // test_equip_weapon_changes_damage
        Player p;
        // Seed nasce equipado: desequipa p/ medir soco antes.
        // (No seed a espada soma +0: bônus escala de attrs 10 é zero.
        // Com FOR 20 o bônus aparece e a espada supera o soco.)
        int souls = 1000000;
        for (int i = 0; i < 10; ++i)
            assert(p.attrs.buy(Attr::Strength, souls));
        p.refreshDerived();
        p.equipment.unequip(core::EquipSlot::RightHand);
        int dmgUnarmed = p.meleeDamage();
        p.equipment.equip(Item{"iron_sword",1});
        int dmgSword = p.meleeDamage();
        assert(dmgSword > dmgUnarmed);
        std::printf("test_equip_weapon_changes_damage OK\n");
    }

    std::printf("attr_integration test OK\n");
    return 0;
}
