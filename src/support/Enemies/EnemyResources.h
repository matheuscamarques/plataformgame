/**
 * @file src/support/Enemies/EnemyResources.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara HP, mana, stamina, postura e stagger com regras por tier.
 * @details Expõe struct Cost e EnemyResources com tick e takeDamage, incluído por Enemy, Factory e SkillSystem.
 */

#pragma once
#include "core/Cooldown.h"
#include "core/Resistances.h"
#include <algorithm>

namespace support {

// Custo de uma skill. Trash ignora mana/stamina, mas postura sempre conta.
struct Cost {
    float mana    = 0.f;
    float stamina = 0.f;
    float posture = 0.f;
};

// 4 recursos de um inimigo. Não vive no Player (S2 é só inimigo).
// Trash: só HP + Postura. Elite/Boss: os 4.
struct EnemyResources {
    // --- HP ---
    int hp    = 0;
    int hpMax = 0;

    // --- Mana ---
    float mana      = 0.f;
    float manaMax   = 0.f;
    float manaRegen = 1.0f;
    core::Cooldown manaRegenDelay{2.0f};

    // --- Stamina ---
    float stamina      = 0.f;
    float staminaMax   = 0.f;
    float staminaRegen = 20.f;
    core::Cooldown staminaRegenDelay{0.8f};

    // --- Postura ---
    float posture      = 0.f;
    float postureMax   = 0.f;
    float postureRegen = 15.f;
    core::Cooldown postureRegenDelay{1.2f};

    // --- Stagger ---
    float staggerDuration = 0.8f;
    float staggerTimer    = 0.f;

    // --- Tier ---
    // Trash ignora mana e stamina em canPay/pay/regen.
    bool isTrash = false;

    // Resistências por tipo (Fase 1 elementais; default 1.0).
    // Copiado do arquétipo na Factory.
    core::Resistances resistances;

    // --- API ---
    bool canPay(const Cost& c) const;
    void pay(const Cost& c);
    void tick(float dt);

    bool staggered() const { return staggerTimer > 0.f; }
    bool isDead()    const { return hp <= 0; }

    // Retorna dano efetivamente aplicado (0 se já morto ou amount <= 0).
    // Protege duplo hit no mesmo frame de re-trigger.
    // Tipo filtra pela resistência antes de aplicar.
    int takeDamage(int amount,
                   core::DamageType type = core::DamageType::Physical);

    // Reduz postura e atualiza stagger. Use isso, não mexa em .posture direto
    // se quiser stagger automático.
    void damagePosture(float amount);

private:
    void checkStagger();
};

} // namespace support
