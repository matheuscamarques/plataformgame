/**
 * @file tests/test_player_resistances.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava resistências do player (Fase 1 elementais).
 * @details Cobre derivação por END/VIT/FTH + universal por nível, hurt com tipo e esqueleto físico/fogo, roda com make test que compila em build/tests/test_player_resistances.
 */

#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"

int main() {
    using core::Attr;
    using core::DamageType;

    { // BaseSeed (Nv15: universal 0.972 em tudo, sem viés)
        Player p;
        assert(p.attrs.level() == 15);
        for (int i = 0; i < 4; ++i) {
            const float r =
                p.computeResistances().get(static_cast<DamageType>(i));
            assert(r < 1.f && r > 0.9f); // só universal, sem viés
        }
    }
    { // EndProtectsPhysical (END 20: físico 0.95, frost 0.97)
        Player p;
        int souls = 100000;
        for (int i = 0; i < 10; ++i)
            assert(p.attrs.buy(Attr::Endurance, souls));
        p.refreshDerived();
        const auto r = p.computeResistances();
        assert(r.get(DamageType::Physical) < 1.f);
        assert(r.get(DamageType::Frost) < 1.f);
        assert(r.get(DamageType::Physical) < r.get(DamageType::Frost));
        assert(r.get(DamageType::Fire) < 1.f); // universal por nível
    }
    { // HurtAppliesType (fogo 0.5 filtra; físico passa cheio)
        Player p;
        p.resistances_.set(DamageType::Fire, 0.5f);
        p.hp = 100;
        assert(p.hurt(20, DamageType::Fire));
        assert(p.hp == 90); // 20 * 0.5
        p.hurtIframes.tick(1.f); // zera i-frame p/ 2o golpe
        assert(p.hurt(20, DamageType::Physical));
        assert(p.hp == 71); // 20 × universal Nv15 (0.972) = 19
    }
    { // SkeletonBoneAndBurn (físico 0.7, fogo 1.3 no arquétipo)
        const support::EnemyArchetype *a =
            support::ArchetypeRegistry::instance().find("skeleton");
        assert(a != nullptr);
        assert(a->resistances.get(DamageType::Physical) == 0.7f);
        assert(a->resistances.get(DamageType::Fire) == 1.3f);
        auto e = support::Factory::spawnEnemy("skeleton", 0.f, 0.f);
        assert(e != nullptr);
        assert(e->resources.resistances.get(DamageType::Physical) == 0.7f);
        e->resources.hp = 100;
        e->resources.hpMax = 100;
        assert(e->resources.takeDamage(10, DamageType::Physical) == 7);
        assert(e->resources.takeDamage(10, DamageType::Fire) == 13);
    }

    { // WeaponBuffExpires (30s volta p/ físico; 0 = permanente)
        Player p;
        p.weaponBuffType = core::DamageType::Frost;
        p.weaponBuffTimer = 30.f;
        for (int i = 0; i < 950; ++i) p.tick(); // margem float de 1/30
        assert(p.weaponBuffTimer <= 0.f);
        assert(p.weaponBuffType == core::DamageType::Physical);
        p.weaponBuffType = core::DamageType::Fire;
        p.weaponBuffTimer = 0.f;
        for (int i = 0; i < 100; ++i) p.tick();
        assert(p.weaponBuffType == core::DamageType::Fire); // sem timer: fica
    }

    std::printf("player_resistances test OK\n");
    return 0;
}
