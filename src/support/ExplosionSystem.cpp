#include "ExplosionSystem.h"

#include <cmath>
#include <string>

#include "../defines.h"
#include "../entities/entity/entity.hpp"
#include "EnemyResources.h"
#include "EnemySystem.h"
#include "GameContext.h"
#include "ParticleSystem.h"
#include "PatienceSystem.h"
#include "World/World.h"

namespace support {

namespace {
float distSq(sf::Vector2f a, sf::Vector2f b) {
    const float dx = a.x - b.x, dy = a.y - b.y;
    return dx * dx + dy * dy;
}

// Tempo que a IA ignora vel (knockback visível antes do snap).
constexpr float kKnockbackLockTime = 0.2f;
} // namespace

int ExplosionSystem::explode(sf::Vector2f center, const ExplosionDef &def, GameContext &ctx) {
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
    return hit;
}

bool ExplosionSystem::applyToTarget(const ExplosionTarget &t,
                                    sf::Vector2f center,
                                    const ExplosionDef &def) {
    const float r2 = def.radius * def.radius;
    if (!t.body) return false;

    // Narrowphase por parte: melhor multiplicador tocado, não soma.
    // Whiff (nenhuma parte no raio) = sem hit.
    const PartDef *bestDef = nullptr;
    float bestMult = 0.f;
    bool  inRange  = false;

    t.body->forEach([&](const PartState &st, const PartDef &pd) {
        const sf::Vector2f pc{
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
        t.resources->takeDamage(dmg);
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

        sf::Vector2f dir = t.center - center;
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 1.f) { dir.x /= len; dir.y /= len; }
        else           { dir = {0.f, -1.f}; } // centro exato → pop up

        t.mover->setVx(t.mover->getVx() + dir.x * def.knockback * k);
        t.mover->setVy(t.mover->getVy() + dir.y * def.knockback * k * 0.5f
                       - 80.f * k);
    }

    return true;
}

void ExplosionSystem::breakTilesInCircle(sf::Vector2f center, int tilesRadius, GameContext &ctx) {
    if (!ctx.world) return;

    const int cx = static_cast<int>(std::floor(center.x / BLOCK_SIZE));
    const int cy = static_cast<int>(std::floor(center.y / BLOCK_SIZE));
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
                const sf::Vector2f tileCenter{
                    (tx + 0.5f) * BLOCK_SIZE,
                    (ty + 0.5f) * BLOCK_SIZE
                };
                // Cor do tile quebrado (primário = tipo real).
                particles_->spawnTileBreak(tileCenter, static_cast<int>(broken), 0, 0);
            }
            if (broken == Tile::Air) continue; // nada quebrou aqui
            brokeAny = true;
            if (isOreTile(broken)) brokeOre = true;
        }
    }
    // mineração → paciência: só anão que já reconheceu liga (sistema
    // filtra estágio/distância). Pá própria (dig/collapse) não passa
    // por aqui: usam World::breakTile direto.
    if (brokeAny && ctx.enemies) {
        ctx.enemies->forEach([&](Enemy &s) {
            if (!s.ai || std::string(s.ai->name()) != "DwarfAI") return;
            const float dx = center.x - s.body.getCenterX();
            const float dy = center.y - s.body.getCenterY();
            patienceOnMine(s.patience, brokeOre, std::sqrt(dx * dx + dy * dy));
        });
    }
}

} // namespace support
