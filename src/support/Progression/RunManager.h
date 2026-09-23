/**
 * @file src/support/Progression/RunManager.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara estado de run com morte, pause e restart.
 * @details Define classe com flags dead e paused, tick e restart no checkpoint mais fundo via StratumManager, incluída por game.h e gateada antes dos sistemas.
 */

#pragma once

namespace support {

struct GameContext;
class StratumManager;

// Estado da run (morte, pause, restart). NÃO é core::System: o Game
// chama explicitamente antes do scheduler para gatear o tick.
// R vivo = respawn voluntário no checkpoint (desentalo/backtrack).
class RunManager {
public:
    bool isDead() const { return dead_; }
    bool isPaused() const { return paused_; }

    void setStratumManager(StratumManager *s) { stratum_ = s; }

    void tick(float dt, GameContext &ctx);

    // Respawn no checkpoint mais fundo (x atual). Sobe até achar ar
    // (fronteira é geralmente sólida). Limpa inimigos/pools, respawna
    // 2 slimes perto. Partículas não limpam (ctx não tem particles).
    void restart(GameContext &ctx);

private:
    bool dead_ = false;
    bool paused_ = false;
    StratumManager *stratum_ = nullptr;
};

} // namespace support
