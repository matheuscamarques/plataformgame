/**
 * @file src/support/Enemies/DwarfAI.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara estados, config e hooks da IA do anão básico.
 * @details Define enum DwarfState e classe DwarfAI com onTick e onTakeHit, usada por EnemySystem e criada via BehaviorRegistry.
 */

#pragma once

#include <cstdint>
#include <string>

#include "core/Cooldown.h"
#include "entities/Entity.hpp"
#include "Barks.h"
#include "Behavior.h"

namespace support {

// Anão básico: patrulha a home, alerta, joga dinamite, picaretada de
// perto, recua quando o player sai. Sem interrupt-on-hit (Behavior não
// tem hook de dano — Recover 0.6s é a janela de punish). Sem skills,
// sem paciência, sem variante: isso é Anão Elite.
// Estado mora aqui (como SlimeAI guarda dir_); corpo/recursos no Enemy.
enum class DwarfState : uint8_t {
    Patrol,
    Alert,
    ThrowWindup,
    ThrowRelease,
    Recover,
    Melee,
    Retreat
};

class DwarfAI : public Behavior {
public:
    struct Config {
        int hp = 60;
        float aggroRange = 200.f; // px
        float homeRadius = 96.f;  // px
        // Alcances moram nas skills (dwarf_dynamite/dwarf_melee),
        // não aqui — UtilityAI decide por range da skill.
    };

    DwarfAI() = default;
    explicit DwarfAI(Config cfg) : cfg_(cfg) {}

    const char *name() const override { return "DwarfAI"; }
    core::EntityKind kind() const override { return core::EntityKind::Dwarf; }
    void onTick(Enemy &e, float dt, GameContext &ctx) override;
    void onTakeHit(Enemy &e, int applied, GameContext &ctx) override;

    // Observabilidade de teste (não API de gameplay).
    DwarfState state() const { return state_; }
    // Raio de aggro p/ overlay de debug (círculo F5). Só leitura.
    float aggroRange() const { return cfg_.aggroRange; }

private:
    Config cfg_;
    DwarfState state_ = DwarfState::Patrol;

    sf::Vector2f home_{0.f, 0.f};
    bool homed_ = false;
    int patrolDir_ = 1;

    core::Cooldown stateTimer_;
    // Skill escolhida no Recover, executada no fim do windup.
    // Cooldown/custo moram no SkillSystem (skillCds), não aqui.
    std::string pendingSkill_;
    int lastWarning_ = 0; // último aviso de paciência com bark

    void changeState(DwarfState s, float duration = 0.f);
    void tickPatrol(Enemy &e, float dt, GameContext &ctx);
    void tickCombat(Enemy &e, float dt, GameContext &ctx);
    void emitBark(Enemy &e, BarkId id);
};

} // namespace support
