/**
 * @file tests/test_attr_derived_chain.cpp
 * @brief Testes de cadeia atributo → derivados via Player buy + refreshDerived.
 * @details Cobre VIT→hpMax, END→staminaMax/maxEquipLoad, ATT→spellSlots/fpMax.
 */

#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"

int main() {
    using namespace core;

    { // test_attr_vit_hp
        Player p;
        int hpBefore = p.hpMax;
        int souls = 1000000;
        assert(p.attrs.buy(Attr::Vitality, souls));
        p.refreshDerived();
        assert(p.hpMax > hpBefore);
        std::printf("test_attr_vit_hp OK\n");
    }
    { // test_attr_end_stamina_load
        Player p;
        float stamBefore = p.staminaMax;
        float loadBefore = p.maxEquipLoad();
        int souls = 1000000;
        assert(p.attrs.buy(Attr::Endurance, souls));
        p.refreshDerived();
        assert(p.staminaMax > stamBefore);
        assert(p.maxEquipLoad() > loadBefore);
        std::printf("test_attr_end_stamina_load OK\n");
    }
    { // test_attr_att_slots_fp
        Player p;
        int slotsBefore = p.spellSlots();
        int souls = 1000000;
        // levar ATT para 12
        for (int i = 0; i < 3 && p.attrs.get(Attr::Attunement) < 12; ++i) {
            assert(p.attrs.buy(Attr::Attunement, souls));
        }
        p.refreshDerived();
        assert(p.spellSlots() > slotsBefore);
        assert(p.fpMax > 0);
        std::printf("test_attr_att_slots_fp OK\n");
    }
    { // test_attr_res_status_threshold
        Player p;
        float thBefore = p.statusThreshold();
        int souls = 1000000;
        assert(p.attrs.buy(Attr::Resistance, souls));
        p.refreshDerived();
        assert(p.statusThreshold() >= thBefore);
        std::printf("test_attr_res_status_threshold OK\n");
    }
    { // test_attr_fth_lightning_resist
        Player p;
        float resBefore = p.resistances_.get(core::DamageType::Lightning);
        int souls = 1000000;
        for (int i = 0; i < 20 && p.attrs.get(Attr::Faith) < 30; ++i) {
            assert(p.attrs.buy(Attr::Faith, souls));
        }
        p.refreshDerived();
        // FTH aumenta resistência universal + específica
        assert(p.resistances_.get(core::DamageType::Lightning) != resBefore);
        std::printf("test_attr_fth_lightning_resist OK\n");
    }
    { // test_attr_int_no_melee
        Player p;
        p.equipment.equip(core::Item{"iron_sword",1});
        int dmgBefore = p.meleeDamage();
        int souls = 1000000;
        assert(p.attrs.buy(Attr::Intelligence, souls));
        p.refreshDerived();
        // iron_sword não tem intScale
        assert(p.meleeDamage() == dmgBefore);
        std::printf("test_attr_int_no_melee OK\n");
    }

    std::printf("attr_derived_chain test OK\n");
    return 0;
}
