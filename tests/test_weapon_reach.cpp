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
    { // MeleeHitboxFromSwingAim (arma + Active = rect E por snapshot)
        Player p; // (0,0,50,50), centro (25,25), facing 1, swingAim E
        p.loadout.equipped = true;
        p.meleePhase = MeleePhase::Active;
        BodySchema s = BodySchema::humanoid(50.f, 50.f);
        p.body.attach(&s);
        p.body.rebuild({100.f, 100.f}, 1);
        // E = {20,0,20,14}: cx=45, cy=25 → {35,18,20,14}.
        sf::FloatRect box = p.meleeHitbox();
        assert(near(box.left, 35.f) && near(box.width, 20.f));
        assert(near(box.top, 18.f) && near(box.height, 14.f));
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
