/**
 * @file tests/test_weapon_reach.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava alcance da arma a partir de Body e facing.
 * @details Cobre BodySystem e contexto, roda com make test que compila em build/tests/test_weapon_reach.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Combat/Body.h"
#include "support/Combat/BodySystem.h"
#include "support/GameContext.h"

using namespace support;

namespace {
bool near(float a, float b) { return std::fabs(a - b) < 0.01f; }
} // namespace

int main() {
    constexpr float kRow = 100.f / 40.f; // AABB 60x100 do ctor: 2.5
    { // SweepEastOnActive (espada + Active = cone E a partir da mão)
        Player p; // (0,0) 60x100, aim E
        p.equipment.equip(core::Item{"iron_sword", 1});
        assert(p.hasWeapon());
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        const support::SweepArc arc = p.sweepArc();
        assert(!arc.empty);
        assert(near(arc.centerAngle, 0.f));
        // Mão a 8 rows + lâmina (16-5)=11px: R=20+27.5, dentro=20-5.
        assert(near(arc.rOuter, 8.f * kRow + 11.f * kRow));
        assert(near(arc.rInner, 8.f * kRow - 2.f * kRow));
    }
    { // SweepUnarmedShorter (soco: punho+avanço 8 rows, sem lâmina)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand);
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.startSwing());
        assert(p.updateMelee(0.10f) == MeleePhase::Active);
        const support::SweepArc arc = p.sweepArc();
        assert(!arc.empty);
        assert(near(arc.rOuter, 8.f * kRow + 8.f * kRow)); // 40 < 47.5
        assert(arc.rOuter < 8.f * kRow + 11.f * kRow);
    }
    { // EmptyOutsideActive (só Active varre)
        Player p;
        p.equipment.equip(core::Item{"iron_sword", 1});
        assert(p.hasWeapon());
        p.body.rebuild({0.f, 0.f}, 1);
        assert(p.sweepArc().empty);
        assert(p.startSwing()); // Windup: ainda vazio
        assert(p.sweepArc().empty);
    }
    { // AxeVsSwordReach (arma diferente = alcance diferente, sem lógica nova)
        // Espada: swing 16px de largura a 2.5x = 40px.
        // Machado: idle 8px de largura a 2.5x = 20px.
        auto reachOf = [](const std::string &defId) {
            Player p;
            p.equipment.equip(core::Item{defId, 1});
            p.meleePhase = MeleePhase::Active;
            p.currentFrameId = SpriteFrameId::PlayerPunch;
            p.facing = 1;
            GameContext ctx{};
            ctx.player = &p;
            BodySystem sys;
            sys.tick(1.f / 30.f, ctx);
            const PartState *w = p.body.find(BodyPartId::Weapon);
            assert(w != nullptr);
            return w->worldBox.width;
        };
        const float sword = reachOf("iron_sword");
        const float axe = reachOf("iron_axe");
        assert(near(sword, 40.f) && near(axe, 20.f));
        assert(sword > axe);
    }

    std::printf("weapon reach test OK\n");
    return 0;
}
