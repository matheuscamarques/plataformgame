/**
 * @file src/core/TarotCard.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Cartas de tarô: buff + debuff acumuláveis (fundação v1).
 * @details TarotArcana com 6 arcanos iniciais, TarotEffect só com campos que têm gancho vivo (damage, taken, attack/stamina, hpMax) e TarotRegistry com REGISTER_TAROT, incluído por Player e DeathSystem.
 */

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace core {

// Subconjunto inicial (6 de 22). Resto entra por dado, sem código.
enum class TarotArcana : uint8_t {
    Fool = 0,    // +dano / +dano recebido
    Devil,       // +25% dano / +25% recebido (alto risco)
    Star,        // +regen stamina / +custo stamina
    Tower,       // +vel ataque / +dano recebido
    Empress,     // +HP max / +custo stamina
    Strength,    // +dano / +custo stamina (força bruta cansa)
    COUNT
};

inline const char *tarotName(TarotArcana a) {
    switch (a) {
        case TarotArcana::Fool:     return "O Louco";
        case TarotArcana::Devil:    return "O Diabo";
        case TarotArcana::Star:     return "A Estrela";
        case TarotArcana::Tower:    return "A Torre";
        case TarotArcana::Empress:  return "A Imperatriz";
        case TarotArcana::Strength: return "A Força";
        default:                    return "?";
    }
}

// Efeito de UMA cópia. Só campos com gancho vivo no motor
// (computeModifiers/meleeDamage); resto entra quando o gancho existir.
struct TarotEffect {
    float damageMult = 1.f;        // meleeDamageBreakdown (novo gancho)
    float damageTakenMult = 1.f;   // hurt() via computeModifiers
    float attackSpeedMult = 1.f;   // updateMelee via computeModifiers
    float staminaRegenMult = 1.f;  // regen via computeModifiers
    float staminaCostMult = 1.f;   // custos via computeModifiers
    float hpMaxMult = 1.f;         // effectiveHpMax via computeModifiers
};

struct TarotCardDef {
    TarotArcana arcana = TarotArcana::Fool;
    std::string flavor; // 1 linha de lore p/ UI futura
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
