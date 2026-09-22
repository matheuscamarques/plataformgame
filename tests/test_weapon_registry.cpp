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
        assert(keys.size() == 4u);
        assert(keys[0] == "sword" && keys[1] == "axe");
        assert(keys[2] == "iron_sword" && keys[3] == "iron_axe");
    }
    { // ItemVariantsMirrorBase (mesma geometria de sword/axe)
        const WeaponDef *s = WeaponRegistry::instance().find("iron_sword");
        const WeaponDef *a = WeaponRegistry::instance().find("iron_axe");
        assert(s && s->spriteW == 16 && s->hasSwingPhases);
        assert(a && a->spriteW == 8 && !a->hasSwingPhases);
    }

    std::printf("weapon registry test OK\n");
    return 0;
}
