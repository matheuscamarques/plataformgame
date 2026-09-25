#include <cassert>
#include <cstdio>

#include "entities/Player/Player.h"
#include "support/Combat/ExplosionSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Effects/Throwable.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// F8b: G conjura sintonizada — Arrow vira Bolt, Heal cura, FP paga.
namespace {
void attuneAll(Player& p) {
    int souls = 1000000000;
    p.attrs.buy(core::Attr::Intelligence, souls);
    p.attrs.buy(core::Attr::Intelligence, souls); // INT 12
    p.attrs.buy(core::Attr::Faith, souls);
    p.attrs.buy(core::Attr::Faith, souls); // FÉ 12
    p.attrs.buy(core::Attr::Attunement, souls);
    p.attrs.buy(core::Attr::Attunement, souls); // ATT 12: 1 slot
    p.refreshDerived();
}
} // namespace

int main() {
    using namespace support;

    { // CastArrow (stats, custo FP, cooldown barra 2º)
        Player p;
        ThrowSystem ts;
        attuneAll(p);
        assert(p.attune("soul_arrow"));
        assert(p.fp == 200.f);
        assert(p.castAttuned(ts));
        assert(p.fp == 175.f); // -25
        assert(ts.activeCount() == 1u);
        bool seen = false;
        ts.forEachActive([&](const Throwable& t) {
            seen = true;
            assert(t.kind == ThrowKind::Bolt);
            assert(t.fuse == -1.f && t.radius == 0.f);
            assert(t.tilesRadius == 0);
            // 30 + 30×((12-10)/20) = 33 (rampa nova)
            assert(t.damage == 33);
        });
        assert(seen);
        assert(!p.castAttuned(ts)); // cooldown
        assert(p.fp == 175.f);      // sem gasto no bloqueio
    }
    { // CastHeal (cura com teto, custa 40 FP)
        Player p;
        ThrowSystem ts;
        attuneAll(p);
        assert(p.attune("heal_light"));
        p.hp = 10;
        assert(p.castAttuned(ts));
        assert(p.hp == 10 + 50 + 24); // base + FÉ×2
        assert(p.fp == 160.f);
        assert(ts.activeCount() == 0u); // sem projétil
        p.hp = 90;
        p.throwCooldown.reset();
        assert(p.castAttuned(ts));
        assert(p.hp == p.hpMax); // trava no teto
    }
    { // CastFails (vazio, sem FP, sem req, def morto)
        Player p;
        ThrowSystem ts;
        assert(!p.castAttuned(ts)); // nada sintonizado
        attuneAll(p);
        assert(p.attune("soul_arrow"));
        p.fp = 10.f;
        assert(!p.castAttuned(ts)); // sem FP
        assert(p.fp == 10.f);
        Player weak;
        weak.attuned.push_back("soul_arrow"); // INT 10: req falha
        assert(!weak.castAttuned(ts));
        Player stale;
        stale.attuned.push_back("nao_existe");
        assert(!stale.castAttuned(ts));
    }
    { // BoltHitsEnemy (impacto drena HP e libera o projétil)
        Player p;
        EnemySystem enemies;
        ThrowSystem ts;
        enemies.spawn("slime", 100.f, 100.f);
        int hp0 = -1;
        enemies.forEach([&](Enemy& s) { hp0 = s.resources.hp; });
        Throwable* t = ts.throwItem({110.f, 110.f}, {0.f, 0.f},
                                    ThrowKind::Bolt);
        assert(t != nullptr);
        t->fuse = -1.f;
        t->damage = 30;
        GameContext ctx{};
        ctx.enemies = &enemies;
        ts.tick(1.f / 30.f, ctx);
        int hp1 = -1;
        enemies.forEach([&](Enemy& s) { hp1 = s.resources.hp; });
        assert(hp1 == hp0 - 30);
        assert(ts.activeCount() == 0u);
    }
    { // FpRegen (8/s até o teto)
        Player p;
        p.fp = 100.f;
        p.tick();
        assert(p.fp > 100.f && p.fp <= 200.f);
    }
    { // CastFireball (custo, projétil Fire, raio sem tiles)
        Player p;
        ThrowSystem ts;
        attuneAll(p);
        int souls = 1000000000;
        p.attrs.buy(core::Attr::Intelligence, souls);
        p.attrs.buy(core::Attr::Intelligence, souls); // INT 14
        p.refreshDerived();
        assert(p.attune("fireball"));
        assert(p.fp == 200.f);
        assert(p.castAttuned(ts));
        assert(p.fp == 165.f); // -35
        assert(ts.activeCount() == 1u);
        bool seen = false;
        ts.forEachActive([&](const Throwable& t) {
            seen = true;
            assert(t.kind == ThrowKind::Fireball);
            assert(t.damageType == core::DamageType::Fire);
            assert(t.fuse == -1.f && t.radius == 40.f);
            assert(t.tilesRadius == 0);
            // 40 + 40×((14-10)/20) = 48
            assert(t.damage == 48);
        });
        assert(seen);
    }
    { // FireballExplodesFire (impacto explode em área com tipo)
        Player p;
        EnemySystem enemies;
        ThrowSystem ts;
        ExplosionSystem expl;
        ts.setExplosionSystem(&expl);
        enemies.spawn("skeleton", 100.f, 100.f); // fogo 1.3
        enemies.forEach([](support::Enemy &s) {
            s.bodyParts.rebuild({s.body.getX(), s.body.getY()}, 1);
        });
        Throwable* t = ts.throwItem({130.f, 140.f}, {0.f, 0.f},
                                    ThrowKind::Fireball);
        assert(t != nullptr);
        t->fuse = -1.f;
        t->damage = 40;
        t->damageType = core::DamageType::Fire;
        t->radius = 40.f;
        GameContext ctx{};
        ctx.enemies = &enemies;
        std::vector<ExplosionTarget> targets;
        enemies.forEach([&](Enemy& s) {
            targets.push_back({{s.body.getCenterX(), s.body.getCenterY()},
                               &s.bodyParts, &s.resources, false});
        });
        ctx.explosionTargets = &targets;
        ts.tick(1.f / 30.f, ctx);
        int hp = -1;
        enemies.forEach([&](Enemy& s) { hp = s.resources.hp; });
        // 45 - 40×1.3×mult(parte 1.0+) < 45: fogo morde o esqueleto
        assert(hp < 45);
        assert(ts.activeCount() == 0u);
    }
    { // FrostWeaponBuffsMelee (cast seta Frost 30s; melee usa o tipo)
        Player p;
        ThrowSystem ts;
        attuneAll(p);
        assert(p.attune("frost_weapon"));
        assert(p.castAttuned(ts));
        assert(p.fp == 170.f); // -30
        assert(p.weaponBuffType == core::DamageType::Frost);
        assert(p.weaponBuffTimer == 30.f);
        assert(ts.activeCount() == 0u); // buff: sem projétil
    }

    std::printf("spells test OK\n");
    return 0;
}
