/**
 * @file tests/test_juice.cpp
 * @brief Teste headless do juice procedural (pouso, bob, squash).
 * @details Cobre landAnimT (edge no collide, decay no tick, sem squash
 * na água) e helpers puros. Roda com make test.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include "assets/PlayerSprite.h"
#include "core/EntityKind.h"
#include "entities/Entity.hpp"
#include "entities/Player/Player.h"

static bool near(float a, float b, float eps = 1e-4f) {
    return std::fabs(a - b) <= eps;
}

int main() {
    { // LandArmaNoPouso (borda de descida; repetir não rearma)
        Player p;
        Entity ground(4242, 0.f, 100.f, 200.f, 50.f);
        p.setX(0.f);
        p.setY(60.f); // base dentro do chão
        p.jumping = false;
        assert(p.landAnimT == 0.f);
        p.collide(ground);
        assert(p.jumping);
        assert(near(p.landAnimT, Player::kLandAnimDur));
        assert(near(p.getY(), 0.f)); // snap no topo, sem push lateral
        assert(near(p.getX(), 0.f));
        p.collide(ground); // já no chão: sem rearmar
        assert(near(p.landAnimT, Player::kLandAnimDur));
    }
    { // LandDecaiNoTick (0.12s a 1/30: 4 ticks zeram)
        Player p;
        Entity ground(4242, 0.f, 100.f, 200.f, 50.f);
        p.setX(0.f);
        p.setY(60.f);
        p.jumping = false;
        p.collide(ground);
        assert(p.landAnimT > 0.f);
        p.tick();
        assert(p.landAnimT > 0.f && p.landAnimT < Player::kLandAnimDur);
        p.tick();
        p.tick();
        p.tick();
        assert(p.landAnimT == 0.f);
    }
    { // AguaSemSquash (água seta chão sem animar pouso)
        Player p;
        Entity water(core::kIdWater, 0.f, 100.f, 200.f, 50.f);
        p.setX(0.f);
        p.setY(60.f);
        p.jumping = false;
        p.collide(water);
        assert(p.jumping && p.landAnimT == 0.f);
    }
    { // IdleBob (1Hz: zero no tick 0, pico ~tick 7)
        assert(near(game::idleBobY(0), 0.f));
        assert(near(game::idleBobY(7), 0.9945f, 1e-3f));
        assert(near(game::idleBobY(15), 0.f, 1e-3f));
        assert(near(game::idleBobY(7, 2.f), 1.989f, 1e-3f));
    }
    { // WalkBob (contact A/C, par, afunda 1.5px no impacto)
        assert(near(game::walkBobY(0), 1.5f));
        assert(near(game::walkBobY(1), 0.f));
        assert(near(game::walkBobY(2), 1.5f));
        assert(near(game::walkBobY(3), 0.f));
    }
    { // LandSquash (18% cheio, metade na metade, identidade fora)
        const auto full = game::landSquash(0.12f);
        assert(near(full.kx, 1.18f) && near(full.ky, 0.82f));
        const auto half = game::landSquash(0.06f);
        assert(near(half.kx, 1.09f) && near(half.ky, 0.91f));
        const auto off = game::landSquash(0.f);
        assert(near(off.kx, 1.f) && near(off.ky, 1.f));
        const auto neg = game::landSquash(-1.f);
        assert(near(neg.kx, 1.f) && near(neg.ky, 1.f));
        const auto over = game::landSquash(0.5f); // clamp no teto
        assert(near(over.kx, 1.18f) && near(over.ky, 0.82f));
    }
    { // FallStretch (só queda rápida; borda estrita em 12)
        const auto fast = game::fallStretch(20.f);
        assert(near(fast.kx, 0.9f) && near(fast.ky, 1.1f));
        const auto edge = game::fallStretch(12.f);
        assert(near(edge.kx, 1.f) && near(edge.ky, 1.f));
        const auto up = game::fallStretch(-5.f);
        assert(near(up.kx, 1.f) && near(up.ky, 1.f));
    }

    std::printf("juice test OK\n");
    return 0;
}
