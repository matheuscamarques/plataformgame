/**
 * @file tests/test_resistances.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava DamageType + Resistances (Fase 1 elementais).
 * @details Cobre nomes, get/set/scale com clamp 0..3 e applyResistance, roda com make test que compila em build/tests/test_resistances.
 */

#include <cassert>
#include <cstdio>
#include <string>

#include "core/DamageType.h"
#include "core/Resistances.h"

int main() {
    using core::DamageType;

    { // NamesCoverEnum (todo tipo tem nome, sem "?")
        assert(std::string(core::damageTypeName(DamageType::Physical)) ==
               "physical");
        assert(std::string(core::damageTypeName(DamageType::Fire)) == "fire");
        assert(std::string(core::damageTypeName(DamageType::Frost)) == "frost");
        assert(std::string(core::damageTypeName(DamageType::Lightning)) ==
               "lightning");
        assert(static_cast<int>(DamageType::COUNT) == 4);
    }
    { // DefaultNeutral (tudo 1.0)
        core::Resistances r;
        for (int i = 0; i < 4; ++i)
            assert(r.get(static_cast<DamageType>(i)) == 1.f);
        assert(core::applyResistance(100, DamageType::Fire, r) == 100);
    }
    { // SetScaleClamp (0..3, escala multiplica)
        core::Resistances r;
        r.set(DamageType::Physical, 0.7f);
        assert(r.get(DamageType::Physical) == 0.7f);
        r.scale(DamageType::Physical, 0.5f);
        assert(r.get(DamageType::Physical) == 0.35f);
        r.set(DamageType::Fire, 99.f);
        assert(r.get(DamageType::Fire) == 3.f); // clamp alto
        r.set(DamageType::Frost, -5.f);
        assert(r.get(DamageType::Frost) == 0.f); // clamp baixo
    }
    { // ApplyTruncates (física 0.83, fogo 1.3 — esqueleto)
        core::Resistances r;
        r.set(DamageType::Physical, 0.7f);
        r.set(DamageType::Fire, 1.3f);
        assert(core::applyResistance(12, DamageType::Physical, r) == 8);
        assert(core::applyResistance(20, DamageType::Fire, r) == 26);
        assert(core::applyResistance(10, DamageType::Lightning, r) == 10);
    }

    std::printf("resistances test OK\n");
    return 0;
}
