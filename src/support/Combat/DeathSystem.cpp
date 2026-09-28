/**
 * @file src/support/Combat/DeathSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Rola drops do arquétipo e remove inimigos mortos com recompensas.
 * @details Implementa tick que usa ArchetypeRegistry e rollDrops mais removeDead, chamado pelo loop com DropSystem e ParticleSystem injetados.
 */

#include "DeathSystem.h"

#include <vector>

#include "core/Config.h"
#include "core/DropTable.h"
#include "core/TarotCard.h"
#include "entities/Player/Player.h"
#include "support/Debug/DebugFeed.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Progression/DropSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Effects/ParticleSystem.h"
#include "world/World.h"
#include "core/Vec.h"
#include "core/Coords.h"

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
            const core::TilePos dtp = core::worldToTile({e.body.getCenterX(), e.body.getCenterY()});
            const int tx = dtp.x;
            const int ty = dtp.y;
            for (const auto& [id, qty] :
                 core::rollDrops(arch->drops, e.variantLevel,
                                 core::dropSalt(tx, ty, seed))) {
                drops_->spawnItem(id, qty,
                                  {e.body.getCenterX(), e.body.getCenterY()});
            }
            // Equipamento vestido cai (100%: o que veste, dropa).
            e.equipment.forEach([&](core::EquipSlot, const core::Item& it) {
                if (it.isEmpty()) return;
                drops_->spawnItem(it.defId, it.quantity,
                                  {e.body.getCenterX(), e.body.getCenterY()});
            });
            // Tarô (fado forçado, 78 arcanos): chance determinística por
            // tile+seed; elite (variante nv4+, aura/flama) ×5; carta
            // sorteada por tier dentro do salt (sem pickup, sem escolha).
            // Aplica direto + vinheta + log; Morte credita killstack.
            if (ctx.player) {
                const uint32_t salt = core::dropSalt(tx, ty, seed);
                const float eliteMult =
                    (e.variantLevel >= 4) ? 5.f : 1.f;
                const float chance = core::kTarotBaseChance * eliteMult *
                    ctx.player->tarotFx.itemDropChanceMult;
                if ((salt % 10000u) <
                    static_cast<uint32_t>(chance * 10000.f)) {
                    const float u =
                        ((salt >> 8) % 1000u) / 1000.f;
                    const core::TarotTier tier = core::rollTier(u);
                    std::vector<core::TarotArcana> pool;
                    for (int i = 0;
                         i < static_cast<int>(core::TarotArcana::COUNT);
                         ++i) {
                        const auto a =
                            static_cast<core::TarotArcana>(i);
                        if (core::tierOf(a) == tier) pool.push_back(a);
                    }
                    if (!pool.empty()) {
                        const auto arcana =
                            pool[(salt >> 16) % pool.size()];
                        ctx.player->addTarotCard(arcana);
                        ctx.player->showTarotReveal(arcana);
                        if (ctx.debug)
                            ctx.debug->pushLog(
                                std::string("tarot ") +
                                core::tarotName(arcana) + " (peso " +
                                std::to_string(
                                    ctx.player->tarotWeight()) +
                                ")");
                    }
                }
            }
        });
    }
    ctx.enemies->removeDead([&](Enemy &e) {
        const core::Vec2f pos{e.body.getCenterX(), e.body.getCenterY()};
        if (particles_) particles_->spawnTileBreak(pos, 0, 0, 0);
        // Morte: cada abate alimenta o killstack do player (janela 30s).
        if (ctx.player) ctx.player->addKillStack();
        if (!drops_) return;
        // XP por arquétipo (slime 100, anão 150); sem archetype = 1.
        int xp = 1;
        if (!e.archetypeId.empty()) {
            if (const EnemyArchetype *arch =
                    ArchetypeRegistry::instance().find(e.archetypeId))
                xp = arch->xp;
        }
        drops_->spawnXP(pos, xp);
    }, &ctx);
}

} // namespace support
