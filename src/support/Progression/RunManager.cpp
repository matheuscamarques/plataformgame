/**
 * @file src/support/Progression/RunManager.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Controla pause, morte e respawn da run atual.
 * @details Implementa tick que alterna pause e detecta morte mais restart que reposiciona no checkpoint e limpa inimigos e pools, chamada pelo Game antes do scheduler via RunManager.h.
 */

#include "RunManager.h"

#include <algorithm>
#include <cmath>

#include "defines.h"
#include "entities/Player/Player.h"
#include "support/Progression/DropSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Input/InputMap.h"
#include "StratumManager.h"
#include "support/Effects/ThrowSystem.h"
#include "world/Stratum.h"
#include "world/World.h"

namespace support {

void RunManager::tick(float /*dt*/, GameContext &ctx) {
    if (ctx.input && ctx.input->pressed(Action::Pause) && !dead_)
        paused_ = !paused_;
    if (ctx.input && ctx.input->pressed(Action::Restart))
        restart(ctx);
    if (!dead_ && ctx.player && ctx.player->hp <= 0) {
        dead_ = true;
        // Mancha de souls (F1): carteira cai no cadáver em orbe.
        // Morrer de novo com mancha ativa perde a antiga (DS).
        stainSouls_ = ctx.player->souls;
        if (stainSouls_ > 0) {
            if (ctx.drops)
                ctx.drops->spawnXP({ctx.player->getCenterX(),
                                    ctx.player->getCenterY()},
                                   stainSouls_);
            ctx.player->souls = 0;
        }
    }
}

void RunManager::restart(GameContext &ctx) {
    Player *p = ctx.player;
    if (!p) return;

    float x = p->getX();
    float y = stratum_ ? stratum_->respawnPoint(x).y : 0.f;
    if (ctx.world) {
        int tx = static_cast<int>(std::floor(p->getCenterX() / core::kBlockSize));
        int ty = static_cast<int>(std::floor(y / core::kBlockSize));
        int guard = 0;
        // 2 tiles livres (corpo 50px): cabeça e pés fora da rocha.
        while (guard++ < 400 &&
               (ctx.world->isSolid(tx, ty) || ctx.world->isSolid(tx, ty - 1)))
            ty--;
        y = static_cast<float>(ty) * core::kBlockSize;
    }
    p->respawn(x, y);

    if (ctx.enemies) {
        ctx.enemies->clear();
        ctx.enemies->spawn("slime", x - 300.f, y);
        ctx.enemies->spawn("slime", x + 300.f, y);
    }
    if (ctx.throws) ctx.throws->clear();
    if (ctx.drops) ctx.drops->clear();
    // Mancha move p/ o respawn com conservação exata: desconta da
    // carteira (recuperou no pé + R = zera e renasce no spawn) e
    // re-spawna. R vivo não mexe na carteira (generoso).
    if (stainSouls_ > 0) {
        p->souls = std::max(0, p->souls - stainSouls_);
        if (ctx.drops)
            ctx.drops->spawnXP({p->getCenterX(), p->getCenterY()},
                               stainSouls_);
        stainSouls_ = 0;
    }
    // Partículas: fora do ctx, somem sozinhas (lifetime curto).

    dead_ = false;
    paused_ = false;
}

} // namespace support
