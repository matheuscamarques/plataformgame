/**
 * @file tests/test_enemy_resources.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava mana e stamina com custo, regen e gate.
 * @details Cobre EnemyResources e Cost, roda com make test que compila em build/tests/test_enemy_resources.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/Enemies/EnemyResources.h"

using support::Cost;
using support::EnemyResources;

static bool near(float a, float b) { return std::fabs(a - b) < 1e-5f; }
static bool nearLoose(float a, float b) { return std::fabs(a - b) < 0.5f; }

static EnemyResources full() {
    EnemyResources r;
    r.manaMax = r.mana = 10.f;
    r.staminaMax = r.stamina = 20.f;
    r.postureMax = r.posture = 30.f;
    return r;
}

int main() {
    // canPay
    assert(full().canPay({5.f, 10.f, 15.f}));
    { auto r = full(); assert(!r.canPay({50.f, 0.f, 0.f})); }
    { auto r = full(); r.stamina = 5.f; assert(!r.canPay({0.f, 10.f, 0.f})); }
    { // trash ignora mana/stamina, mas paga postura
        EnemyResources r;
        r.isTrash = true;
        r.posture = r.postureMax = 50.f;
        assert(r.canPay({100.f, 100.f, 10.f}));
        assert(!r.canPay({0.f, 0.f, 60.f}));
    }

    // pay
    {
        auto r = full();
        r.pay({5.f, 10.f, 15.f});
        assert(near(r.mana, 5.f) && near(r.stamina, 10.f) && near(r.posture, 15.f));
    }
    {
        auto r = full();
        r.pay({5.f, 0.f, 0.f});
        assert(r.manaRegenDelay.running());
    }
    { // trash pay não toca mana/stamina
        EnemyResources r;
        r.isTrash = true;
        r.mana = 100.f;
        r.stamina = 100.f;
        r.posture = r.postureMax = 50.f;
        r.pay({50.f, 50.f, 10.f});
        assert(near(r.mana, 100.f) && near(r.stamina, 100.f) && near(r.posture, 40.f));
    }

    // regen
    {
        EnemyResources r;
        r.manaMax = 100.f;
        r.mana = 50.f;
        r.manaRegen = 10.f;
        r.manaRegenDelay = core::Cooldown(2.0f);
        r.pay({10.f, 0.f, 0.f});
        r.tick(1.0f); // dentro do delay: nada
        assert(near(r.mana, 40.f));
    }
    {
        EnemyResources r;
        r.manaMax = 100.f;
        r.mana = 50.f;
        r.manaRegen = 10.f;
        r.manaRegenDelay = core::Cooldown(1.0f);
        r.pay({10.f, 0.f, 0.f}); // mana = 40, delay = 1.0
        r.tick(1.0f);            // delay zera E regen aplica no mesmo tick
        assert(nearLoose(r.mana, 50.f));
        r.tick(1.0f);            // +10 de novo
        assert(nearLoose(r.mana, 60.f));
    }
    {
        EnemyResources r;
        r.manaMax = 100.f;
        r.mana = 95.f;
        r.manaRegen = 100.f;
        r.manaRegenDelay = core::Cooldown(0.f);
        r.tick(1.0f);
        assert(near(r.mana, 100.f)); // clamp no máximo
    }
    {
        EnemyResources r;
        r.isTrash = true;
        r.manaMax = 100.f;
        r.mana = 0.f;
        r.manaRegen = 100.f;
        r.manaRegenDelay = core::Cooldown(0.f);
        r.tick(1.0f);
        assert(near(r.mana, 0.f)); // trash não regen mana
    }

    // stagger
    {
        EnemyResources r;
        r.postureMax = r.posture = 20.f;
        r.damagePosture(20.f);
        assert(r.staggered() && near(r.posture, 0.f));
    }
    {
        EnemyResources r;
        r.postureMax = r.posture = 20.f;
        r.damagePosture(10.f);
        assert(!r.staggered() && near(r.posture, 10.f));
    }
    {
        EnemyResources r;
        r.staggerDuration = 0.8f;
        r.postureMax = r.posture = 20.f;
        r.damagePosture(20.f);
        assert(r.staggered());
        r.tick(1.0f);
        assert(!r.staggered());
    }
    {
        EnemyResources r;
        r.postureMax = r.posture = 20.f;
        r.pay({0.f, 0.f, 20.f});
        assert(r.staggered());
    }

    // takeDamage
    {
        EnemyResources r;
        r.hp = r.hpMax = 10;
        assert(r.takeDamage(5) == 5 && r.hp == 5);
    }
    {
        EnemyResources r;
        r.hp = r.hpMax = 10;
        assert(r.takeDamage(50) == 10 && r.hp == 0 && r.isDead());
    }
    {
        EnemyResources r;
        r.hp = r.hpMax = 10;
        r.takeDamage(10);
        assert(r.takeDamage(5) == 0 && r.hp == 0);
    }
    {
        EnemyResources r;
        r.hp = r.hpMax = 10;
        assert(r.takeDamage(-5) == 0 && r.hp == 10);
    }

    std::printf("enemy resources test OK\n");
    return 0;
}
