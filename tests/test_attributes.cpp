#include <cassert>
#include <cstdio>

#include "core/Attributes.h"

// F2: pontos, nível, custo DS1 e compra atômica. Headless puro.
int main() {
    using namespace core;

    { // BaseLevel1 (8×10, nada gasto = nível 1)
        Attributes at;
        assert(at.level() == 1 && at.spent() == 0);
        for (int i = 0; i < kAttrCount; ++i)
            assert(at.get(static_cast<Attr>(i)) == 10);
        assert(kAttrCount == 8);
    }
    { // CostMatchesGuide (50→51 = 14535; monótono crescente)
        assert(Attributes::costForLevel(50) == 14535);
        int prev = Attributes::costForLevel(1);
        assert(prev > 0);
        for (int lv = 2; lv <= 200; ++lv) {
            const int c = Attributes::costForLevel(lv);
            assert(c > prev); // estritamente crescente até 200
            prev = c;
        }
    }
    { // BuySpendsSouls (custo do nível atual, +1 ponto e +1 nível)
        Attributes at;
        int souls = 100000;
        const int c1 = Attributes::costForLevel(1);
        assert(at.buy(Attr::Vitality, souls));
        assert(at.get(Attr::Vitality) == 11 && at.level() == 2);
        assert(souls == 100000 - c1);
    }
    { // BuyAtomic (sem souls ou no cap: false, nada muda)
        Attributes at;
        int broke = 10;
        assert(!at.buy(Attr::Strength, broke));
        assert(broke == 10 && at.get(Attr::Strength) == 10);
        int rich = 1000000000;
        for (int i = 0; i < 200 && at.get(Attr::Faith) < 99; ++i)
            assert(at.buy(Attr::Faith, rich));
        assert(at.get(Attr::Faith) == 99);
        assert(!at.buy(Attr::Faith, rich)); // cap: nem com souls
    }
    { // NamesCoverAll (8 nomes PT, sem "?")
        for (int i = 0; i < kAttrCount; ++i) {
            const char* n = attrName(static_cast<Attr>(i));
            assert(n != nullptr && n[0] != '?' && n[0] != '\0');
        }
    }

    std::printf("attributes test OK\n");
    return 0;
}
