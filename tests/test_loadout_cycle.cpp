#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"

// Ciclo do M: Iron→Leather→Gold→Diamond→pelado→Iron (5 estados).
// Pelado = equipped false (sprite base sem nada, p/ teste).
int main() {
    using core::MaterialId;

    { // CyclesFourMaterialsThenBare (1 aperto = 1 estado)
        Player p;
        assert(p.loadout.equipped);
        assert(p.loadout.weapon == MaterialId::Iron);
        p.cycleMaterial();
        assert(p.loadout.weapon == MaterialId::Leather);
        assert(p.loadout.equipped);
        p.cycleMaterial();
        assert(p.loadout.weapon == MaterialId::Gold);
        p.cycleMaterial();
        assert(p.loadout.weapon == MaterialId::Diamond);
        p.cycleMaterial();
        assert(!p.loadout.equipped); // 5º estado: pelado
        p.cycleMaterial();
        assert(p.loadout.equipped); // fecha o ciclo no Iron
        assert(p.loadout.weapon == MaterialId::Iron);
    }
    { // BareKeepsSlotsSynced (volta com tudo no Iron)
        Player p;
        for (int i = 0; i < 4; ++i) p.cycleMaterial();
        assert(!p.loadout.equipped);
        p.cycleMaterial();
        assert(p.loadout.helm == MaterialId::Iron);
        assert(p.loadout.chest == MaterialId::Iron);
        assert(p.loadout.legs == MaterialId::Iron);
    }

    std::puts("loadout cycle test OK");
    return 0;
}
