/**
 * @file tests/test_parry.cpp
 * @brief Teste headless do parry (Fase 2.2): janela, negação, riposte.
 * @details Windup rebate físico (sem dano/i-frame) e arma riposte ×1.5
 * por 3s; Active/Recovery e magia passam normal. Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include "core/DamageType.h"
#include "entities/Player/Player.h"

int main() {
    using core::DamageType;

    { // WindowIdleApplies (sem swing: dano entra normal)
        Player p;
        assert(p.hp == 100);
        assert(p.hurt(10, DamageType::Physical));
        assert(p.hp == 90);
        assert(p.riposteT_ == 0.f);
        assert(!p.consumeParryFx());
    }
    { // WindupParriesPhysical (rebate: sem dano, arma riposte + fx)
        Player p;
        assert(p.startSwing());
        assert(p.parryWindowActive());
        assert(!p.hurt(10, DamageType::Physical));
        assert(p.hp == 100);
        assert(p.riposteT_ > 0.f);
        assert(p.consumeParryFx());
        assert(!p.consumeParryFx()); // 1 dose
    }
    { // ActiveTakesNormal (janela é só o Windup)
        Player p;
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        assert(!p.parryWindowActive());
        assert(p.hurt(10, DamageType::Physical));
        assert(p.hp == 90);
        assert(p.riposteT_ == 0.f);
    }
    { // RecoveryTakesNormal (fora do Windup, apanha)
        Player p;
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        assert(p.updateMelee(0.10f) == MeleePhase::Recovery);
        assert(p.hurt(10, DamageType::Physical));
        assert(p.hp == 90);
    }
    { // MagicUnparryable (frost no Windup entra normal)
        Player p;
        assert(p.startSwing());
        assert(p.hurt(10, DamageType::Frost));
        assert(p.hp < 100);
        assert(p.riposteT_ == 0.f);
        assert(!p.consumeParryFx());
    }
    { // RiposteExactUnarmed (soco 8 → 12 por 3s, depois volta)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand);
        assert(p.meleeDamage() == 8);
        assert(p.startSwing());
        assert(!p.hurt(10, DamageType::Physical));
        assert(p.meleeDamage() == 12); // 8 × 1.5 exato
        for (int i = 0; i < 91; ++i) p.tick(); // 3.03s: expira
        assert(p.riposteT_ == 0.f);
        assert(p.meleeDamage() == 8);
    }
    { // RespawnClears (morte não carrega riposte nem fx)
        Player p;
        assert(p.startSwing());
        assert(!p.hurt(10, DamageType::Physical));
        assert(p.riposteT_ > 0.f);
        p.respawn(0.f, 0.f);
        assert(p.riposteT_ == 0.f);
        assert(!p.consumeParryFx());
        assert(p.hp == p.effectiveHpMax());
    }

    std::printf("parry test OK\n");
    return 0;
}
