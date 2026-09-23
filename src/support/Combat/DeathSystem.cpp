/**
 * @file src/support/Combat/DeathSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Rola drops do arquétipo e remove inimigos mortos com recompensas.
 * @details Implementa tick que usa ArchetypeRegistry e rollDrops mais removeDead, chamado pelo loop com DropSystem e ParticleSystem injetados.
 */

#include "DeathSystem.h"

#include "core/Config.h"
#include "core/DropTable.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Progression/DropSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Effects/ParticleSystem.h"
#include "world/World.h"

namespace support {

void DeathSystem::tick(float /*dt*/, GameContext &ctx) {
    if (!ctx.enemies) return;
    // Seguro: EnemySystem (150), BodySystem (250) e ExplosionSystem (320)
    // já rodaram; nada depois de 330 itera slimes no tick.
    // Drops primeiro (fase 2): rola a tabela do archetype por inimigo
    // morto; salt = (tile, seed) — determinístico por mundo.
    if (drops_) {
        const uint32_t seed = ctx.world ? ctx.world->getSeed() : 0u;
        ctx.enemies->forEach([&](Enemy &e) {
            if (!e.resources.isDead()) return;
            if (e.archetypeId.empty()) return;
            const EnemyArchetype *arch =
                ArchetypeRegistry::instance().find(e.archetypeId);
            if (!arch || arch->drops.entries.empty()) return;
            const int tx = static_cast<int>(e.body.getCenterX() / core::kBlockSize);
            const int ty = static_cast<int>(e.body.getCenterY() / core::kBlockSize);
            for (const auto& [id, qty] :
                 core::rollDrops(arch->drops, e.variantLevel,
                                 core::dropSalt(tx, ty, seed))) {
                drops_->spawnItem(id, qty,
                                  {e.body.getCenterX(), e.body.getCenterY()});
            }
        });
    }
    ctx.enemies->removeDead([&](sf::Vector2f pos) {
        if (particles_) particles_->spawnTileBreak(pos, 0, 0, 0);
        if (drops_)     drops_->spawnXP(pos, 1);
    }, &ctx);
}

} // namespace support
