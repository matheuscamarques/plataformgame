/**
 * @file src/support/Progression/DropSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Gera, atrai e coleta orbes de XP e itens.
 * @details Implementa spawnXP e spawnItem com física, magnetismo, coleta por proximidade, cores por raridade e sons, tickado pelo scheduler e renderizado em mundo via DropSystem.h.
 */

#include "DropSystem.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

#include "core/ItemDef.h"
#include "core/Time.h"
#include "core/VecSfml.h"
#include "core/Random.h"
#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "support/GameContext.h"

namespace support {

namespace {
// Cor do retângulo por raridade (mesmo idioma das partículas: cor = dado).
sf::Color rarityColor(core::ItemRarity r) {
    using core::ItemRarity;
    switch (r) {
        case ItemRarity::Common:    return {200, 200, 200};
        case ItemRarity::Uncommon:  return {100, 220, 120};
        case ItemRarity::Rare:      return {100, 160, 255};
        case ItemRarity::Epic:      return {190, 110, 255};
        case ItemRarity::Legendary: return {255, 170, 60};
    }
    return {200, 200, 200};
}
} // namespace

XPOrb *DropSystem::spawnXP(core::Vec2f pos, int value) {
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
    core::Vec2f playerPos{0.f, 0.f};
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
                // Souls (F1): XP coletado credita a carteira do player.
                ctx.player->souls += o.value;
                // SFX coleta (sem ctx.audio em teste = mudo).
                if (ctx.audio)
                    ctx.audio->play(game::keyOf(game::Sfx::XpCollect), 0.6f);
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

    // Orbes de item: mesmo magnet do XP + pickupDelay (não suga no spawn)
    // + depósito no inventário do player (sobra fica no chão).
    itemPool_.forEachActive([&](ItemOrb &o) {
        if (!o.active) return;

        o.lifetime -= dt;
        if (o.lifetime <= 0.f) {
            o.active = false;
            itemPool_.release(&o);
            return;
        }
        o.pickupDelay -= dt;

        if (hasPlayer && o.pickupDelay <= 0.f) {
            const float dx = playerPos.x - o.pos.x;
            const float dy = playerPos.y - o.pos.y;
            const float d2 = dx * dx + dy * dy;

            if (d2 < kCollectRadius * kCollectRadius) {
                const int leftover =
                    ctx.player->inventory.add(o.item);
                if (leftover <= 0) {
                    if (ctx.audio)
                        ctx.audio->play(game::keyOf(game::Sfx::XpCollect), 0.6f);
                    o.active = false;
                    itemPool_.release(&o);
                    return;
                }
                // Cheio: para de magnetizar, fica no chão p/ depois.
                o.item.quantity = static_cast<uint16_t>(leftover);
                o.magnetized = false;
            } else {
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
                o.pos += o.vel * dt;
            }
        } else {
            o.vel.y += kGravity * dt;
            o.pos += o.vel * dt;
        }
    });
}

void DropSystem::render(sf::RenderTarget &target) {
    sf::CircleShape c(3.f);
    c.setOrigin(3.f, 3.f);
    c.setFillColor({100, 220, 120});
    pool_.forEachActive([&](const XPOrb &o) {
        c.setPosition(core::toSf(o.pos));
        target.draw(c);
    });
    sf::RectangleShape r({7.f, 7.f});
    itemPool_.forEachActive([&](const ItemOrb &o) {
        const core::ItemDef* def = o.item.def();
        const auto col = def ? rarityColor(def->rarity)
                             : sf::Color(200, 200, 200);
        r.setFillColor(col);
        r.setOutlineColor(sf::Color::Black);
        r.setOutlineThickness(1.f);
        // Idle bob visual (física intacta: pos real não muda).
        const float bob =
            std::sin(core::Time::elapsed() * 2.f + o.pos.x * 0.1f) * 2.f;
        r.setPosition(o.pos.x - 3.5f, o.pos.y - 3.5f + bob);
        target.draw(r);
    });
}

ItemOrb *DropSystem::spawnItem(const std::string& defId, int qty,
                               core::Vec2f pos) {
    const core::ItemDef* def = core::ItemRegistry::instance().find(defId);
    if (!def || qty <= 0) return nullptr;
    auto *o = itemPool_.acquire();
    if (!o) return nullptr;
    *o = ItemOrb{};
    o->item.defId = defId;
    o->item.quantity = static_cast<uint16_t>(
        qty > 0xFFFF ? 0xFFFF : qty);
    o->pos = pos;
    o->vel = {core::randRange(-70.f, 70.f), core::randRange(-190.f, -110.f)};
    o->active = true;
    o->lifetime = o->maxLifetime;
    o->pickupDelay = 0.5f;
    return o;
}

} // namespace support
