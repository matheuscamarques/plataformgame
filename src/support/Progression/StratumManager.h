/**
 * @file src/support/Progression/StratumManager.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara progresso vertical com checkpoints por estrato.
 * @details Define classe System com prioridades, deepest, current, máscara e respawnPoint no checkpoint, incluída por game.h e RunManager para respawn e HUD.
 */

#pragma once
#include "core/System.h"
#include <SFML/System/Vector2.hpp>
#include <cstdint>

namespace support {

struct GameContext;

// Modelo C (híbrido): entrar num estrato novo desbloqueia o checkpoint;
// morte respawna no checkpoint mais fundo (fiação da morte vem no B).
// Priority 60: só lê a posição do player, sem física nem combate.
// Sem save em disco ainda: checkpoints vivem na run (B persiste).
class StratumManager : public core::System {
public:
    const char *name() const override { return "StratumManager"; }
    int priority() const override { return 60; }

    void tick(float /*dt*/, GameContext &ctx) override;

    int deepest() const { return deepest_; }
    // Estrato onde o player ESTÁ (sobe e desce; p/ música/ HUD).
    // deepest() é o máximo alcançado (checkpoint); current() é posição.
    int current() const { return current_; }
    uint32_t unlockedMask() const { return unlocked_; }
    bool unlocked(int s) const { return (unlocked_ >> s) & 1u; }

    // Ponto de respawn: x atual do player, y do checkpoint mais fundo.
    // Sala real do checkpoint (altar) é futura; B refina o x/y exato.
    sf::Vector2f respawnPoint(float x) const;

private:
    uint32_t unlocked_ = 1u; // estrato 0 nasce desbloqueado
    int deepest_ = 0;
    int current_ = 0;
};

} // namespace support
