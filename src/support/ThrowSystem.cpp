#include "ThrowSystem.h"

#include <cmath>

#include "../defines.h"
#include "ExplosionSystem.h"
#include "GameContext.h"
#include "ParticleSystem.h"
#include "World/World.h"

namespace support {

namespace {
constexpr float kRestThreshold = 4.f; // px/s — abaixo disso considera parado
}

Throwable *ThrowSystem::throwItem(sf::Vector2f from, sf::Vector2f vel, ThrowKind kind) {
    auto *t = pool_.acquire();
    if (!t) return nullptr;
    *t = Throwable{};
    t->pos    = from;
    t->vel    = vel;
    t->kind   = kind;
    t->active = true;
    return t;
}

void ThrowSystem::tick(float dt, GameContext &ctx) {
    pool_.forEachActive([&](Throwable &t) {
        if (!t.active) return;

        // Gravidade + integração
        t.vel.y += t.gravity * dt;

        // Integração em passos pequenos para não atravessar tile
        const int steps = 4;
        for (int s = 0; s < steps; ++s) {
            t.pos += t.vel * (dt / steps);
            handleTileCollision(t, ctx);
        }

        // Considera "resting" quando velocidade baixa por tempo
        if (t.vel.x * t.vel.x + t.vel.y * t.vel.y < kRestThreshold * kRestThreshold) {
            t.restingTimer += dt;
            if (t.restingTimer > 0.1f) t.resting = true;
        } else {
            t.restingTimer = 0.f;
            t.resting = false;
        }

        // Fuse
        if (t.fuse > 0.f) {
            t.fuse -= dt;
            if (t.fuse <= 0.f) {
                t.fuse = 0.f;
                handleFuse(t, ctx);
                pool_.release(&t); // seguro: release dentro do forEach
            }
        }
    });
}

void ThrowSystem::handleTileCollision(Throwable &t, GameContext &ctx) {
    if (!ctx.world) return;

    const int tx = static_cast<int>(std::floor(t.pos.x / BLOCK_SIZE));
    const int ty = static_cast<int>(std::floor(t.pos.y / BLOCK_SIZE));
    if (!ctx.world->isSolid(tx, ty)) return;

    // Reverte ~1 frame e zera vertical, atrito na horizontal.
    t.pos.y -= t.vel.y * (1.f / 60.f);
    t.vel.y = 0.f;
    t.vel.x *= 0.6f;
}

void ThrowSystem::handleFuse(Throwable &t, GameContext &ctx) {
    if (explosions_) {
        ExplosionDef def;
        def.radius      = t.radius;
        def.damage      = t.damage;
        def.postureDmg  = t.postureDmg;
        def.tilesRadius = t.tilesRadius;
        def.knockback   = t.knockback;
        explosions_->explode(t.pos, def, ctx);
    } else if (particles_) {
        // Sem ExplosionSystem: ao menos o flash visual.
        particles_->spawnHitSpark(t.pos);
    }
}

} // namespace support
