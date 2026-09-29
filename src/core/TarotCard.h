/**
 * @file src/core/TarotCard.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Cartas de tarô: 78 arcanos (22 Maiores + 56 Menores), fado forçado.
 * @details TarotArcana com os 78 arcanos em ordem fixa (índice determinístico),
 * TarotTier com peso do destino, TarotEffect só com campos de efeito agregado
 * (cada um com gancho vivo no Player, exceto postureMaxMult — sem poise no
 * player ainda, agregado e testado p/ quando o gancho existir) e
 * TarotRegistry com REGISTER_TAROT. Puro core: sem support, sem RNG global
 * (sorteio via salt em DeathSystem); incluído por Player e DeathSystem.
 */

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace core {

// 22 Maiores (0-21) + 56 Menores (22-77). Ordem fixa: índice estável p/
// sorteio determinístico por salt. Menores: Wands=Fogo, Cups=Água,
// Swords=Ar, Pentacles=Terra; cada naipe: Ás, 2-10, Pajem, Cavaleiro,
// Rainha, Rei (14).
enum class TarotArcana : uint8_t {
    // ---- 22 Arcanos Maiores ----
    Fool = 0, Magician, HighPriestess, Empress, Emperor,
    Hierophant, Lovers, Chariot, Strength, Hermit,
    WheelOfFortune, Justice, HangedMan, Death, Temperance,
    Devil, Tower, Star, Moon, Sun, Judgment, World,
    // ---- Wands (Paus/Fogo) ----
    WandsAce, WandsTwo, WandsThree, WandsFour, WandsFive,
    WandsSix, WandsSeven, WandsEight, WandsNine, WandsTen,
    WandsPage, WandsKnight, WandsQueen, WandsKing,
    // ---- Cups (Copas/Água) ----
    CupsAce, CupsTwo, CupsThree, CupsFour, CupsFive,
    CupsSix, CupsSeven, CupsEight, CupsNine, CupsTen,
    CupsPage, CupsKnight, CupsQueen, CupsKing,
    // ---- Swords (Espadas/Ar) ----
    SwordsAce, SwordsTwo, SwordsThree, SwordsFour, SwordsFive,
    SwordsSix, SwordsSeven, SwordsEight, SwordsNine, SwordsTen,
    SwordsPage, SwordsKnight, SwordsQueen, SwordsKing,
    // ---- Pentacles (Ouros/Terra) ----
    PentaclesAce, PentaclesTwo, PentaclesThree, PentaclesFour,
    PentaclesFive, PentaclesSix, PentaclesSeven, PentaclesEight,
    PentaclesNine, PentaclesTen, PentaclesPage, PentaclesKnight,
    PentaclesQueen, PentaclesKing,
    COUNT
};

inline const char *tarotName(TarotArcana a) {
    static constexpr const char *kNames[] = {
        "O Louco", "O Mago", "A Sacerdotisa", "A Imperatriz", "O Imperador",
        "O Hierofante", "Os Amantes", "O Carro", "A Força", "O Eremita",
        "A Roda da Fortuna", "A Justiça", "O Enforcado", "A Morte",
        "A Temperança", "O Diabo", "A Torre", "A Estrela", "A Lua", "O Sol",
        "O Julgamento", "O Mundo",
        "Ás de Paus", "2 de Paus", "3 de Paus", "4 de Paus", "5 de Paus",
        "6 de Paus", "7 de Paus", "8 de Paus", "9 de Paus", "10 de Paus",
        "Pajem de Paus", "Cavaleiro de Paus", "Rainha de Paus", "Rei de Paus",
        "Ás de Copas", "2 de Copas", "3 de Copas", "4 de Copas", "5 de Copas",
        "6 de Copas", "7 de Copas", "8 de Copas", "9 de Copas", "10 de Copas",
        "Pajem de Copas", "Cavaleiro de Copas", "Rainha de Copas",
        "Rei de Copas",
        "Ás de Espadas", "2 de Espadas", "3 de Espadas", "4 de Espadas",
        "5 de Espadas", "6 de Espadas", "7 de Espadas", "8 de Espadas",
        "9 de Espadas", "10 de Espadas", "Pajem de Espadas",
        "Cavaleiro de Espadas", "Rainha de Espadas", "Rei de Espadas",
        "Ás de Ouros", "2 de Ouros", "3 de Ouros", "4 de Ouros", "5 de Ouros",
        "6 de Ouros", "7 de Ouros", "8 de Ouros", "9 de Ouros", "10 de Ouros",
        "Pajem de Ouros", "Cavaleiro de Ouros", "Rainha de Ouros",
        "Rei de Ouros",
    };
    static_assert(sizeof(kNames) / sizeof(kNames[0]) ==
                      static_cast<std::size_t>(TarotArcana::COUNT),
                  "tarotName: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(a);
    if (i < 0 || i >= static_cast<int>(TarotArcana::COUNT)) return "?";
    return kNames[i];
}

// Raridade do fado: dirige peso do destino e chance de sorteio.
// Lendário 4 (Devil, Tower, World, Judgment), Épico 8, Raro 10
// (restam os Maiores), Incomum 16 (Ás + corte dos naipes),
// Comum 40 (resto dos Menores). 4+8+10+16+40 = 78.
enum class TarotTier : uint8_t {
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary,
    COUNT
};

inline constexpr int kTarotTierCount = static_cast<int>(TarotTier::COUNT);

inline TarotTier tierOf(TarotArcana a) {
    switch (a) {
        case TarotArcana::Devil:
        case TarotArcana::Tower:
        case TarotArcana::World:
        case TarotArcana::Judgment:
            return TarotTier::Legendary;
        case TarotArcana::Magician:
        case TarotArcana::Emperor:
        case TarotArcana::Chariot:
        case TarotArcana::Death:
        case TarotArcana::HangedMan:
        case TarotArcana::Moon:
        case TarotArcana::WheelOfFortune:
        case TarotArcana::Justice:
            return TarotTier::Epic;
        case TarotArcana::Fool:
        case TarotArcana::HighPriestess:
        case TarotArcana::Empress:
        case TarotArcana::Hierophant:
        case TarotArcana::Lovers:
        case TarotArcana::Strength:
        case TarotArcana::Hermit:
        case TarotArcana::Temperance:
        case TarotArcana::Star:
        case TarotArcana::Sun:
            return TarotTier::Rare;
        default:
            break;
    }
    // Menores: Ás (0) + corte (11-13) = Incomum; resto = Comum.
    const int idx = static_cast<int>(a) -
                    static_cast<int>(TarotArcana::WandsAce);
    const int pos = idx % 14;
    if (pos == 0 || pos >= 11) return TarotTier::Uncommon;
    return TarotTier::Common;
}

// Peso do destino por tier: 1/2/5/10/25. Sem escolha, sem descarte:
// o peso só zera apagando o personagem (deleteCharacter).
inline int weightOf(TarotTier t) {
    static constexpr int kWeights[] = {1, 2, 5, 10, 25}; // Common..Legendary
    static_assert(sizeof(kWeights) / sizeof(kWeights[0]) ==
                      static_cast<std::size_t>(kTarotTierCount),
                  "weightOf: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(t);
    if (i < 0 || i >= kTarotTierCount) return 0;
    return kWeights[i];
}

// Chance base de fado por morte (1%). Elite (variante nv4+) ×5;
// sem boss no jogo ainda (×50 reservado p/ quando existir).
inline constexpr float kTarotBaseChance = 1.01f;

// Sorteio de tier por u∈[0,1): Comum 70 / Incomum 20 / Raro 7 /
// Épico 2.5 / Lendário 0.5. Puro (chamador deriva u do salt).
inline TarotTier rollTier(float u) {
    if (u < 0.70f) return TarotTier::Common;
    if (u < 0.90f) return TarotTier::Uncommon;
    if (u < 0.97f) return TarotTier::Rare;
    if (u < 0.995f) return TarotTier::Epic;
    return TarotTier::Legendary;
}

// Efeito de UMA cópia. Semântica: mults >1 = bônus, <1 = penalidade,
// exceto *ResistMult e damageTakenMult onde >1 = apanha mais.
// defenseMult divide o dano recebido (1.15 = -13% recebido).
// postureMaxMult agregado sem consumidor (sem poise no player).
struct TarotEffect {
    // Ofensivos
    float damageMult = 1.f;          // meleeDamageBreakdown
    float magicDamageMult = 1.f;     // melee mágico (frost_weapon/staff)
    float physicalDamageMult = 1.f;  // melee físico
    float critDamageMult = 1.f;      // crítico do 10º swing (×1.5 base)
    float attackSpeedMult = 1.f;     // updateMelee via computeModifiers
    // Defensivos
    float hpMaxMult = 1.f;           // effectiveHpMax via computeModifiers
    float defenseMult = 1.f;         // divisor em hurt()
    float postureMaxMult = 1.f;      // reservado (sem poise no player)
    float statusResistMult = 1.f;    // limiar em statusThreshold()
    float healingReceivedMult = 1.f; // poção + Heal
    // Mobilidade e recursos
    float moveSpeedMult = 1.f;       // physics::Input.speedMult
    float staminaRegenMult = 1.f;    // regen via computeModifiers
    float staminaCostMult = 1.f;     // custos via computeModifiers
    float fpRegenMult = 1.f;         // regen de FP no tick
    // Economia (souls = XP neste jogo)
    float soulsGainMult = 1.f;       // coleta de orbe (DropSystem)
    float xpGainMult = 1.f;          // consumíveis de alma (Lost/Great)
    float itemDropChanceMult = 1.f;  // chance do próximo fado
    // Recebido (inversos: >1 = apanha mais)
    float damageTakenMult = 1.f;     // hurt() via computeModifiers
    float fireResistMult = 1.f;      // computeResistances (Fogo)
    float magicResistMult = 1.f;     // computeResistances (Lightning)
    float physicalResistMult = 1.f;  // computeResistances (Físico)
    // Condicionais de dano
    float lowHpDamageMult = 1.f;     // HP do player < 50%
    float highHpDamageMult = 1.f;    // HP do player > 50%
    float fullHpDamageMult = 1.f;    // alvo com HP cheio (meleeDamageVs)
    float woundedTargetDamageMult = 1.f; // alvo com HP < 25% (Vs)
    float aloneDamageMult = 1.f;     // 1 inimigo vivo (meleeDamageVs)
    float heavyDamageMult = 1.f;     // 3º golpe do combo (Estrela)
    float chargeDamageMult = 1.f;    // finalizador ≈ investida (Carro)
    float precisionMult = 1.f;       // <1 = graze periódico (Roda)
    // Condicionais de defesa (hurt)
    float lowHpDefenseMult = 1.f;    // próprio HP < 25% (9 de Paus)
    float movingDefenseMult = 1.f;   // em movimento (Carro)
    // Magia e atributos
    int spellSlots = 0;              // +N slots a cada 2 cópias (Mago)
    float buffDurationMult = 1.f;    // duração de buffs (Temperança)
    float attrMult = 1.f;            // Mundo: entradas de scaling/vitais
    // Especiais
    bool reviveOnce = false;         // Julgamento: nega a morte
    float reviveHpPercent = 0.f;     // fração do HP máx no revive (carta dá)
    int maxRevives = 0;              // acumula por cópia
    float damageConversionRate = 0.f;// Enforcado: fração do recebido vira bônus
    int killStackMax = 0;            // Morte: teto de stacks
    float killStackBonus = 0.f;      // Morte: +dano por stack
    // Mundo (escopo Cósmico/Híbrido: SpawnSystem/Factory/Lighting leem)
    float spawnRateMult = 1.f;       // orçamento de spawn (Diabo/Mundo)
    float daySpawnMult = 1.f;        // Sol: orçamento de dia
    float nightSpawnMult = 1.f;      // Lua: orçamento de noite
    float enemyHpMult = 1.f;         // Factory: HP dos nascidos (Diabo)
    float eliteChanceMult = 1.f;     // Factory: +1 variante (Torre)
    float lightMult = 1.f;           // raio de luz (Sol)
    float visionMult = 1.f;          // raio de luz (Lua, penalidade)
    float fogMult = 1.f;             // divisor do luar (Lua, penalidade)
    bool bloodMoon = false;          // Mundo: toda noite é de sangue
};

struct TarotCardDef {
    TarotArcana arcana = TarotArcana::Fool;
    std::string flavor; // 1 linha de lore p/ vinheta/HUD
    TarotEffect effect;
};

class TarotRegistry {
public:
    struct ArcanaHash {
        std::size_t operator()(TarotArcana a) const noexcept {
            return static_cast<std::size_t>(a);
        }
    };

    static TarotRegistry &instance() {
        static TarotRegistry r;
        return r;
    }

    void add(TarotArcana a, TarotCardDef d) {
        if (!has(a)) keys_.push_back(a);
        items_[a] = std::move(d);
    }

    bool has(TarotArcana a) const { return items_.count(a) > 0; }

    const TarotCardDef *find(TarotArcana a) const {
        auto it = items_.find(a);
        return it == items_.end() ? nullptr : &it->second;
    }

    const std::vector<TarotArcana> &keys() const { return keys_; }

private:
    std::unordered_map<TarotArcana, TarotCardDef, ArcanaHash> items_;
    std::vector<TarotArcana> keys_;
};

#define _REG_TAROT_CONCAT(a, b) a##b
#define _REG_TAROT_TYPE(l) _REG_TAROT_CONCAT(AutoRegTarot_, l)
#define _REG_TAROT_INST(l) _REG_TAROT_CONCAT(instTarot_, l)
#define REGISTER_TAROT(arcana, ...)                                       \
    namespace {                                                           \
    struct _REG_TAROT_TYPE(__LINE__) {                                    \
        _REG_TAROT_TYPE(__LINE__)() {                                     \
            ::core::TarotRegistry::instance().add(arcana, __VA_ARGS__);   \
        }                                                                 \
    };                                                                    \
    static _REG_TAROT_TYPE(__LINE__) _REG_TAROT_INST(__LINE__);            \
    }

} // namespace core
