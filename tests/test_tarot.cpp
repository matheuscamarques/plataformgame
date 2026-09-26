/**
 * @file tests/test_tarot.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless do tarô (fundação v1, sem UI/drop).
 * @details Cobre registry (6 cartas), agregado linear, ganchos vivos (dano, taken, stamina, hpMax) e morte limpa, roda com make test que compila em build/tests/test_tarot.
 */

#include <cassert>
#include <cstdio>
#include <string>

#include "entities/Player/Player.h"

int main() {
    using core::TarotArcana;

    { // RegistryHasSix (nomes PT, efeitos nos defaults certos)
        const auto &keys = core::TarotRegistry::instance().keys();
        assert(keys.size() == 6u);
        assert(std::string(core::tarotName(TarotArcana::Devil)) ==
               "O Diabo");
        const auto *d =
            core::TarotRegistry::instance().find(TarotArcana::Devil);
        assert(d != nullptr);
        assert(d->effect.damageMult == 1.25f);
        assert(d->effect.damageTakenMult == 1.25f);
        assert(core::TarotRegistry::instance().find(
                   TarotArcana::COUNT) == nullptr);
    }
    { // AggregateLinear (Diabo ×2 = 1.5× dano e recebido)
        Player p;
        assert(p.tarotFx.damageMult == 1.f);
        p.addTarotCard(TarotArcana::Devil);
        assert(p.tarotCards[TarotArcana::Devil] == 1);
        assert(p.tarotFx.damageMult == 1.25f);
        p.addTarotCard(TarotArcana::Devil);
        assert(p.tarotFx.damageMult == 1.5f);
        assert(p.tarotFx.damageTakenMult == 1.5f);
        p.addTarotCard(TarotArcana::Fool);
        assert(p.tarotFx.damageMult == 1.5f * 1.05f); // cartas multiplicam
    }
    { // HooksAlive (melee, taken, stamina, hpMax respondem)
        Player p;
        const int bare = p.meleeDamage();
        p.addTarotCard(TarotArcana::Devil);
        assert(p.meleeDamage() > bare); // dano entra no breakdown
        assert(p.computeModifiers().damageTakenMult == 1.25f);
        Player s;
        s.addTarotCard(TarotArcana::Star);
        assert(s.computeModifiers().staminaRegenMult == 1.15f);
        assert(s.computeModifiers().staminaCostMult == 1.10f);
        Player e;
        const int hpBare = e.hpMax;
        e.addTarotCard(TarotArcana::Empress);
        e.refreshDerived();
        assert(e.effectiveHpMax() > hpBare); // hpMaxMult conta
    }
    { // UnknownIgnored (arcana fora do registry não soma)
        Player p;
        p.addTarotCard(TarotArcana::COUNT);
        assert(p.tarotCards.empty());
        assert(p.tarotFx.damageMult == 1.f);
    }
    { // DeathClears (morreu, zerou; morte limpa run)
        Player p;
        p.addTarotCard(TarotArcana::Devil);
        assert(p.tarotFx.damageMult == 1.25f);
        p.respawn(0.f, 0.f);
        assert(p.tarotCards.empty());
        assert(p.tarotFx.damageMult == 1.f);
    }

    std::printf("tarot test OK\n");
    return 0;
}
