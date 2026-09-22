#include "BodySystem.h"

#include "assets/SpriteFrameRegistry.h"
#include "support/Combat/WeaponRegistry.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "entities/Player/Player.h"

namespace support {

namespace {
// Bbox real da arma equipada (espelho exato do drawPlayerWeapon:
// mesma mão, mesma origem, mesma escala). Fonte única de verdade
// para hitbox (MeleeSystem) e desenho.
sf::FloatRect computeWeaponBbox(Player &p) {
    const PartState *arm = p.body.find(BodyPartId::ArmR);
    if (!arm) return {0.f, 0.f, 0.f, 0.f};
    const float s = p.getH() / 20.f; // kPlayerH
    const float handX = arm->worldBox.left + arm->worldBox.width * 0.5f;
    const float handY = arm->worldBox.top + arm->worldBox.height;
    // Offsets por arma via registry (mesma matemática de antes).
    float handOX = 4.f, handOY = 8.f;
    if (const core::ItemDef* wdef = p.weaponDef()) {
        if (const auto *wd0 = WeaponRegistry::instance().find(wdef->id)) {
            handOX = wd0->handOffsetX;
            handOY = wd0->handOffsetY;
        }
    }
    const float baseX = handX + handOX * static_cast<float>(p.facing) * s;
    const float baseY = handY + handOY * s;
    // Só Active/Recovery estendem; resto, arma recolhida (nulo).
    if (p.meleePhase != MeleePhase::Active &&
        p.meleePhase != MeleePhase::Recovery)
        return {baseX, baseY, 0.f, 0.f};
    // Dimensões por arma via registry (sem if por id).
    float sw = 16.f * s, sh = 8.f * s;
    float ox = 5.f * s, oy = 5.f * s;
    if (const core::ItemDef* wdef = p.weaponDef()) {
        if (const auto *wd = WeaponRegistry::instance().find(wdef->id)) {
            sw = wd->spriteW * s;
            sh = wd->spriteH * s;
            ox = wd->originX * s;
            oy = wd->originY * s;
        }
    }
    float left, top;
    if (p.facing >= 0) {
        left = baseX - ox;
        top = baseY - oy;
    } else {
        left = baseX - (sw - ox);
        top = baseY - oy;
    }
    return {left, top, sw, sh};
}

} // namespace

void BodySystem::tick(float /*dt*/, GameContext &ctx) {
    if (ctx.player) {
        Player &p = *ctx.player;
        const auto f = assets::frameData(p.currentFrameId);
        if (f.rows) {
            p.body.rebuildFromSprite(
                {p.getX(), p.getY()}, {p.getW(), p.getH()},
                f.rows, f.w, f.h, f.pal, f.palCount,
                p.facing);
        } else {
            p.body.rebuild({p.getX(), p.getY()}, p.facing);
        }
        // A: sobrescreve Weapon com o bbox real (rebuild limpa por frame).
        if (p.hasWeapon()) {
            if (auto *w = const_cast<PartState *>(
                    p.body.find(BodyPartId::Weapon))) {
                w->worldBox = computeWeaponBbox(p);
            }
        }
    }
    if (ctx.enemies) {
        ctx.enemies->forEach([](Enemy &s) {
            const auto f = assets::frameData(s.currentFrameId);
            if (f.rows) {
                s.bodyParts.rebuildFromSprite(
                    {s.body.getX(), s.body.getY()},
                    {s.body.getW(), s.body.getH()},
                    f.rows, f.w, f.h, f.pal, f.palCount,
                    s.body.facing);
            } else {
                s.bodyParts.rebuild({s.body.getX(), s.body.getY()},
                                     s.body.facing);
            }
        });
    }
}

} // namespace support
