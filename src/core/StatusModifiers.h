#pragma once

namespace core {

// Modificadores comportamentais de status (review F7+): struct puro,
// sem dependências de Player ou SFML. Player::computeModifiers agrega
// a partir dos status ativos; o tick consome nos pontos-chave.
// Poison (DS1 fiel): só dano, todos os mults em 1.0.
// Bleed: micro-slow no proc (0.3s); sem ele, tudo identidade.
struct StatusModifiers {
    float moveSpeedMult    = 1.0f; // futuro: physics::step (outro dono)
    float staminaRegenMult = 1.0f; // F7+: regen de estamina
    float staminaCostMult  = 1.0f; // F7+: custo swing/sprint
    float attackSpeedMult  = 1.0f; // Frost ativo: duração do swing
    float damageTakenMult  = 1.0f; // F7+: hurt()
    float hpMaxMult        = 1.0f; // futuro: Curse (effectiveHpMax)
    bool  canRoll          = true; // futuro: sem rolagem sob efeito
    bool  canAttack        = true; // futuro: travado sob efeito
    bool  blocksHealing    = false; // futuro: Curse bloqueia cura
};

} // namespace core
