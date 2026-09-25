/**
 * @file tests/test_player_derived.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava derivados por atributo (pós-fix STR).
 * @details Cobre VIT->hpMax, END->stamina/load, ATT->slots/FP, RES->threshold, FTH->lightning, STR->universal, cadeia buy+refresh e DEX scaling, roda com make test que compila em build/tests/test_player_derived.
 */

#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"

namespace {

void buyN(Player &p, core::Attr a, int n) {
    int souls = 1000000000;
    for (int i = 0; i < n; ++i) assert(p.attrs.buy(a, souls));
    p.refreshDerived();
}

} // namespace

int main() {
    using core::Attr;
    using core::DamageType;

    { // VitRaisesHp (compra reflete no hpMax)
        Player p;
        const int before = p.hpMax;
        buyN(p, Attr::Vitality, 10);
        assert(p.hpMax > before);
    }
    { // EndRaisesStaminaAndLoad
        Player p;
        const float st = p.staminaMax, ld = p.maxEquipLoad();
        buyN(p, Attr::Endurance, 10);
        assert(p.staminaMax > st);
        assert(p.maxEquipLoad() > ld);
    }
    { // AttRaisesSlotsAndFp
        Player p;
        const int sl = p.spellSlots();
        const float fp = p.fpMax;
        buyN(p, Attr::Attunement, 10); // ATT 20
        assert(p.spellSlots() >= sl && p.fpMax > fp);
    }
    { // ResRaisesThreshold (buildup de poison/bleed/frost mais lento)
        Player p;
        const float th = p.statusThreshold();
        buyN(p, Attr::Resistance, 10);
        assert(p.statusThreshold() > th);
    }
    { // FaithProtectsLightning
        Player p;
        const float before =
            p.computeResistances().get(DamageType::Lightning);
        buyN(p, Attr::Faith, 20);
        assert(p.computeResistances().get(DamageType::Lightning) < before);
    }
    { // UniversalPerLevel (STR nao da resistencia, mas nivel protege)
        Player p;
        const float before =
            p.computeResistances().get(DamageType::Fire);
        buyN(p, Attr::Strength, 10); // STR nao protege fogo direto
        assert(p.computeResistances().get(DamageType::Fire) < before);
    }
    { // LevelupChain (buy + refresh: a UI nao pode esquecer o refresh)
        Player p;
        const int hp = p.hpMax;
        int souls = 99999;
        assert(p.attrs.buy(Attr::Vitality, souls));
        p.refreshDerived();
        assert(p.hpMax > hp);
    }
    { // DexScalesIronSword (grade B rende mais que D)
        Player p; // seed: espada, tudo 10
        const int base = p.meleeDamage();
        buyN(p, Attr::Dexterity, 10); // DEX 20
        const int dex = p.meleeDamage();
        assert(dex > base);
        buyN(p, Attr::Strength, 10); // STR 20 junto
        assert(p.meleeDamage() > dex); // soma das duas escalas
    }

    std::printf("player_derived test OK\n");
    return 0;
}
