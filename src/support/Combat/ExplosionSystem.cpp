/**
 * @file src/support/Combat/ExplosionSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Executa explosão com quebra de tiles, dano em área e screen shake.
 * @details Implementa explode mais breakTiles e applyToTarget, chamado por dinamite, barril e skills via GameContext e ExplosionTarget.
 */

#include "ExplosionSystem.h"

#include <cmath>
#include <string>

#include "defines.h"
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "support/Camera/Camera.h"
#include "support/Debug/DebugFeed.h"
#include "support/Enemies/EnemyResources.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Effects/ParticleSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Progression/DropSystem.h"
#include "support/Progression/PatienceSystem.h"
#include "world/Block.h"
#include "world/World.h"
#include "core/Coords.h"

namespace support {

namespace {
float distSq(core::Vec2f a, core::Vec2f b) {
    const float dx = a.x - b.x, dy = a.y - b.y;
    return dx * dx + dy * dy;
}

// Tempo que a IA ignora vel (knockback visível antes do snap).
constexpr float kKnockbackLockTime = 0.2f;
} // namespace

int ExplosionSystem::explode(core::Vec2f center, const ExplosionDef &def, GameContext &ctx) {
    // 1. Quebra tiles
    breakTilesInCircle(center, def.tilesRadius, ctx);

    // 2. Flash agregado (o detalhe por tile sai no passo 1)
    if (particles_) {
        particles_->spawnHitSpark(center);
    }

    // 3. Dano em alvos — caller passa targets via ctx.explosionTargets
    int hit = 0;
    if (ctx.explosionTargets) {
        for (const auto &t : *ctx.explosionTargets) {
            if (applyToTarget(t, center, def)) ++hit;
        }
    }
    // Visual: anel até o raio real (ThrowSystem desenha; sem ctx = sem custo).
    if (ctx.throws) ctx.throws->spawnBlast(center, def.radius);

    // Screen shake pelo player (item 23): 0.4 colado, 0 além de 200px.
    if (ctx.camera && ctx.player) {
        const float dx = ctx.player->getCenterX() - center.x;
        const float dy = ctx.player->getCenterY() - center.y;
        const float d2 = dx * dx + dy * dy;
        if (d2 < 200.f * 200.f)
            ctx.camera->addTrauma(0.4f * (1.f - d2 / (200.f * 200.f)));
    }

    if (ctx.debug)
        ctx.debug->pushLog("explode " + std::to_string(def.damage) +
                           " hit" + std::to_string(hit));
    return hit;
}

bool ExplosionSystem::applyToTarget(const ExplosionTarget &t,
                                    core::Vec2f center,
                                    const ExplosionDef &def) {
    const float r2 = def.radius * def.radius;
    if (!t.body) return false;

    // Narrowphase por parte: melhor multiplicador tocado, não soma.
    // Whiff (nenhuma parte no raio) = sem hit.
    const PartDef *bestDef = nullptr;
    float bestMult = 0.f;
    bool  inRange  = false;

    t.body->forEach([&](const PartState &st, const PartDef &pd) {
        if (st.fromSchema) return; // parte oculta: não sofre dano de área
        const core::Vec2f pc{
            st.worldBox.left + st.worldBox.width  * 0.5f,
            st.worldBox.top  + st.worldBox.height * 0.5f
        };
        if (distSq(pc, center) > r2) return;
        inRange = true;
        if (pd.damageMult > bestMult) {
            bestMult = pd.damageMult;
            bestDef  = &pd;
        }
    });

    if (!inRange) return false;

    const int   dmg     = static_cast<int>(def.damage * (bestDef ? bestDef->damageMult : 1.f));
    const float postDmg = def.postureDmg * (bestDef ? bestDef->postureMult : 1.f);

    if (t.resources) {
        t.resources->takeDamage(dmg, def.damageType);
        t.resources->damagePosture(postDmg);
    }
    // Player não tem EnemyResources: HP próprio entra quando existir.
    // Por enquanto, conta como atingido sem aplicar dano.

    // Knockback: impulso radial com falloff + pop up. Sem lock, a IA
    // (setVx todo tick) apagaria o vx no frame seguinte — lock primeiro.
    if (t.knockbackLock) t.knockbackLock->trigger(kKnockbackLockTime);
    if (t.mover) {
        const float dist = std::sqrt(distSq(t.center, center));
        const float k = std::max(0.f, 1.f - dist / def.radius);

        core::Vec2f dir = t.center - center;
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 1.f) { dir.x /= len; dir.y /= len; }
        else           { dir = {0.f, -1.f}; } // centro exato → pop up

        t.mover->setVx(t.mover->getVx() + dir.x * def.knockback * k);
        t.mover->setVy(t.mover->getVy() + dir.y * def.knockback * k * 0.5f
                       - 80.f * k);
    }

    return true;
}

void ExplosionSystem::breakTilesInCircle(core::Vec2f center, int tilesRadius, GameContext &ctx) {
    if (!ctx.world) return;

    const core::TilePos ctp = core::worldToTile({center.x, center.y});
    const int cx = ctp.x;
    const int cy = ctp.y;
    const float r2 = static_cast<float>(tilesRadius * tilesRadius);

    // Paciência: 1 call por explosão (não por tile — senão 1 blast =
    // traição instantânea). Minério pesa mais que pedra.
    bool brokeOre = false;
    bool brokeAny = false;
    for (int ty = cy - tilesRadius; ty <= cy + tilesRadius; ++ty) {
        for (int tx = cx - tilesRadius; tx <= cx + tilesRadius; ++tx) {
            const int dx = tx - cx;
            const int dy = ty - cy;
            if (dx * dx + dy * dy > r2) continue;

            Tile broken = Tile::Air;
            if (ctx.world->breakTile(tx, ty, &broken) && particles_) {
                const core::Vec2f tileCenter{
                    (tx + 0.5f) * core::kBlockSize,
                    (ty + 0.5f) * core::kBlockSize
                };
                // Cor do tile quebrado (primário = tipo real).
                particles_->spawnTileBreak(tileCenter, static_cast<int>(broken), 0, 0);
            }
            if (broken == Tile::Air) continue; // nada quebrou aqui
            brokeAny = true;
            if (isOreTile(broken)) brokeOre = true;
            // Drop do bloco (fase 2): dropId vazio = sem drop.
            if (drops_) {
                const BlockDef& bd = support::blockDef(broken);
                if (bd.dropId && bd.dropId[0] != '\0')
                    drops_->spawnItem(bd.dropId, 1,
                                      {(tx + 0.5f) * core::kBlockSize,
                                       (ty + 0.5f) * core::kBlockSize});
            }
        }
    }
    // mineração → paciência: só anão que já reconheceu liga (sistema
    // filtra estágio/distância). Pá própria (dig/collapse) não passa
    // por aqui: usam World::breakTile direto.
    if (brokeAny && ctx.enemies) {
        ctx.enemies->forEach([&](Enemy &s) {
            if (!s.ai || s.ai->kind() != core::EntityKind::Dwarf) return;
            const float dx = center.x - s.body.getCenterX();
            const float dy = center.y - s.body.getCenterY();
            const int w0 = s.patience.warningLevel;
            const bool b0 = s.patience.betrayed;
            patienceOnMine(s.patience, brokeOre, std::sqrt(dx * dx + dy * dy));
            // SFX warnings/traição (transição; sem ctx.audio em teste = mudo).
            if (ctx.audio) {
                if (!b0 && s.patience.betrayed)
                    ctx.audio->play(game::keyOf(game::Sfx::DwarfBetray));
                else if (s.patience.warningLevel > w0)
                    ctx.audio->play(game::keyOf(static_cast<game::Sfx>(
                        static_cast<int>(game::Sfx::DwarfWarn1)
                        + s.patience.warningLevel - 1)));
            }
        });
    }
}

} // namespace support
