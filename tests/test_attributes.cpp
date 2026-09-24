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
    { // DerivedFormulas (VIT10 = 10000; soft cap 40; END10 = 150/60)
        assert(Attributes::maxHP(10) == 10000);
        assert(Attributes::maxHP(40) == 16000);
        assert(Attributes::maxHP(41) - Attributes::maxHP(40) == 50);
        assert(Attributes::maxHP(1) == 8200);
        int prev = Attributes::maxHP(1);
        for (int v = 2; v <= 99; ++v) {
            const int h = Attributes::maxHP(v);
            assert(h > prev);
            prev = h;
        }
        assert(Attributes::maxStamina(10) == 150);
        assert(Attributes::maxLoad(10) == 60.f);
        assert(Attributes::maxLoad(20) == 80.f);
    }
    { // ScaleMath (tabela S-E; fator 0 na base, 1.0 aos 40)
        assert(scaleMult(ScaleGrade::S) == 1.0f);
        assert(scaleMult(ScaleGrade::A) == 0.8f);
        assert(scaleMult(ScaleGrade::B) == 0.6f);
        assert(scaleMult(ScaleGrade::C) == 0.4f);
        assert(scaleMult(ScaleGrade::D) == 0.2f);
        assert(scaleMult(ScaleGrade::E) == 0.1f);
        assert(scaleMult(ScaleGrade::None) == 0.f);
        assert(scaleFactor(10) == 0.f); // seed não muda dano
        assert(scaleFactor(40) == 1.0f);
        assert(scaleFactor(25) == 0.5f);
        assert(scaleFactor(1) == 0.f); // clamp, sem negativo
        assert(scaleLetter(ScaleGrade::B) == 'B' && scaleLetter(ScaleGrade::None) == '-');
    }

    std::printf("attributes test OK\n");
    return 0;
}
