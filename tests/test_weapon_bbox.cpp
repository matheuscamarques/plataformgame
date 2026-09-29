/**
 * @file tests/test_weapon_bbox.cpp
 * @brief Teste headless de B.4: mão única p/ bbox e draw.
 * @details weaponHand() = pose IK quando live, box legado senão;
 * computeWeaponBbox segue a pose (não o box). Roda com make test.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Combat/BodySystem.h"
#include "support/GameContext.h"

static bool near(float a, float b, float eps) {
    return std::fabs(a - b) <= eps;
}

int main() {
    using support::BodyPartId;

    { // FallbackLegado (sem live: mão = base do box do ArmR)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        assert(!p.handTargetsLive_);
        const auto* arm = p.body.find(BodyPartId::ArmR);
        assert(arm);
        const core::Vec2f h = p.weaponHand();
        assert(near(h.x, arm->worldBox.left + arm->worldBox.width * 0.5f,
                    1e-6f) &&
               near(h.y, arm->worldBox.top + arm->worldBox.height, 1e-6f));
    }
    { // LiveSeguePose (mão == pose resolvida == alvo alcançável)
        Player p;
        p.body.rebuild({0.f, 0.f}, 1);
        p.aimDir = support::AimDir::W;
        assert(p.startSwing());
        assert(p.handTargetsLive_);
        assert(p.weaponHand().x == p.poseR_.handWorld.x &&
               p.weaponHand().y == p.poseR_.handWorld.y);
        assert(near(p.poseR_.handWorld.x, p.targetHandR_.x, 1e-3f) &&
               near(p.poseR_.handWorld.y, p.targetHandR_.y, 1e-3f));
    }
    { // BboxSeguePose (Active W: centro = mão + offset da espada)
        Player p;
        p.equipment.equip(core::Item{"iron_sword", 1});
        support::GameContext ctx{};
        ctx.player = &p;
        support::BodySystem bs;
        bs.tick(1.f / 30.f, ctx); // boxes do sprite Idle
        p.aimDir = support::AimDir::W;
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        bs.tick(1.f / 30.f, ctx); // bbox com a pose do Active
        const auto* w = p.body.find(BodyPartId::Weapon);
        assert(w && w->worldBox.width > 0.f);
        // Espada 16x8, origem (5,5), offset (4,8), s=2.5:
        // centro = mão + (10-12.5+20, 20-12.5+10) = mão + (17.5,17.5).
        const float cx = w->worldBox.left + w->worldBox.width * 0.5f;
        const float cy = w->worldBox.top + w->worldBox.height * 0.5f;
        assert(near(cx, p.poseR_.handWorld.x + 17.5f, 1.f) &&
               near(cy, p.poseR_.handWorld.y + 17.5f, 1.f));
        // Discriminador: legado usaria a base do box (abaixo do torso),
        // 20px à direita da pose (recuo W de 8 rows). Precisa diferir.
        const auto* arm = p.body.find(BodyPartId::ArmR);
        assert(arm);
        const float legX =
            arm->worldBox.left + arm->worldBox.width * 0.5f + 17.5f;
        assert(std::fabs(cx - legX) > 5.f);
    }

    std::printf("weapon bbox test OK\n");
    return 0;
}
