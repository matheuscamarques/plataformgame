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
        p.meleePhase = MeleePhase::Idle;
        assert(p.hasWeapon()); // seed: espada de ferro
        p.aimDir = AimDir::N;

        assert(p.startSwing());
        assert(p.swingAim == AimDir::N);

        p.aimDir = AimDir::E; // meio do swing: input muda
        assert(p.swingAim == AimDir::N);
    }
    { // HitboxUsesSwingAim (N = 20px acima do centro)
        Player p;
        assert(p.hasWeapon()); // seed: espada de ferro
        p.meleePhase = MeleePhase::Active;
        p.meleeCombo = 0;
        p.swingAim = AimDir::N;

        sf::FloatRect box = p.meleeHitbox();
        // Rect do N x2 (corpo 100): cy=-40, h=40 → topo 60px acima.
        assert(near(box.top, p.getCenterY() - 60.f));
        assert(near(box.height, 40.f));
        assert(near(box.top + box.height, p.getCenterY() - 20.f));
    }
    { // FallbackSocoWhenUnequipped (N ignorado sem arma)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand);
        p.meleePhase = MeleePhase::Active;
        p.meleeCombo = 0; // kLight[0].hx = 16
        p.swingAim = AimDir::N;

        sf::FloatRect box = p.meleeHitbox();
        assert(near(box.width, 16.f));
        // Soco é reto à frente, centrado em Y (sem -20 do N).
        assert(near(box.top + box.height * 0.5f, p.getCenterY()));
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
