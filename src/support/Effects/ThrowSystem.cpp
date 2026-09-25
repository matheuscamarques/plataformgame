/**
 * @file src/support/Effects/ThrowSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Simula arremessos, colisões, pavio e explosões.
 * @details Implementa tick com física, quique, repouso, luz da TNT no grid e chamada a ExplosionSystem e ParticleSystem, orquestrada pelo scheduler via GameContext.
 */

#include "ThrowSystem.h"

#include <cmath>
#include <string>

#include <algorithm>

#include "defines.h"
#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "physics/Physics2D.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/GameContext.h"
#include "ParticleSystem.h"
#include "support/Progression/PatienceSystem.h"
#include "world/Chunk.h"
#include "world/LightPropagator.h"
#include "world/World.h"
#include "core/VecSfml.h"
#include "core/Coords.h"

namespace support {

namespace {
constexpr float kRestThreshold = 4.f; // px/s — abaixo disso considera parado

// Nível de luz da TNT no grid: 6 com fuse cheio, 12 prestes a explodir.
// heat usa /0.8 (mesma base do tntGlowParams); fuse nasce em 1.0.
uint8_t tntLightLevel(float fuse) {
    const float heat = std::clamp(1.f - fuse / 0.8f, 0.f, 1.f);
    return static_cast<uint8_t>(6.f + 6.f * heat);
}

// floor-div p/ tiles possivelmente negativos (mundo pode ser < 0).
inline int chunkOfTile(int t) {
    return t >= 0 ? t / Chunk::W : -((-t + Chunk::W - 1) / Chunk::W);
}
}

Throwable *ThrowSystem::throwItem(core::Vec2f from, core::Vec2f vel, ThrowKind kind) {
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

        // Bolt teleguiado (kamehameha): persegue o inimigo mais
        // próximo no cone de 65° à frente (alcance 350px), curva máx
        // 8°/tick com velocidade preservada. Fora do cone voa reto
        // (legado intacto).
        if (t.kind == ThrowKind::Bolt && ctx.enemies) {
            const float speed = t.vel.length();
            if (speed > 1.f) {
                const core::Vec2f dir = t.vel / speed;
                float best = 350.f * 350.f;
                core::Vec2f aim{0.f, 0.f};
                bool found = false;
                ctx.enemies->forEach([&](Enemy &s) {
                    if (s.resources.isDead()) return;
                    const core::Vec2f to{s.body.getCenterX() - t.pos.x,
                                         s.body.getCenterY() - t.pos.y};
                    const float d2 = to.dot(to);
                    if (d2 > best || d2 <= 0.f) return;
                    if (dir.dot(to.normalized()) < 0.42f) return; // cone
                    best = d2;
                    aim = to;
                    found = true;
                });
                if (found) {
                    const float ang = std::atan2(dir.y, dir.x);
                    const float want = std::atan2(aim.y, aim.x);
                    float diff = want - ang;
                    while (diff > 3.14159265f) diff -= 2 * 3.14159265f;
                    while (diff < -3.14159265f) diff += 2 * 3.14159265f;
                    const float na =
                        ang + std::clamp(diff, -0.14f, 0.14f);
                    t.vel = {std::cos(na) * speed, std::sin(na) * speed};
                }
            }
            // Rastro: poeira a cada 3 ticks.
            if (particles_ && (++t.trailTick % 3 == 0))
                particles_->emitDust(t.pos, 1, 30.f);
        }

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

        // ── Bomba do player emite luz no grid enquanto o fuse corre ──
        // Sem raycast (fonte rápida, oclusão irrelevante); re-flood só se
        // tile ou nível mudar. addBlockSource preserva luz maior (player).
        if (ctx.world && isPlayerBomb(t.kind) && t.fuse > 0.f) {
            const core::TilePos ttp = core::worldToTile({t.pos.x, t.pos.y});
            const int tx = ttp.x;
            const int ty = ttp.y;
            const uint8_t lvl = tntLightLevel(t.fuse);
            if (tx != t.lastLightTileX || ty != t.lastLightTileY ||
                lvl != t.lastLightLevel) {
                const int ccx = chunkOfTile(tx);
                const int ccy = chunkOfTile(ty);
                // Trocou de chunk: limpa o resíduo no anterior.
                if ((t.lastChunkX != ccx || t.lastChunkY != ccy) &&
                    t.lastChunkX != -9999) {
                    if (Chunk* old = ctx.world->findChunk(t.lastChunkX, t.lastChunkY)) {
                        old->blockLight.assign(Chunk::W * Chunk::H, 0);
                        old->lightDirty = true;
                    }
                }
                if (Chunk* c = ctx.world->findChunk(ccx, ccy)) {
                    const int lx = tx - ccx * Chunk::W;
                    const int ly = ty - ccy * Chunk::H;
                    LightPropagator::addBlockSource(*c, lx, ly, lvl, false);
                    c->lightDirty = true;
                    t.lastLightTileX = tx;
                    t.lastLightTileY = ty;
                    t.lastLightLevel = lvl;
                    t.lastChunkX = ccx;
                    t.lastChunkY = ccy;
                }
            }
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
                pl->hurt(t.damage, t.damageType);
                if (particles_) particles_->spawnHitSpark(t.pos);
                pool_.release(&t);
                return;
            }
        }
        // Bolt (magia do player, F8b): espelho do spit contra inimigos.
        if (t.kind == ThrowKind::Bolt && ctx.enemies) {
            bool hit = false;
            ctx.enemies->forEach([&](Enemy &s) {
                if (hit || s.resources.isDead()) return;
                if (t.pos.x >= s.body.getX() &&
                    t.pos.x <= s.body.getX() + s.body.getW() &&
                    t.pos.y >= s.body.getY() &&
                    t.pos.y <= s.body.getY() + s.body.getH()) {
                    s.resources.takeDamage(t.damage, t.damageType);
                    if (particles_) particles_->spawnHitSpark(t.pos);
                    // Clarão kamehameha no impacto (só visual, sem dano).
                    spawnBlast(t.pos, 25.f);
                    hit = true;
                }
            });
            if (hit) {
                pool_.release(&t);
                return;
            }
        }
        // Fireball (Fase 3): no impacto explode Fire em área (sem dano
        // direto — o alvo entra na área). Sem tiles quebrados.
        if (t.kind == ThrowKind::Fireball && ctx.enemies) {
            bool hit = false;
            ctx.enemies->forEach([&](Enemy &s) {
                if (hit || s.resources.isDead()) return;
                if (t.pos.x >= s.body.getX() &&
                    t.pos.x <= s.body.getX() + s.body.getW() &&
                    t.pos.y >= s.body.getY() &&
                    t.pos.y <= s.body.getY() + s.body.getH()) {
                    hit = true;
                }
            });
            if (hit) {
                if (explosions_) {
                    ExplosionDef def;
                    def.radius = t.radius;
                    def.damage = t.damage;
                    def.postureDmg = t.postureDmg;
                    def.tilesRadius = 0;
                    def.knockback = t.knockback;
                    def.damageType = core::DamageType::Fire;
                    explosions_->explode(t.pos, def, ctx);
                }
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

    const core::TilePos ttp = core::worldToTile({t.pos.x, t.pos.y});
    const int tx = ttp.x;
    const int ty = ttp.y;
    // Query via backend (Fase 2): mesma pergunta ao World, pela interface.
    const physics::Physics2D phys(*ctx.world);
    if (!phys.isSolidTile(tx, ty)) return;

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

void ThrowSystem::spawnBlast(core::Vec2f center, float radius) {
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
        ring.setPosition(core::toSf(b.center));
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
            flash.setPosition(core::toSf(b.center));
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
    // A luz do grid some com o objeto (chunk zerado + dirty). Se o player
    // dividia o chunk, o guard do App re-adiciona no próximo tick.
    t.lastLightTileX = -9999;
    if (ctx.world && t.lastChunkX != -9999) {
        if (Chunk* c = ctx.world->findChunk(t.lastChunkX, t.lastChunkY)) {
            c->blockLight.assign(Chunk::W * Chunk::H, 0);
            c->lightDirty = true;
        }
        t.lastChunkX = -9999;
        t.lastChunkY = -9999;
    }
    if (explosions_) {
        ExplosionDef def;
        def.radius      = t.radius;
        def.damage      = t.damage;
        def.postureDmg  = t.postureDmg;
        def.tilesRadius = t.tilesRadius;
        def.knockback   = t.knockback;
        def.damageType  = t.damageType; // bomba elemental explode elemental
        explosions_->explode(t.pos, def, ctx);
        // SFX explosão com atenuação (ouvinte = player; sem ctx = sem custo).
        if (ctx.audio && ctx.player)
            ctx.audio->playAt(game::keyOf(game::Sfx::Explosion),
                              core::toSf(t.pos),
                              {ctx.player->getCenterX(), ctx.player->getCenterY()});
    } else if (particles_) {
        // Sem ExplosionSystem: ao menos o flash visual.
        particles_->spawnHitSpark(t.pos);
    }
}

} // namespace support
