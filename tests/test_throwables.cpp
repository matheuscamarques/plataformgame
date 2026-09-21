#include <cassert>
#include <cstdio>
#include <cstring>
#include <set>

#include "assets/Sprites/ThrowableSprites.h"
#include "support/Effects/ThrowSystem.h"

// TNT + blast: arte travada por strlen (sem GL) e lógica do anel
// (spawn → tick → expira). renderBlasts() nunca é chamado aqui.
int main() {
    using namespace sprites;

    { // TntWidths (6x8 nos 3 frames; linha errada = sprite deslocada)
        const char *const *frames[] = {kTntFresh, kTntBurning, kTntCritical};
        for (auto f : frames)
            for (int y = 0; y < kTntH; ++y)
                assert(std::strlen(f[y]) == static_cast<std::size_t>(kTntW));
        assert(kTntPalCount == 5u);
    }
    { // TntPalNoDup (1 char = 1 cor)
        std::set<char> seen;
        for (std::size_t i = 0; i < kTntPalCount; ++i)
            assert(seen.insert(kTntPal[i].ch).second);
    }
    { // TntFuseBurnsDown (pavio encurta: s/F descem por frame)
        auto fuseRow = [](const char *const *f) {
            for (int y = 0; y < kTntH; ++y)
                if (std::strchr(f[y], 's')) return y;
            return -1;
        };
        // Faísca desce: fresh row 0 → burning row 1 → critical row 2.
        assert(fuseRow(kTntFresh) == 0);
        assert(fuseRow(kTntBurning) == 1);
        assert(fuseRow(kTntCritical) == 2);
    }
    { // BlastLifecycle (spawn → ativo → expira em maxTtl)
        support::ThrowSystem ts;
        assert(ts.activeBlastCount() == 0u);
        ts.spawnBlast({100.f, 200.f}, 40.f);
        assert(ts.activeBlastCount() == 1u);
        ts.tickBlasts(0.1f);
        assert(ts.activeBlastCount() == 1u);
        ts.tickBlasts(0.3f); // total 0.4 > 0.35
        assert(ts.activeBlastCount() == 0u);
    }
    { // BlastClear (restart da run não deixa anel preso)
        support::ThrowSystem ts;
        ts.spawnBlast({0.f, 0.f}, 40.f);
        ts.spawnBlast({10.f, 10.f}, 60.f);
        ts.clearBlasts();
        assert(ts.activeBlastCount() == 0u);
    }

    std::printf("throwables test OK\n");
    return 0;
}
