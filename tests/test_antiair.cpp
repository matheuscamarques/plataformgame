/**
 * @file tests/test_antiair.cpp
 * @brief Trava o alcance antiaéreo: harpia na órbita (65px) é atingível.
 * @details Via caminhos reais (updateLimbs Active + sweepArc): espada
 * acerta a 65px acima do ombro; soco precisa pular (erra em pé, acerta
 * a 50px). Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include "core/ItemDef.h"
#include "entities/Player/Player.h"

namespace {

void setup(Player& p, support::AimDir aim, bool sword) {
    static auto schema = support::BodySchema::humanoid(100.f, 60.f);
    if (sword && core::ItemRegistry::instance().find("iron_sword"))
        p.equipment.equip(core::Item{"iron_sword", 1});
    p.setX(0.f);
    p.setY(0.f);
    p.setVx(0.f);
    p.setVy(0.f);
    p.jumping = true;
    p.facing = 1;
    p.setFacing8(support::Facing::E);
    p.body.attach(&schema);
    p.body.rebuild({0.f, 0.f}, 1);
    p.swingAim = aim;
    p.meleePhase = MeleePhase::Active;
    p.meleeTimer = 10.f;
    p.updateLimbs();
}

bool hitsAt(Player& p, float dxPx, float dyPx, float radius) {
    const auto arc = p.sweepArc();
    assert(!arc.empty);
    return support::sweepHitsCircle(
        arc, {arc.origin.x + dxPx, arc.origin.y + dyPx}, radius);
}

} // namespace

int main() {
    // Harpia (corpo 36x26): círculo r=13 no centro.
    { // SocoEmPeErraOrbita (11+8 rows = 47px < 65: whiff honesto)
        Player p;
        setup(p, support::AimDir::N, false);
        assert(!hitsAt(p, 0.f, -65.f, 13.f));
    }
    { // SocoAlcancaPerto (47+13 >= 50: pulo curto conecta)
        Player p;
        setup(p, support::AimDir::N, false);
        assert(hitsAt(p, 0.f, -50.f, 13.f));
    }
    { // EspadaAlcancaOrbita (11+11 rows = 55px + 13 >= 65)
        Player p;
        setup(p, support::AimDir::N, true);
        assert(hitsAt(p, 0.f, -65.f, 13.f));
    }
    { // EspadaErraLonge (além da órbita+margem: whiff)
        Player p;
        setup(p, support::AimDir::N, true);
        assert(!hitsAt(p, 0.f, -80.f, 13.f));
    }
    { // LadoIntacto (side reach não mudou: E acerta a 40, erra a 65)
        Player p;
        setup(p, support::AimDir::E, true);
        assert(hitsAt(p, 40.f, 0.f, 13.f));
        assert(!hitsAt(p, 65.f, 0.f, 13.f));
    }
    { // BaixoIntacto (PunchDown no slime baixo-frente: sem regressão)
        Player p;
        setup(p, support::AimDir::S, false);
        assert(hitsAt(p, 10.f, 45.f, 15.f));
    }

    std::printf("antiair test OK\n");
    return 0;
}
