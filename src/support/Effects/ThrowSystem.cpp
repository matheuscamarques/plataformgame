#include "ThrowSystem.h"

#include <cmath>
#include <string>

#include <algorithm>

#include "defines.h"
#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/GameContext.h"
#include "ParticleSystem.h"
#include "support/Progression/PatienceSystem.h"
#include "world/World.h"

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
            return;
        }

        // Sem fuse (spit): impacto no player + expira parado.
        // Dinamite sempre tem fuse > 0 até explodir: este ramo não a toca.
        if (t.kind == ThrowKind::Spit && ctx.player) {
            Player *pl = ctx.player;
            if (t.pos.x >= pl->getX() && t.pos.x <= pl->getX() + pl->getW() &&
                t.pos.y >= pl->getY() && t.pos.y <= pl->getY() + pl->getH()) {
                pl->hurt(t.damage);
                if (particles_) particles_->spawnHitSpark(t.pos);
                pool_.release(&t);
                return;
            }
        }
        if (t.fuse <= 0.f && t.resting) {
            if (particles_) particles_->spawnHitSpark(t.pos); // poof
            pool_.release(&t);
        }

        // Pepita parada perto de anão: vínculo (3 estágios no sistema).
        // Player ainda não joga pepita — mecanismo pronto, item pendente.
        if (t.kind == ThrowKind::GoldNugget && t.resting && ctx.enemies) {
            ctx.enemies->forEach([&](Enemy &s) {
                if (!s.ai || s.ai->kind() != core::EntityKind::Dwarf) return;
                const float dx = t.pos.x - s.body.getCenterX();
                const float dy = t.pos.y - s.body.getCenterY();
                if (dx * dx + dy * dy < 150.f * 150.f)
                    patienceOnNugget(s.patience);
            });
        }
    });
}

void ThrowSystem::handleTileCollision(Throwable &t, GameContext &ctx) {
    if (!ctx.world) return;

    const int tx = static_cast<int>(std::floor(t.pos.x / core::kBlockSize));
    const int ty = static_cast<int>(std::floor(t.pos.y / core::kBlockSize));
    if (!ctx.world->isSolid(tx, ty)) return;

    // Reverte ~1 frame e zera vertical, atrito na horizontal.
    // Spark só em impacto de verdade: sem gate, os 4 substeps gerariam
    // faísca todo tick enquanto a dinamite descansa no chão.
    const float impactVy = t.vel.y;
    t.pos.y -= t.vel.y * (1.f / 60.f);
    t.vel.y = 0.f;
    t.vel.x *= 0.6f;
    if (impactVy > 150.f && particles_) {
        particles_->spawnHitSpark(t.pos);
    }
}

void ThrowSystem::spawnBlast(sf::Vector2f center, float radius) {
    BlastVisual b;
    b.center = center;
    b.radius = radius;
    b.ttl = b.maxTtl;
    b.lightRadius = radius * 1.5f; // visual: 3× o dano de largura, como antes
    b.lightPeak = 1.0f;
    b.active = true;
    blasts_.push_back(b);
}

void ThrowSystem::tickBlasts(float dt) {
    for (auto& b : blasts_) {
        if (!b.active) continue;
        b.ttl -= dt;
        if (b.ttl <= 0.f) b.active = false;
    }
    blasts_.erase(
        std::remove_if(blasts_.begin(), blasts_.end(),
                       [](const BlastVisual& b){ return !b.active; }),
        blasts_.end());
}

void ThrowSystem::renderBlasts(sf::RenderTarget& target,
                                BlastGlowFn glow) {
    for (const auto& b : blasts_) {
        const float u = 1.f - (b.ttl / b.maxTtl);   // 0 → 1
        // Flash laranja via drawRadial injetado (origem/escala central).
        // Fade quadrático: forte no boom, some rápido.
        if (glow) {
            const float fade = (1.f - u) * (1.f - u);
            const auto a = static_cast<sf::Uint8>(255.f * fade * b.lightPeak);
            if (a >= 4)
                glow(b.center, b.lightRadius, sf::Color(255, 160, 60, a));
        }
        // Anel cresce de 20% a 100% do raio em 0.35s.
        const float r = b.radius * (0.2f + 0.8f * u);
        // Alpha cai conforme expande.
        const float a = (1.f - u) * 220.f;

        sf::CircleShape ring(r);
        ring.setOrigin(r, r);
        ring.setPosition(b.center);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineColor(sf::Color(255, 120, 40,
                                       static_cast<sf::Uint8>(a)));
        ring.setOutlineThickness(3.f);
        target.draw(ring);

        // Flash interno (branco-amarelo) nos primeiros 100ms.
        if (u < 0.3f) {
            const float innerA = (1.f - u / 0.3f) * 200.f;
            const float innerR = r * 0.5f;
            sf::CircleShape flash(innerR);
            flash.setOrigin(innerR, innerR);
            flash.setPosition(b.center);
            flash.setFillColor(sf::Color(255, 240, 180,
                                         static_cast<sf::Uint8>(innerA)));
            target.draw(flash);
        }
    }
}

void ThrowSystem::clearBlasts() {
    blasts_.clear();
}

std::size_t ThrowSystem::activeBlastCount() const {
    std::size_t n = 0;
    for (const auto& b : blasts_) if (b.active) ++n;
    return n;
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
        // SFX explosão com atenuação (ouvinte = player; sem ctx = sem custo).
        if (ctx.audio && ctx.player)
            ctx.audio->playAt(game::keyOf(game::Sfx::Explosion), t.pos,
                              {ctx.player->getCenterX(), ctx.player->getCenterY()});
    } else if (particles_) {
        // Sem ExplosionSystem: ao menos o flash visual.
        particles_->spawnHitSpark(t.pos);
    }
}

} // namespace support
