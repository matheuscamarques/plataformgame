/**
 * @file tests/test_weapon_registry.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava sword e axe com sprite e origem corretos.
 * @details Cobre WeaponRegistry e WeaponDef, roda com make test que compila em build/tests/test_weapon_registry.
 */

#include <cassert>
#include <cstdio>

#include "support/Combat/WeaponRegistry.h"

int main() {
    using namespace support;

    { // SwordRegistered
        const WeaponDef *d = WeaponRegistry::instance().find("sword");
        assert(d != nullptr);
        assert(d->spriteW == 16 && d->spriteH == 8);
        assert(d->originX == 5.f && d->originY == 5.f);
    }
    { // AxeRegistered
        const WeaponDef *d = WeaponRegistry::instance().find("axe");
        assert(d != nullptr);
        assert(d->spriteW == 8 && d->spriteH == 20);
        assert(d->originX == 4.f && d->originY == 10.f);
    }
    { // UnknownReturnsNull
        assert(WeaponRegistry::instance().find("dagger") == nullptr);
    }
    { // KeysHoldAll (base + variantes por item do Equipment)
        const auto &keys = WeaponRegistry::instance().keys();
        assert(keys.size() == 12u);
        assert(keys[0] == "sword" && keys[1] == "axe");
        assert(keys[2] == "iron_sword" && keys[3] == "iron_axe");
        assert(keys[4] == "gold_sword" && keys[5] == "gold_axe");
        assert(keys[6] == "diamond_sword" && keys[7] == "diamond_axe");
        assert(keys[8] == "leather_sword" && keys[9] == "leather_axe");
        assert(keys[10] == "wooden_staff" && keys[11] == "priest_bell");
    }
    { // ItemVariantsMirrorBase (mesma geometria de sword/axe)
        const WeaponDef *s = WeaponRegistry::instance().find("iron_sword");
        const WeaponDef *a = WeaponRegistry::instance().find("iron_axe");
        assert(s && s->spriteW == 16 && s->hasSwingPhases);
        assert(a && a->spriteW == 8 && !a->hasSwingPhases);
        const WeaponDef *st = WeaponRegistry::instance().find("wooden_staff");
        const WeaponDef *bl = WeaponRegistry::instance().find("priest_bell");
        assert(st && st->spriteW == 8 && st->spriteH == 20 &&
               !st->hasSwingPhases);
        assert(bl && bl->spriteW == 8 && bl->spriteH == 20 &&
               !bl->hasSwingPhases);
    }

    std::printf("weapon registry test OK\n");
    return 0;
}
