#include "ParticleSystem.h"

#include <algorithm>
#include <cmath>

#include "core/Random.h"
#include "defines.h"
#include "world/World.h"

namespace support {

namespace {

sf::Color colorForBlock(int kind) {
    switch (kind) {
        case 0:  return {120, 120, 120}; // Stone
        case 1:  return {110,  80,  50}; // Dirt
        case 2:  return {200, 170,  70}; // OreGold
        case 3:  return {180, 180, 190}; // OreIron
        case 4:  return {150, 220, 240}; // OreCrystal
        default: return {160, 160, 160};
    }
}

} // namespace

void ParticleSystem::tick(float dt, GameContext &ctx) {
    debris_.forEachActive([&](Particle &p) {
        p.lifetime -= dt;
        if (p.lifetime <= 0.f) {
            debris_.release(&p);
            return;
        }
        p.vel.y += p.gravity * dt;
        p.pos += p.vel * dt;

        if (p.collides && ctx.world) {
            int tx = static_cast<int>(p.pos.x / BLOCK_SIZE);
            int ty = static_cast<int>(p.pos.y / BLOCK_SIZE);
            if (ctx.world->isSolid(tx, ty)) {
                p.vel = {0.f, 0.f};
                p.gravity = 0.f;
                p.lifetime = std::min(p.lifetime, 0.4f);
            }
        }
    });

    dust_.forEachActive([&](Particle &p) {
        p.lifetime -= dt;
        if (p.lifetime <= 0.f) {
            dust_.release(&p);
            return;
        }
        p.vel *= (1.f - 2.f * dt);
        p.pos += p.vel * dt;
    });
}

void ParticleSystem::render(sf::RenderTarget &target) {
    // Retângulo colorido. Sprite vem depois.
    sf::RectangleShape rs;
    auto draw = [&](const Particle &p) {
        rs.setSize({p.size.x, p.size.y});
        rs.setPosition(p.pos);
        rs.setFillColor(p.color);
        target.draw(rs);
    };
    debris_.forEachActive(draw);
    dust_.forEachActive(draw);
}

void ParticleSystem::spawnTileBreak(sf::Vector2f center,
                                    int primaryKind,
                                    int secondaryKind,
                                    uint8_t oreMask) {
    int oreCount = 0;
    for (uint8_t m = oreMask; m; m >>= 1) oreCount += (m & 1);

    emitDebris(center, primaryKind, 6 + (oreCount > 0 ? 2 : 0), 120.f);

    if (oreCount > 0 && secondaryKind > 0) {
        emitDebris(center, secondaryKind, oreCount * 2, 160.f);
    }

    emitDust(center, 10 + oreCount * 2, 180.f);
}

void ParticleSystem::spawnHitSpark(sf::Vector2f point) {
    for (int i = 0; i < 4; ++i) {
        auto *p = dust_.acquire();
        if (!p) return;
        p->active = true;
        const float a = core::randRange(0.f, 6.2831853f);
        const float s = core::randRange(40.f, 100.f);
        p->pos = point;
        p->vel = {std::cos(a) * s, std::sin(a) * s};
        p->size = {1.f, 1.f};
        p->color = {255, 220, 120, 220};
        p->kind = ParticleKind::Spark;
        p->maxLifetime = core::randRange(0.1f, 0.25f);
        p->lifetime = p->maxLifetime;
        p->gravity = 0.f;
        p->collides = false;
    }
}

void ParticleSystem::emitDebris(sf::Vector2f origin, int kind, int count, float spread) {
    const sf::Color col = colorForBlock(kind);
    for (int i = 0; i < count; ++i) {
        auto *p = debris_.acquire();
        if (!p) return;
        p->active = true;
        const float a = core::randRange(0.f, 6.2831853f);
        const float s = core::randRange(40.f, spread);
        p->pos = origin;
        p->vel = {std::cos(a) * s, std::sin(a) * s - 60.f};
        p->size = {2.f, 2.f};
        p->color = col;
        p->kind = ParticleKind::StoneDebris;
        p->maxLifetime = core::randRange(0.6f, 1.6f);
        p->lifetime = p->maxLifetime;
        p->gravity = 480.f;
        p->collides = true;
    }
}

void ParticleSystem::emitDust(sf::Vector2f origin, int count, float spread) {
    for (int i = 0; i < count; ++i) {
        auto *p = dust_.acquire();
        if (!p) return;
        p->active = true;
        const float a = core::randRange(0.f, 6.2831853f);
        const float s = core::randRange(20.f, spread);
        p->pos = origin;
        p->vel = {std::cos(a) * s, std::sin(a) * s - 30.f};
        p->size = {1.f, 1.f};
        p->color = {180, 170, 150, 200};
        p->kind = ParticleKind::Dust;
        p->maxLifetime = core::randRange(0.2f, 0.6f);
        p->lifetime = p->maxLifetime;
        p->gravity = 20.f;
        p->collides = false;
    }
}

} // namespace support
