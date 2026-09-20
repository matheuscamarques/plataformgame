#include "DropSystem.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

#include "core/Random.h"
#include "entities/player/player.h"
#include "support/GameContext.h"

namespace support {

XPOrb *DropSystem::spawnXP(sf::Vector2f pos, int value) {
    auto *o = pool_.acquire();
    if (!o) return nullptr;
    *o = XPOrb{};
    o->pos        = pos;
    o->vel        = {core::randRange(-60.f, 60.f), core::randRange(-180.f, -100.f)};
    o->value      = value;
    o->active     = true;
    o->lifetime   = o->maxLifetime;
    return o;
}

void DropSystem::tick(float dt, GameContext &ctx) {
    // Player pode ser nulo (testes de expiração): envelhece igual,
    // só pula magnetismo/coleta.
    sf::Vector2f playerPos{0.f, 0.f};
    const bool hasPlayer = (ctx.player != nullptr);
    if (hasPlayer) {
        playerPos = {ctx.player->getCenterX(), ctx.player->getCenterY()};
    }

    pool_.forEachActive([&](XPOrb &o) {
        if (!o.active) return;

        o.lifetime -= dt;
        if (o.lifetime <= 0.f) {
            o.active = false;
            pool_.release(&o);
            return;
        }

        if (hasPlayer) {
            const float dx = playerPos.x - o.pos.x;
            const float dy = playerPos.y - o.pos.y;
            const float d2 = dx * dx + dy * dy;

            if (d2 < kCollectRadius * kCollectRadius) {
                collected_ += o.value;
                o.active = false;
                pool_.release(&o);
                return;
            }

            if (d2 < kMagnetRadius * kMagnetRadius) o.magnetized = true;
            if (o.magnetized) {
                const float d = std::sqrt(d2);
                if (d > 1.f) {
                    o.vel.x = (dx / d) * kMagnetSpeed;
                    o.vel.y = (dy / d) * kMagnetSpeed;
                }
            } else {
                o.vel.y += kGravity * dt;
            }
        } else {
            o.vel.y += kGravity * dt;
        }

        o.pos += o.vel * dt;
    });
}

void DropSystem::render(sf::RenderTarget &target) {
    sf::CircleShape c(3.f);
    c.setOrigin(3.f, 3.f);
    c.setFillColor({100, 220, 120});
    pool_.forEachActive([&](const XPOrb &o) {
        c.setPosition(o.pos);
        target.draw(c);
    });
}

} // namespace support
