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
    { // MeleeHitboxFromBodyWeapon (arma equipada + Active = bbox do Body)
        Player p;
        p.loadout.equipped = true;
        p.meleePhase = MeleePhase::Active;
        BodySchema s = BodySchema::humanoid(50.f, 50.f);
        p.body.attach(&s);
        p.body.rebuild({100.f, 100.f}, 1);
        if (auto *w = const_cast<PartState *>(p.body.find(BodyPartId::Weapon))) {
            w->worldBox = {130.f, 120.f, 24.f, 8.f};
        }
        sf::FloatRect box = p.meleeHitbox();
        assert(near(box.left, 130.f) && near(box.width, 24.f));
    }
    { // FallbackToLightWhenNoWeapon (sem equipamento = kLight, soco)
        Player p;
        p.loadout.equipped = false;
        p.meleePhase = MeleePhase::Active;
        BodySchema s = BodySchema::humanoid(50.f, 50.f);
        p.body.attach(&s);
        p.body.rebuild({100.f, 100.f}, 1);
        // Weapon do schema cai em 0.1px → fallback kLight[0].hx = 16.
        sf::FloatRect box = p.meleeHitbox();
        assert(near(box.width, 16.f));
    }
    { // NoHitboxOutsideActive (só Active acerta)
        Player p;
        p.loadout.equipped = true;
        p.meleePhase = MeleePhase::Windup;
        BodySchema s = BodySchema::humanoid(50.f, 50.f);
        p.body.attach(&s);
        p.body.rebuild({100.f, 100.f}, 1);
        sf::FloatRect box = p.meleeHitbox();
        assert(box.width <= 0.f && box.height <= 0.f);
    }
    { // AxeVsSwordReach (arma diferente = alcance diferente, sem lógica nova)
        // Espada: swing 16px de largura a 2.5x = 40px.
        // Machado: idle 8px de largura a 2.5x = 20px.
        auto reachOf = [](const std::string &weaponId) {
            Player p;
            p.loadout.equipped = true;
            p.loadout.weaponId = weaponId;
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
        const float sword = reachOf("sword");
        const float axe = reachOf("axe");
        assert(near(sword, 40.f) && near(axe, 20.f));
        assert(sword > axe);
    }

    std::printf("weapon reach test OK\n");
    return 0;
}
