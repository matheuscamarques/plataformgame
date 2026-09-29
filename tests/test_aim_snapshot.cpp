/**
 * @file tests/test_aim_snapshot.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava snapshot de mira que congela direção do golpe.
 * @details Cobre AimDir e Player, roda com make test que compila em build/tests/test_aim_snapshot.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Combat/AimDir.h"

namespace {
bool near(float a, float b) { return std::fabs(a - b) < 0.01f; }
} // namespace

int main() {
    using support::AimDir;

    { // SwingFreezesDirection (snapshot: input posterior não move o golpe)
        Player p;
        p.equipment.equip(core::Item{"iron_sword", 1});
        p.meleePhase = MeleePhase::Idle;
        assert(p.hasWeapon());
        p.aimDir = AimDir::N;

        assert(p.startSwing());
        assert(p.swingAim == AimDir::N);

        p.aimDir = AimDir::E; // meio do swing: input muda
        assert(p.swingAim == AimDir::N);
    }
    { // SweepUsesSwingAim (N = cone apontando p/ cima)
        Player p;
        p.equipment.equip(core::Item{"iron_sword", 1});
        assert(p.hasWeapon());
        p.body.rebuild({0.f, 0.f}, 1);
        p.aimDir = AimDir::N;
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);

        const support::SweepArc arc = p.sweepArc();
        assert(!arc.empty);
        // N em screen-space (Y p/ baixo) = -90°.
        assert(near(arc.centerAngle, -3.14159265f * 0.5f));
        assert(arc.rOuter > arc.rInner && arc.rInner >= 0.f);
    }
    { // SweepUnarmedAimBased (soco segue a mira, não o facing)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand);
        p.body.rebuild({0.f, 0.f}, 1);
        p.aimDir = AimDir::N;
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);

        const support::SweepArc arc = p.sweepArc();
        assert(!arc.empty);
        assert(near(arc.centerAngle, -3.14159265f * 0.5f));
    }
    { // ResolveAimCoversEightWays
        using support::resolveAim;
        assert(resolveAim(false, false, false, true, 1) == AimDir::E);
        assert(resolveAim(false, false, true, false, 1) == AimDir::W);
        assert(resolveAim(true, false, false, false, 1) == AimDir::N);
        assert(resolveAim(false, true, false, false, 1) == AimDir::S);
        assert(resolveAim(true, false, false, true, 1) == AimDir::NE);
        assert(resolveAim(true, false, true, false, 1) == AimDir::NW);
        assert(resolveAim(false, true, false, true, 1) == AimDir::SE);
        assert(resolveAim(false, true, true, false, 1) == AimDir::SW);
        assert(resolveAim(false, false, false, false, -1) == AimDir::W);
        assert(resolveAim(false, false, false, false, 1) == AimDir::E);
    }
    { // AimVectorIsUnit
        for (int i = 0; i < static_cast<int>(AimDir::COUNT); ++i) {
            core::Vec2f v =
                support::aimVector(static_cast<AimDir>(i));
            assert(near(std::sqrt(v.x * v.x + v.y * v.y), 1.f));
        }
        core::Vec2f n = support::aimVector(AimDir::N);
        assert(near(n.x, 0.f) && near(n.y, -1.f)); // Y cresce p/ baixo
    }

    std::printf("aim snapshot test OK\n");
    return 0;
}
