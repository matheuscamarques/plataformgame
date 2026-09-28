/**
 * @file tests/test_player_class.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless das 6 classes iniciais e applyClass.
 * @details Cobre classDef (nomes, bases, ids existentes), applyClass
 * (attrs, equipamento, itens, magias sintonizadas, souls zero, vitais
 * cheios) e Attributes::set com clamp, roda com make test que compila
 * em build/tests/test_player_class.
 */

#include <cassert>
#include <cstdio>
#include <string>

#include "core/PlayerClass.h"
#include "entities/Player/Player.h"

int main() {
    using core::Attr;
    using core::PlayerClass;

    { // ClassTable (6 nomes, bases na tabela, ids existentes)
        assert(std::string(core::className(PlayerClass::Knight)) ==
               "Cavaleiro");
        assert(std::string(core::className(PlayerClass::Deprived)) ==
               "Desprovido");
        for (int i = 0; i < static_cast<int>(PlayerClass::COUNT); ++i) {
            const auto def =
                core::classDef(static_cast<PlayerClass>(i));
            for (const auto& id : def.equipment)
                assert(core::ItemRegistry::instance().find(id));
            for (const auto& [id, qty] : def.items) {
                assert(core::ItemRegistry::instance().find(id));
                assert(qty > 0);
            }
            for (const auto& id : def.spells)
                assert(core::ItemRegistry::instance().find(id));
        }
        const auto knight = core::classDef(PlayerClass::Knight);
        assert(knight.base[static_cast<int>(Attr::Vitality)] == 12);
        assert(knight.base[static_cast<int>(Attr::Strength)] == 12);
        const auto deprived = core::classDef(PlayerClass::Deprived);
        for (int i = 0; i < core::kAttrCount; ++i)
            assert(deprived.base[i] == 10);
    }
    { // AttrSetClamp (1..99, sem estouro)
        core::Attributes a;
        a.set(Attr::Strength, 14);
        assert(a.get(Attr::Strength) == 14);
        a.set(Attr::Strength, 0);
        assert(a.get(Attr::Strength) == 1);
        a.set(Attr::Strength, 999);
        assert(a.get(Attr::Strength) == 99);
    }
    { // ApplyKnight (attrs, espada+peitoral, 3 poções, souls zero)
        Player p;
        p.applyClass(PlayerClass::Knight);
        assert(p.attrs.get(Attr::Vitality) == 12);
        assert(p.attrs.get(Attr::Intelligence) == 8);
        assert(p.souls == 0);
        assert(p.weaponDef() &&
               p.weaponDef()->id == "iron_sword");
        assert(p.inventory.count("potion") == 3);
        assert(p.inventory.count("dynamite") == 0); // kit zerado
        assert(p.hp == p.hpMax && p.hp == p.effectiveHpMax());
        assert(p.attuned.empty()); // sem magia inicial
    }
    { // ApplyMage (cajado + soul_arrow sintonizada, 1 slot de ATT12)
        Player p;
        p.applyClass(PlayerClass::Mage);
        assert(p.attrs.get(Attr::Intelligence) == 14);
        assert(p.attrs.get(Attr::Attunement) == 12);
        assert(p.weaponDef() &&
               p.weaponDef()->id == "wooden_staff");
        assert(p.attuned.size() == 1u);
        assert(p.attuned[0] == "soul_arrow");
        assert(p.spellSlots() == 1);
    }
    { // ApplyCleric (sino + heal_light sintonizada)
        Player p;
        p.applyClass(PlayerClass::Cleric);
        assert(p.attrs.get(Attr::Faith) == 14);
        assert(p.weaponDef() &&
               p.weaponDef()->id == "priest_bell");
        assert(p.attuned.size() == 1u);
        assert(p.attuned[0] == "heal_light");
    }
    { // ApplyThief (leve + dinamites) e Deprived (nu + 1 poção)
        Player p;
        p.applyClass(PlayerClass::Thief);
        assert(p.attrs.get(Attr::Dexterity) == 14);
        assert(p.weaponDef() &&
               p.weaponDef()->id == "leather_sword");
        assert(p.inventory.count("dynamite") == 5);
        Player q;
        q.applyClass(PlayerClass::Deprived);
        for (int i = 0; i < core::kAttrCount; ++i)
            assert(q.attrs.get(static_cast<Attr>(i)) == 10);
        assert(!q.hasWeapon());
        assert(q.inventory.count("potion") == 1);
        assert(q.souls == 0);
    }
    { // ApplyBarbarian (machado, sem magia, vitais cheios)
        Player p;
        p.applyClass(PlayerClass::Barbarian);
        assert(p.attrs.get(Attr::Strength) == 14);
        assert(p.attrs.get(Attr::Vitality) == 14);
        assert(p.weaponDef() && p.weaponDef()->id == "iron_axe");
        assert(p.inventory.count("potion") == 2);
        assert(p.stamina == p.staminaMax && p.fp == p.fpMax);
    }

    std::printf("player class test OK\n");
    return 0;
}
