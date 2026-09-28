/**
 * @file tests/test_tarot.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless do tarô (78 arcanos, fado forçado, sem UI/drop).
 * @details Cobre registry (78 cartas + tiers + pesos), agregado linear,
 * ganchos vivos (dano, taken/defesa, crítico, condicionais, cura, move,
 * souls/xp, resists, revive, conversão, killstack, maldição), vinheta e
 * morte limpa, roda com make test que compila em build/tests/test_tarot.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>

#include "entities/Player/Player.h"
#include "support/Lighting/LightingSystem.h"

namespace {

bool near(float a, float b) { return std::fabs(a - b) < 1e-4f; }

} // namespace

int main() {
    using core::TarotArcana;

    { // RegistryComplete (78 nomes PT, tiers 4/8/10/16/40, pesos)
        const auto &keys = core::TarotRegistry::instance().keys();
        assert(keys.size() == 78u);
        assert(std::string(core::tarotName(TarotArcana::Devil)) ==
               "O Diabo");
        assert(std::string(core::tarotName(TarotArcana::World)) ==
               "O Mundo");
        assert(std::string(core::tarotName(TarotArcana::WandsAce)) ==
               "Ás de Paus");
        assert(std::string(core::tarotName(TarotArcana::PentaclesKing)) ==
               "Rei de Ouros");
        for (int i = 0;
             i < static_cast<int>(TarotArcana::COUNT); ++i) {
            const auto *d = core::TarotRegistry::instance().find(
                static_cast<TarotArcana>(i));
            assert(d != nullptr);
            assert(!d->flavor.empty());
        }
        assert(core::TarotRegistry::instance().find(
                   TarotArcana::COUNT) == nullptr);
        // Tiers: 4 lendários, 8 épicos, 10 raros, 16 incomuns, 40 comuns.
        int leg = 0, epic = 0, rare = 0, unc = 0, com = 0;
        for (int i = 0;
             i < static_cast<int>(TarotArcana::COUNT); ++i) {
            switch (core::tierOf(static_cast<TarotArcana>(i))) {
                case core::TarotTier::Legendary: ++leg; break;
                case core::TarotTier::Epic:      ++epic; break;
                case core::TarotTier::Rare:      ++rare; break;
                case core::TarotTier::Uncommon:  ++unc; break;
                default:                         ++com; break;
            }
        }
        assert(leg == 4 && epic == 8 && rare == 10);
        assert(unc == 16 && com == 40);
        assert(core::weightOf(core::TarotTier::Common) == 1);
        assert(core::weightOf(core::TarotTier::Uncommon) == 2);
        assert(core::weightOf(core::TarotTier::Rare) == 5);
        assert(core::weightOf(core::TarotTier::Epic) == 10);
        assert(core::weightOf(core::TarotTier::Legendary) == 25);
        assert(core::rollTier(0.0f) == core::TarotTier::Common);
        assert(core::rollTier(0.8f) == core::TarotTier::Uncommon);
        assert(core::rollTier(0.95f) == core::TarotTier::Rare);
        assert(core::rollTier(0.99f) == core::TarotTier::Epic);
        assert(core::rollTier(0.999f) == core::TarotTier::Legendary);
    }
    { // AggregateLinear (Diabo ×2 = 1.5× dano e recebido; peso soma)
        Player p;
        assert(p.tarotFx.damageMult == 1.f);
        assert(p.totalTarotCards() == 0 && p.tarotWeight() == 0);
        p.addTarotCard(TarotArcana::Devil);
        assert(p.tarotCards[TarotArcana::Devil] == 1);
        assert(p.tarotFx.damageMult == 1.25f);
        assert(p.totalTarotCards() == 1 && p.tarotWeight() == 25);
        p.addTarotCard(TarotArcana::Devil);
        assert(p.tarotFx.damageMult == 1.5f);
        assert(p.tarotFx.damageTakenMult == 1.5f);
        assert(p.tarotWeight() == 50);
        p.addTarotCard(TarotArcana::Fool);
        assert(p.tarotFx.damageMult == 1.5f * 1.05f); // cartas multiplicam
        assert(p.tarotWeight() == 55); // 50 + 5 (Raro)
    }
    { // HooksAlive (melee, taken/defesa, stamina, hpMax respondem)
        Player p;
        const int bare = p.meleeDamage();
        p.addTarotCard(TarotArcana::Devil);
        assert(p.meleeDamage() > bare); // dano entra no breakdown
        assert(p.computeModifiers().damageTakenMult == 1.25f);
        assert(near(p.takenMult(), 1.25f)); // sem defesa: taken puro
        Player s;
        s.addTarotCard(TarotArcana::Star);
        assert(s.computeModifiers().staminaRegenMult == 1.15f);
        assert(s.computeModifiers().attackSpeedMult == 1.10f);
        assert(near(s.tarotFx.heavyDamageMult, 0.85f));
        Player e;
        const int hpBare = e.hpMax;
        e.addTarotCard(TarotArcana::Empress);
        e.refreshDerived();
        assert(e.effectiveHpMax() > hpBare); // hpMaxMult conta
        assert(near(e.tarotFx.healingReceivedMult, 1.15f));
        assert(near(e.computeModifiers().attackSpeedMult, 0.90f));
    }
    { // HooksEconomy (move, souls, cura, resists, FP)
        Player p;
        p.addTarotCard(TarotArcana::Chariot);
        assert(near(p.computeModifiers().moveSpeedMult, 1.15f));
        assert(near(p.tarotFx.movingDefenseMult, 0.90f));
        assert(near(p.tarotFx.chargeDamageMult, 1.10f));
        assert(near(p.takenMult(), 1.f)); // parado: defesa cheia
        Player w;
        w.addTarotCard(TarotArcana::WandsSix);
        assert(near(w.tarotFx.soulsGainMult, 1.10f));
        assert(near(w.tarotFx.xpGainMult, 0.95f));
        Player h;
        h.addTarotCard(TarotArcana::Hierophant);
        assert(near(h.tarotFx.healingReceivedMult, 1.20f));
        assert(near(h.tarotFx.critDamageMult, 0.90f));
        Player m;
        m.addTarotCard(TarotArcana::Magician);
        assert(near(m.tarotFx.magicDamageMult, 1.10f));
        assert(near(m.tarotFx.physicalDamageMult, 0.90f));
        assert(m.tarotFx.spellSlots == 0); // 1 cópia: sem slot
        m.addTarotCard(TarotArcana::Magician);
        assert(m.tarotFx.spellSlots == 1); // 2 cópias: +1 slot
        assert(m.spellSlots() == 3);       // base 2 (ATT18) + 1
        Player f;
        const float fireBare =
            f.computeResistances().get(core::DamageType::Fire);
        const float physBare =
            f.computeResistances().get(core::DamageType::Physical);
        f.addTarotCard(TarotArcana::Sun);
        assert(f.computeResistances().get(core::DamageType::Fire) >
               fireBare); // 1.15 = queima mais
        Player pr;
        pr.addTarotCard(TarotArcana::HighPriestess);
        assert(pr.computeResistances().get(core::DamageType::Physical) >
               physBare); // ferro entra
        assert(pr.computeResistances().get(core::DamageType::Lightning) <
               pr.computeResistances().get(core::DamageType::Physical));
    }
    { // HooksConditional (Amantes/Justiça/Eremita por contexto)
        Player p;
        p.addTarotCard(TarotArcana::Lovers);
        p.hp = p.effectiveHpMax(); // cheio: highHp 1.10
        const int full = p.meleeDamage();
        p.hp = 1; // <50%: lowHp 0.85
        const int low = p.meleeDamage();
        assert(low < full);
        Player j;
        j.addTarotCard(TarotArcana::Justice);
        assert(j.meleeDamageVs(1.f, false) > j.meleeDamageVs(0.5f, false));
        assert(j.meleeDamageVs(0.1f, false) < j.meleeDamageVs(0.5f, false));
        Player h;
        h.addTarotCard(TarotArcana::Hermit);
        assert(h.meleeDamageVs(1.f, true) > h.meleeDamageVs(1.f, false));
        // 9 de Paus: defesa com HP<25% (hurt real, sem movimento).
        Player w9;
        w9.addTarotCard(TarotArcana::WandsNine);
        assert(near(w9.tarotFx.lowHpDefenseMult, 1.10f));
        w9.hp = w9.effectiveHpMax();
        w9.hurt(20);
        const int dHigh = w9.effectiveHpMax() - w9.hp;
        const int lowHp = w9.effectiveHpMax() / 10; // <25%
        w9.hp = lowHp;
        w9.hurtIframes.reset();
        w9.hurt(20);
        const int dLow = lowHp - w9.hp;
        assert(dHigh > 0 && dLow > 0 && dLow < dHigh);
        // Carro: em movimento apanha mais (movingDefense 0.90).
        Player ch;
        ch.addTarotCard(TarotArcana::Chariot);
        ch.hp = ch.effectiveHpMax();
        ch.hurt(20);
        const int still = ch.effectiveHpMax() - ch.hp;
        ch.hp = ch.effectiveHpMax();
        ch.moveLeft = true;
        ch.hurtIframes.reset();
        ch.hurt(20);
        const int moving = ch.effectiveHpMax() - ch.hp;
        ch.moveLeft = false;
        assert(moving > still);
    }
    { // HooksFinisher (Estrela pesa, investida empurra, Roda roça)
        Player st; // mesmo combo 2, com e sem carta
        st.addTarotCard(TarotArcana::Star);
        st.meleeCombo = 2;
        Player stBare;
        stBare.meleeCombo = 2;
        assert(st.meleeDamage() < stBare.meleeDamage()); // heavy 0.85
        Player cg;
        cg.addTarotCard(TarotArcana::Chariot);
        cg.meleeCombo = 2;
        Player cgBare;
        cgBare.meleeCombo = 2;
        assert(cg.meleeDamage() > cgBare.meleeDamage()); // investida 1.10
        Player wh;
        wh.addTarotCard(TarotArcana::WheelOfFortune);
        assert(near(wh.tarotFx.precisionMult, 0.95f));
        // swingId 0: roça (período 20) — sem carta não roça.
        Player bare;
        assert(wh.meleeDamage() < bare.meleeDamage());
        wh.startSwing(); // id 1: fora do graze
        assert(wh.meleeDamage() == bare.meleeDamage());
    }
    { // HooksCosmic (dados p/ Spawn/Factory/Lighting)
        Player d;
        d.addTarotCard(TarotArcana::Devil);
        assert(near(d.tarotFx.spawnRateMult, 1.50f));
        assert(near(d.tarotFx.enemyHpMult, 1.30f));
        Player t;
        t.addTarotCard(TarotArcana::Tower);
        assert(near(t.tarotFx.critDamageMult, 1.30f));
        assert(near(t.tarotFx.defenseMult, 0.80f));
        assert(near(t.tarotFx.eliteChanceMult, 2.00f));
        Player mo;
        mo.addTarotCard(TarotArcana::Moon);
        assert(near(mo.tarotFx.nightSpawnMult, 2.00f));
        assert(near(mo.tarotFx.visionMult, 0.90f));
        assert(near(mo.tarotFx.fogMult, 2.00f));
        Player su;
        su.addTarotCard(TarotArcana::Sun);
        assert(near(su.tarotFx.lightMult, 1.50f));
        assert(near(su.tarotFx.daySpawnMult, 1.50f));
        Player wo;
        wo.addTarotCard(TarotArcana::World);
        assert(wo.tarotFx.bloodMoon);
        assert(near(wo.tarotFx.attrMult, 1.10f));
        assert(near(wo.tarotFx.spawnRateMult, 1.50f));
        assert(wo.computeResistances().get(core::DamageType::Fire) >
               wo.computeResistances().get(core::DamageType::Frost));
        // Lua de sangue é noite: fatefulNight pura (sem GL).
        const auto night =
            support::LightingSystem::fatefulNight(0.0f, 0.3f, 2.f, true);
        assert(night.blood);
        assert(near(night.moon, 0.15f)); // névoa come metade do luar
        const auto day =
            support::LightingSystem::fatefulNight(1.0f, 0.0f, 2.f, true);
        assert(!day.blood); // de dia, sem sangue
    }
    { // HooksSpecial (revive nega a morte; conversão; killstack; vinheta)
        Player p;
        p.addTarotCard(TarotArcana::Judgment);
        assert(p.tarotFx.reviveOnce && p.tarotFx.maxRevives == 1);
        p.hp = 10;
        assert(p.hurt(9999)); // lethal...
        assert(p.hp > 0);     // ...negada (30% do máx)
        p.hurtIframes.reset();
        assert(p.hurt(9999)); // revive gasto...
        assert(p.hp == 0);    // ...morre
        Player c;
        c.addTarotCard(TarotArcana::HangedMan);
        c.hp = c.effectiveHpMax();
        const int dmg = c.meleeDamage();
        c.hurt(20);
        assert(c.meleeDamage() > dmg); // dor virou bônus
        Player k;
        k.addTarotCard(TarotArcana::Death);
        assert(k.tarotFx.killStackMax == 5);
        const int kBare = k.meleeDamage();
        for (int i = 0; i < 7; ++i) k.addKillStack(); // teto 5
        assert(k.meleeDamage() > kBare); // +5% por stack (×1.25)
        Player v;
        assert(!v.tarotRevealActive());
        v.showTarotReveal(TarotArcana::World);
        assert(v.tarotRevealActive());
        v.tickTarot(2.f);
        assert(!v.tarotRevealActive()); // 1.5s some sozinha
    }
    { // CurseWeight (só acima de 100 cobra; DoT nunca mata)
        Player p;
        for (int i = 0; i < 4; ++i)
            p.addTarotCard(TarotArcana::Devil); // peso 100
        p.tickTarot(1.f);
        assert(near(p.computeModifiers().hpMaxMult,
                    p.tarotFx.hpMaxMult)); // 100 = seguro
        p.addTarotCard(TarotArcana::Fool); // peso 105
        p.tickTarot(1.f);
        assert(p.computeModifiers().hpMaxMult <
               p.tarotFx.hpMaxMult); // -5% HP
        p.hp = 5;
        p.tickTarot(600.f); // DoT de peso baixo não existe aqui...
        assert(p.hp >= 1);
        // ...mas em 500+ o veneno corre sem matar.
        Player q;
        for (int i = 0; i < 20; ++i)
            q.addTarotCard(TarotArcana::Devil); // peso 500
        q.addTarotCard(TarotArcana::Tower);     // peso 525
        q.hp = q.effectiveHpMax();
        q.tickTarot(60.f);
        assert(q.hp >= 1);
        assert(q.computeModifiers().moveSpeedMult < 1.f);
    }
    { // UnknownIgnored (arcana fora do registry não soma)
        Player p;
        p.addTarotCard(TarotArcana::COUNT);
        assert(p.tarotCards.empty());
        assert(p.tarotFx.damageMult == 1.f);
        assert(p.tarotWeight() == 0);
    }
    { // DeathKeeps (fado sobrevive: cartas, peso e revive gasto ficam)
        Player p;
        p.addTarotCard(TarotArcana::Devil);
        p.addKillStack();
        assert(p.tarotFx.damageMult == 1.25f);
        p.respawn(0.f, 0.f);
        assert(p.tarotCards[TarotArcana::Devil] == 1);
        assert(p.tarotFx.damageMult == 1.25f);
        assert(p.tarotWeight() == 25);
        assert(p.killStacks() == 0); // momento de combate zera
    }
    { // NeverZeroOrNegative (pilha de penalidades: dano > 0)
        Player p;
        for (int i = 0; i < 2; ++i) {
            p.addTarotCard(TarotArcana::Lovers);   // lowHp 0.85
            p.addTarotCard(TarotArcana::Justice);  // wounded 0.85
            p.addTarotCard(TarotArcana::Star);     // heavy 0.85
            p.addTarotCard(TarotArcana::WheelOfFortune); // graze
            p.addTarotCard(TarotArcana::CupsTen);  // dano 0.90
        }
        p.hp = 1; // força lowHp
        p.meleeCombo = 2; // força heavy + charge ausente
        // Alvo ferido + sozinho: tudo que reduz, aplicado junto.
        assert(p.meleeDamageVs(0.1f, false) > 0);
        assert(p.meleeDamage() > 0);
        // Recebido também nunca inverte (não cura ao apanhar).
        const int hpBefore = p.hp = p.effectiveHpMax();
        p.hurt(10);
        assert(p.hp <= hpBefore);
    }
    { // BurdenFate (fardo = peso/4 na carga; sem carta = zero)
        Player p;
        const float bare = p.equipLoad();
        assert(p.tarotLoad() == 0.f);
        p.addTarotCard(core::TarotArcana::Devil); // peso 25
        assert(p.tarotLoad() == 6.25f);
        assert(p.equipLoad() == bare + 6.25f);
        p.addTarotCard(core::TarotArcana::WandsTwo); // peso +1
        assert(p.tarotLoad() == 6.5f);
        p.respawn(0.f, 0.f); // morte não alivia o fardo
        assert(p.tarotLoad() == 6.5f);
        p.deleteCharacter();
        assert(p.tarotLoad() == 0.f);
    }
    { // DeleteCharacter (apaga tudo: única saída do fado)
        Player p;
        p.addTarotCard(TarotArcana::Devil);
        p.addSouls(500);
        p.deleteCharacter();
        assert(p.tarotCards.empty());
        assert(p.tarotFx.damageMult == 1.f);
        assert(p.tarotWeight() == 0);
        assert(p.souls == 0);
        assert(p.attrs.level() == 1);
    }

    std::printf("tarot test OK\n");
    return 0;
}
