/**
 * @file src/core/TarotCards.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra as 6 cartas iniciais (dado, sem código).
 * @details Cada carta = buff + debuff lineares por cópia; efeitos restritos a ganchos vivos, incluído no build por glob.
 */

#include "core/TarotCard.h"

namespace {

core::TarotCardDef card(core::TarotArcana a, const char *flavor,
                        core::TarotEffect e) {
    core::TarotCardDef d;
    d.arcana = a;
    d.flavor = flavor;
    d.effect = e;
    return d;
}

} // namespace

REGISTER_TAROT(core::TarotArcana::Fool, [] {
    core::TarotEffect e;
    e.damageMult = 1.05f;
    e.damageTakenMult = 1.05f;
    return card(core::TarotArcana::Fool,
                "Inocência ousada: bate e apanha 5% a mais.", e);
}())

REGISTER_TAROT(core::TarotArcana::Devil, [] {
    core::TarotEffect e;
    e.damageMult = 1.25f;
    e.damageTakenMult = 1.25f;
    return card(core::TarotArcana::Devil,
                "Pacto: +25% dano causado e recebido.", e);
}())

REGISTER_TAROT(core::TarotArcana::Star, [] {
    core::TarotEffect e;
    e.staminaRegenMult = 1.15f;
    e.staminaCostMult = 1.10f;
    return card(core::TarotArcana::Star,
                "Fôlego vivo: regenera mais, gasta mais.", e);
}())

REGISTER_TAROT(core::TarotArcana::Tower, [] {
    core::TarotEffect e;
    e.attackSpeedMult = 1.10f;
    e.damageTakenMult = 1.10f;
    return card(core::TarotArcana::Tower,
                "Fúria na queda: golpeia mais rápido, apanha mais.", e);
}())

REGISTER_TAROT(core::TarotArcana::Empress, [] {
    core::TarotEffect e;
    e.hpMaxMult = 1.10f;
    e.staminaCostMult = 1.10f;
    return card(core::TarotArcana::Empress,
                "Corpo abundante, braço pesado: +HP, +custo.", e);
}())

REGISTER_TAROT(core::TarotArcana::Strength, [] {
    core::TarotEffect e;
    e.damageMult = 1.10f;
    e.staminaCostMult = 1.10f;
    return card(core::TarotArcana::Strength,
                "Força bruta: +dano, swings custam mais.", e);
}())
