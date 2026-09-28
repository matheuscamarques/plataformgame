/**
 * @file src/core/TarotCards.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra as 78 cartas do tarô (dado, sem código).
 * @details Cada carta = buff + penalidade lineares por cópia; 6 fundadoras
 * intactas (Fool, Devil, Star, Tower, Empress, Strength), 16 Maiores novas
 * e 56 Menores por naipe/elemento, incluído no build por glob.
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

// Menores: 1 linha por carta (naipe Pravado no nome, efeito inline).
#define MINOR(arcana, flavor, ...) \
    REGISTER_TAROT(arcana, [] { \
        core::TarotEffect e; \
        __VA_ARGS__; \
        return card(arcana, flavor, e); \
    }())

} // namespace

// ---- 6 fundadoras (tabela canônica; teste trava valores) ----

REGISTER_TAROT(core::TarotArcana::Fool, [] {
    core::TarotEffect e;
    e.moveSpeedMult = 1.05f;
    e.damageMult = 1.05f;
    e.defenseMult = 0.95f;
    return card(core::TarotArcana::Fool,
                "Louco: corre e bate, guarda aberta.", e);
}())

REGISTER_TAROT(core::TarotArcana::Devil, [] {
    core::TarotEffect e;
    e.damageMult = 1.25f;
    e.damageTakenMult = 1.25f;
    e.spawnRateMult = 1.50f;
    e.enemyHpMult = 1.30f;
    return card(core::TarotArcana::Devil,
                "Pacto: +dano e +mundo (spawn e HP inimigo).", e);
}())

REGISTER_TAROT(core::TarotArcana::Star, [] {
    core::TarotEffect e;
    e.staminaRegenMult = 1.15f;
    e.attackSpeedMult = 1.10f;
    e.heavyDamageMult = 0.85f;
    return card(core::TarotArcana::Star,
                "Estrela: ritmo vivo, final murcho.", e);
}())

REGISTER_TAROT(core::TarotArcana::Tower, [] {
    core::TarotEffect e;
    e.critDamageMult = 1.30f;
    e.defenseMult = 0.80f;
    e.postureMaxMult = 0.90f;
    e.eliteChanceMult = 2.00f;
    return card(core::TarotArcana::Tower,
                "Queda: crítico brutal, corpo de vidro, elites.", e);
}())

REGISTER_TAROT(core::TarotArcana::Empress, [] {
    core::TarotEffect e;
    e.healingReceivedMult = 1.15f;
    e.hpMaxMult = 1.10f;
    e.attackSpeedMult = 0.90f;
    return card(core::TarotArcana::Empress,
                "Abundância: cura e corpo, golpe lento.", e);
}())

REGISTER_TAROT(core::TarotArcana::Strength, [] {
    core::TarotEffect e;
    e.damageMult = 1.10f;
    e.postureMaxMult = 1.10f;
    e.statusResistMult = 0.90f;
    return card(core::TarotArcana::Strength,
                "Força bruta: +dano, sangue fácil.", e);
}())

// ---- 16 Maiores restantes (Raro/Épico/Lendário por tierOf) ----

REGISTER_TAROT(core::TarotArcana::Magician, [] {
    core::TarotEffect e;
    e.magicDamageMult = 1.10f;
    e.physicalDamageMult = 0.90f;
    e.spellSlots = 1; // +1 slot a cada 2 cópias
    return card(core::TarotArcana::Magician,
                "Truque: +magia, -ferro, +slot (2x).", e);
}())

REGISTER_TAROT(core::TarotArcana::HighPriestess, [] {
    core::TarotEffect e;
    e.magicResistMult = 0.85f;
    e.fpRegenMult = 1.05f;
    e.physicalResistMult = 1.10f;
    return card(core::TarotArcana::HighPriestess,
                "Véu: resiste a magia, o ferro entra.", e);
}())

REGISTER_TAROT(core::TarotArcana::Emperor, [] {
    core::TarotEffect e;
    e.defenseMult = 1.15f;
    e.postureMaxMult = 1.10f;
    e.moveSpeedMult = 0.90f;
    return card(core::TarotArcana::Emperor,
                "Trono: muralha lenta.", e);
}())

REGISTER_TAROT(core::TarotArcana::Hierophant, [] {
    core::TarotEffect e;
    e.healingReceivedMult = 1.20f;
    e.critDamageMult = 0.90f;
    return card(core::TarotArcana::Hierophant,
                "Bênção: cura mais, crítico murcha.", e);
}())

REGISTER_TAROT(core::TarotArcana::Lovers, [] {
    core::TarotEffect e;
    e.highHpDamageMult = 1.10f;
    e.attackSpeedMult = 1.05f;
    e.lowHpDamageMult = 0.85f;
    return card(core::TarotArcana::Lovers,
                "Coração inteiro ou partido: forte em cima, fraco embaixo.", e);
}())

REGISTER_TAROT(core::TarotArcana::Chariot, [] {
    core::TarotEffect e;
    e.moveSpeedMult = 1.15f;
    e.chargeDamageMult = 1.10f;
    e.movingDefenseMult = 0.90f;
    return card(core::TarotArcana::Chariot,
                "Carga: corre e finaliza, nu em movimento.", e);
}())

REGISTER_TAROT(core::TarotArcana::Hermit, [] {
    core::TarotEffect e;
    e.aloneDamageMult = 1.20f;
    e.defenseMult = 0.85f;
    return card(core::TarotArcana::Hermit,
                "Sozinho: +dano contra um, nu contra muitos.", e);
}())

REGISTER_TAROT(core::TarotArcana::WheelOfFortune, [] {
    core::TarotEffect e;
    e.itemDropChanceMult = 1.10f;
    e.soulsGainMult = 1.05f;
    e.precisionMult = 0.95f;
    return card(core::TarotArcana::WheelOfFortune,
                "Giro: o destino dá, a mão roça.", e);
}())

REGISTER_TAROT(core::TarotArcana::Justice, [] {
    core::TarotEffect e;
    e.fullHpDamageMult = 1.15f;
    e.woundedTargetDamageMult = 0.85f;
    return card(core::TarotArcana::Justice,
                "Balança: esmaga o intacto, poupa o caído.", e);
}())

REGISTER_TAROT(core::TarotArcana::HangedMan, [] {
    core::TarotEffect e;
    e.damageConversionRate = 0.25f;
    e.defenseMult = 0.85f;
    return card(core::TarotArcana::HangedMan,
                "Suspenso: a dor vira lâmina por 10s.", e);
}())

REGISTER_TAROT(core::TarotArcana::Death, [] {
    core::TarotEffect e;
    e.killStackMax = 5;
    e.killStackBonus = 0.05f;
    e.hpMaxMult = 0.90f;
    return card(core::TarotArcana::Death,
                "Ceifa: cada morte alimenta, o corpo definha.", e);
}())

REGISTER_TAROT(core::TarotArcana::Temperance, [] {
    core::TarotEffect e;
    e.healingReceivedMult = 1.15f;
    e.attackSpeedMult = 0.90f;
    e.buffDurationMult = 0.90f;
    return card(core::TarotArcana::Temperance,
                "Medida: cura com calma, buff curto.", e);
}())

REGISTER_TAROT(core::TarotArcana::Moon, [] {
    core::TarotEffect e;
    e.magicDamageMult = 1.20f;
    e.nightSpawnMult = 2.00f;
    e.visionMult = 0.90f;
    e.fogMult = 2.00f;
    return card(core::TarotArcana::Moon,
                "Ilusão: magia e noite cheia, olhos turvos.", e);
}())

REGISTER_TAROT(core::TarotArcana::Sun, [] {
    core::TarotEffect e;
    e.hpMaxMult = 1.20f;
    e.healingReceivedMult = 1.10f;
    e.fireResistMult = 1.15f;
    e.lightMult = 1.50f;
    e.daySpawnMult = 1.50f;
    return card(core::TarotArcana::Sun,
                "Meio-dia: corpo solar, dia cheio.", e);
}())

REGISTER_TAROT(core::TarotArcana::Judgment, [] {
    core::TarotEffect e;
    e.reviveOnce = true;
    e.reviveHpPercent = 0.30f;
    e.maxRevives = 1;
    e.xpGainMult = 0.80f;
    return card(core::TarotArcana::Judgment,
                "Trombeta: nega a morte 1x, o além cobra XP.", e);
}())

REGISTER_TAROT(core::TarotArcana::World, [] {
    core::TarotEffect e;
    e.damageMult = 1.10f;
    e.magicDamageMult = 1.10f;
    e.physicalDamageMult = 1.10f;
    e.defenseMult = 1.10f;
    e.hpMaxMult = 1.10f;
    e.moveSpeedMult = 1.10f;
    e.attackSpeedMult = 1.10f;
    e.staminaRegenMult = 1.10f;
    e.fpRegenMult = 1.10f;
    e.critDamageMult = 1.10f;
    e.healingReceivedMult = 1.10f;
    e.fireResistMult = 1.10f;
    e.magicResistMult = 1.10f;
    e.physicalResistMult = 1.10f;
    e.statusResistMult = 0.90f;
    e.spawnRateMult = 1.50f;
    e.attrMult = 1.10f;
    e.bloodMoon = true;
    return card(core::TarotArcana::World,
                "Coroa: tudo +10%, véu e céu em sangue.", e);
}())

// ---- 56 Menores (Comum/Incomum por tierOf) ----
// Wands = Fogo (ação), Cups = Água (cura), Swords = Ar (crítico),
// Pentacles = Terra (souls/defesa). Toda carta: bônus + penalidade.
// Aproximações da tabela (sem sistema p/ o original):
// - WandsThree: alcance → crítico (sem stat de alcance);
// - CupsTwo: proximidade de aliado → cura plana (sem aliados).

// Wands
MINOR(core::TarotArcana::WandsAce, "Fagulha: +ataque, -corpo.",
      e.damageMult = 1.05f; e.hpMaxMult = 0.95f);
MINOR(core::TarotArcana::WandsTwo, "Plano: +velocidade de golpe, -guarda.",
      e.attackSpeedMult = 1.05f; e.defenseMult = 0.95f);
MINOR(core::TarotArcana::WandsThree, "Mira: +crítico, -passo.",
      e.critDamageMult = 1.05f; e.moveSpeedMult = 0.95f);
MINOR(core::TarotArcana::WandsFour, "Brasa mansa: +cura, -dano.",
      e.healingReceivedMult = 1.10f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::WandsFive, "Disputa: +crítico, -postura.",
      e.critDamageMult = 1.05f; e.postureMaxMult = 0.95f);
MINOR(core::TarotArcana::WandsSix, "Louros: +souls, -memória.",
      e.soulsGainMult = 1.10f; e.xpGainMult = 0.95f);
MINOR(core::TarotArcana::WandsSeven, "Muralha: +postura, -ritmo.",
      e.postureMaxMult = 1.05f; e.attackSpeedMult = 0.95f);
MINOR(core::TarotArcana::WandsEight, "Seta: +passo, -peso.",
      e.moveSpeedMult = 1.05f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::WandsNine, "Última guarda: +defesa ferido, -cura.",
      e.lowHpDefenseMult = 1.10f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::WandsTen, "Fardo: +dano, -passo.",
      e.damageMult = 1.10f; e.moveSpeedMult = 0.90f);
MINOR(core::TarotArcana::WandsPage, "Pajem: corre e morde, guarda aberta.",
      e.moveSpeedMult = 1.05f; e.damageMult = 1.05f; e.defenseMult = 0.95f);
MINOR(core::TarotArcana::WandsKnight, "Cavaleiro: investe, sangra fácil.",
      e.chargeDamageMult = 1.10f; e.statusResistMult = 0.95f);
MINOR(core::TarotArcana::WandsQueen, "Rainha: +dano, -cura.",
      e.damageMult = 1.10f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::WandsKing, "Rei: +dano e XP, -souls.",
      e.damageMult = 1.10f; e.xpGainMult = 1.10f; e.soulsGainMult = 0.90f);

// Cups
MINOR(core::TarotArcana::CupsAce, "Fonte: +cura, -dano.",
      e.healingReceivedMult = 1.10f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::CupsTwo, "Encontro: +cura, -dano.",
      e.healingReceivedMult = 1.05f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::CupsThree, "Brinde: +cura e souls, -dano.",
      e.healingReceivedMult = 1.05f; e.soulsGainMult = 1.05f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::CupsFour, "Maré mansa: +FP, -ritmo.",
      e.fpRegenMult = 1.10f; e.attackSpeedMult = 0.95f);
MINOR(core::TarotArcana::CupsFive, "Luto: +dano ferido, -cura.",
      e.lowHpDamageMult = 1.15f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::CupsSix, "Infância: +XP, -dano.",
      e.xpGainMult = 1.10f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::CupsSeven, "Miragem: +magia, -guarda.",
      e.magicDamageMult = 1.10f; e.defenseMult = 0.95f);
MINOR(core::TarotArcana::CupsEight, "Partida: +passo, -cura.",
      e.moveSpeedMult = 1.10f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::CupsNine, "Desejo: +souls, -sangue.",
      e.soulsGainMult = 1.10f; e.statusResistMult = 0.95f);
MINOR(core::TarotArcana::CupsTen, "Lar: +cura, -dano.",
      e.healingReceivedMult = 1.20f; e.damageMult = 0.90f);
MINOR(core::TarotArcana::CupsPage, "Pajem: +magia e FP, -ferro.",
      e.magicDamageMult = 1.05f; e.fpRegenMult = 1.05f; e.physicalDamageMult = 0.95f);
MINOR(core::TarotArcana::CupsKnight, "Cavaleiro: +cura e ritmo, -guarda.",
      e.healingReceivedMult = 1.10f; e.attackSpeedMult = 1.05f; e.defenseMult = 0.95f);
MINOR(core::TarotArcana::CupsQueen, "Rainha: +cura, -dano.",
      e.healingReceivedMult = 1.15f; e.damageMult = 0.90f);
MINOR(core::TarotArcana::CupsKing, "Rei: +cura e FP, -ferro.",
      e.healingReceivedMult = 1.10f; e.fpRegenMult = 1.10f; e.physicalDamageMult = 0.90f);

// Swords
MINOR(core::TarotArcana::SwordsAce, "Fio: +crítico, -cura.",
      e.critDamageMult = 1.10f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::SwordsTwo, "Impasse: +guarda e postura, -dano.",
      e.defenseMult = 1.05f; e.postureMaxMult = 1.05f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::SwordsThree, "Espinho: +dano ferido, -corpo.",
      e.lowHpDamageMult = 1.15f; e.hpMaxMult = 0.90f);
MINOR(core::TarotArcana::SwordsFour, "Repouso: +fôlego, -dano.",
      e.staminaRegenMult = 1.10f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::SwordsFive, "Derrota: +dano, -guarda.",
      e.damageMult = 1.10f; e.defenseMult = 0.90f);
MINOR(core::TarotArcana::SwordsSix, "Travessia: +passo, -postura.",
      e.moveSpeedMult = 1.10f; e.postureMaxMult = 0.95f);
MINOR(core::TarotArcana::SwordsSeven, "Estratagema: +crítico e souls, -XP.",
      e.critDamageMult = 1.10f; e.soulsGainMult = 1.05f; e.xpGainMult = 0.90f);
MINOR(core::TarotArcana::SwordsEight, "Cerca: +guarda, -passo.",
      e.defenseMult = 1.15f; e.moveSpeedMult = 0.90f);
MINOR(core::TarotArcana::SwordsNine, "Insônia: +dano ferido, -cura.",
      e.lowHpDamageMult = 1.10f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::SwordsTen, "Ruína: +crítico, -guarda.",
      e.critDamageMult = 1.20f; e.defenseMult = 0.80f);
MINOR(core::TarotArcana::SwordsPage, "Pajem: +crítico e XP, -cura.",
      e.critDamageMult = 1.05f; e.xpGainMult = 1.05f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::SwordsKnight, "Cavaleiro: +dano e ritmo, -guarda.",
      e.damageMult = 1.10f; e.attackSpeedMult = 1.05f; e.defenseMult = 0.95f);
MINOR(core::TarotArcana::SwordsQueen, "Rainha: +crítico, -cura.",
      e.critDamageMult = 1.10f; e.healingReceivedMult = 0.95f);
MINOR(core::TarotArcana::SwordsKing, "Rei: +dano e crítico, -cura.",
      e.damageMult = 1.10f; e.critDamageMult = 1.10f; e.healingReceivedMult = 0.90f);

// Pentacles
MINOR(core::TarotArcana::PentaclesAce, "Semente: +souls, -dano.",
      e.soulsGainMult = 1.10f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::PentaclesTwo, "Malabares: +guarda, -passo.",
      e.defenseMult = 1.10f; e.moveSpeedMult = 0.95f);
MINOR(core::TarotArcana::PentaclesThree, "Ofício: +XP, -dano.",
      e.xpGainMult = 1.10f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::PentaclesFour, "Cofre: +guarda, -passo.",
      e.defenseMult = 1.15f; e.moveSpeedMult = 0.90f);
MINOR(core::TarotArcana::PentaclesFive, "Fome: +dano ferido, -souls.",
      e.lowHpDamageMult = 1.10f; e.soulsGainMult = 0.95f);
MINOR(core::TarotArcana::PentaclesSix, "Esmola: +cura, -souls.",
      e.healingReceivedMult = 1.10f; e.soulsGainMult = 0.95f);
MINOR(core::TarotArcana::PentaclesSeven, "Colheita: +fôlego, -dano.",
      e.staminaRegenMult = 1.10f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::PentaclesEight, "Forja: +dano, -XP.",
      e.damageMult = 1.10f; e.xpGainMult = 0.95f);
MINOR(core::TarotArcana::PentaclesNine, "Pomares: +souls e guarda, -dano.",
      e.soulsGainMult = 1.10f; e.defenseMult = 1.05f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::PentaclesTen, "Legado: +souls, -dano.",
      e.soulsGainMult = 1.20f; e.damageMult = 0.90f);
MINOR(core::TarotArcana::PentaclesPage, "Pajem: +souls e XP, -dano.",
      e.soulsGainMult = 1.05f; e.xpGainMult = 1.05f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::PentaclesKnight, "Cavaleiro: +guarda e postura, -passo.",
      e.defenseMult = 1.10f; e.postureMaxMult = 1.05f; e.moveSpeedMult = 0.95f);
MINOR(core::TarotArcana::PentaclesQueen, "Rainha: +cura, -dano.",
      e.healingReceivedMult = 1.15f; e.damageMult = 0.95f);
MINOR(core::TarotArcana::PentaclesKing, "Rei: +souls e guarda, -dano.",
      e.soulsGainMult = 1.15f; e.defenseMult = 1.10f; e.damageMult = 0.90f);
