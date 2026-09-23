/**
 * @file tests/test_melee_parts.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava narrowphase com maior damageMult tocado.
 * @details Cobre Body e partículas, roda com make test que compila em build/tests/test_melee_parts.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Combat/Body.h"
#include "support/Combat/MeleeSystem.h"
#include "support/Effects/ParticleSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// Melee per-part: narrowphase escolhe maior damageMult entre as partes
// tocadas (mesma regra da explosão). Slime humanoid(30,40) em (32,10):
// Head [40,64]x[11.2,17.8] 2.0x/1.5x, Torso [32,72]x[18.7,28.3] 1x,
// ArmL [28,32]x[18.4,26.8] 0.6x/0.5x. Soco combo0: 8 dmg, 5 posture.
namespace {

void rebuildAll(support::EnemySystem &enemies) {
    enemies.forEach([](support::Enemy &s) {
        s.bodyParts.rebuild({s.body.getX(), s.body.getY()}, 1);
    });
}

void swingOnce(support::MeleeSystem &ms, support::GameContext &ctx,
               Player &p, int ticks = 6) {
    assert(p.startSwing());
    for (int i = 0; i < ticks; ++i) ms.tick(1.f / 30.f, ctx);
}

void readEnemy(support::EnemySystem &enemies, int &hp, float &posture) {
    enemies.forEach([&](support::Enemy &s) {
        hp = s.resources.hp;
        posture = s.resources.posture;
    });
}

bool near(float a, float b) { return std::fabs(a - b) < 1e-4f; }

} // namespace

int main() {
    using namespace support;

    { // TorsoHit1x (head escapa 7px acima; best=torso entre 1.0/0.6/0.7)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand); // soco: geometria fixa facing-E
        p.setY(10.f); // soco [30,46]x[25,45]: head [11.2,17.8] fora
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        rebuildAll(enemies);

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        swingOnce(ms, ctx, p);
        int hp = -1;
        float posture = -1.f;
        readEnemy(enemies, hp, posture);
        assert(hp == 22); // 8 * 1.0
        assert(near(posture, 15.f)); // 20 - 5 * 1.0
    }
    { // ArmHit06x (só ArmL; torso escapa 0.5px à direita)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand); // soco: geometria fixa facing-E
        p.setX(-14.5f); // soco [15.5,31.5]x[15,35]: torso x>=32 fora
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        rebuildAll(enemies);

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        swingOnce(ms, ctx, p);
        int hp = -1;
        float posture = -1.f;
        readEnemy(enemies, hp, posture);
        assert(hp == 26); // 8 * 0.6 = 4 (trunca)
        assert(near(posture, 17.5f)); // 20 - 5 * 0.5
    }
    { // WhiffInsideAABB (cruza o corpo no canto sem partes, sem dano)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand); // soco: geometria fixa facing-E
        p.setX(-13.f);
        p.setY(-24.f); // soco [17,33]x[9,11]: AABB ok, head x>=40 fora
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        rebuildAll(enemies);

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        swingOnce(ms, ctx, p);
        int hp = -1;
        float posture = -1.f;
        readEnemy(enemies, hp, posture);
        assert(hp == 30);
        assert(near(posture, 20.f));
    }
    { // NarrowWhiffInPartGap (união ok, nenhuma parte: whiff strict)
        // Schema de teste com vão: head [47,57]x[11,15], perna
        // [47,57]x[35,39]. Soco [50,66]x[15,35] cruza a união mas só
        // encosta nas bordas (edge-touch estrito não intersecta).
        static BodySchema gap;
        gap.overallHeight = 30.f;
        gap.halfWidth = 20.f;
        gap.parts = {
            {BodyPartId::Head, {0.f, -12.f}, {10.f, 4.f}, 2.0f, 1.5f, false},
            {BodyPartId::LegL, {0.f, 12.f}, {10.f, 4.f}, 0.7f, 0.8f, false},
        };
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand); // soco: geometria fixa facing-E
        p.setX(20.f);
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        enemies.forEach([&](Enemy &s) {
            s.bodyParts.attach(&gap);
            s.bodyParts.rebuild({s.body.getX(), s.body.getY()}, 1);
        });

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        swingOnce(ms, ctx, p);
        int hp = -1;
        float posture = -1.f;
        readEnemy(enemies, hp, posture);
        assert(hp == 30);
        assert(near(posture, 20.f));
    }
    { // NoSchemaAABB (sem schema = legado 1x; regressão Fase C)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand); // soco: geometria fixa facing-E
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        enemies.forEach([](Enemy &s) {
            s.bodyParts.schema = nullptr;
            s.bodyParts.parts.clear();
        });

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        swingOnce(ms, ctx, p);
        int hp = -1;
        float posture = -1.f;
        readEnemy(enemies, hp, posture);
        assert(hp == 22);
        assert(near(posture, 15.f));
    }
    { // DedupPerSwing (30 ticks = 1 aplicação; torso 1x)
        Player p;
        p.equipment.unequip(core::EquipSlot::RightHand); // soco: geometria fixa facing-E
        p.setY(10.f);
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        rebuildAll(enemies);

        MeleeSystem ms;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        swingOnce(ms, ctx, p, 30);
        int hp = -1;
        float posture = -1.f;
        readEnemy(enemies, hp, posture);
        assert(hp == 22);
        assert(near(posture, 15.f));
    }
    { // SparkSpawnsOnPartHit (faísca observável via activeDebris)
        Player p; // (0,0): best=head
        p.equipment.unequip(core::EquipSlot::RightHand); // soco: geometria fixa facing-E
        EnemySystem enemies;
        enemies.spawn("slime", 32.f, 10.f);
        rebuildAll(enemies);

        MeleeSystem ms;
        ParticleSystem ps;
        ms.setParticleSystem(&ps);
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        assert(ps.activeDebris() == 0u && ps.activeDust() == 0u);
        swingOnce(ms, ctx, p);
        assert(ps.activeDust() > 0u); // spark emite poeira, não detrito
    }

    std::printf("melee parts test OK\n");
    return 0;
}
