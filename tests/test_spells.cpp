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
    p.unattune("soul_arrow"); // seed vem sintonizado: limpa p/ teste
    p.unattune("heal_light");
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
        assert(p.fp == 280.f); // seed ATT 18
        assert(p.castAttuned(ts));
        assert(p.fp == 255.f); // -25
        assert(ts.activeCount() == 1u);
        bool seen = false;
        ts.forEachActive([&](const Throwable& t) {
            seen = true;
            assert(t.kind == ThrowKind::Bolt);
            assert(t.fuse == -1.f && t.radius == 0.f);
            assert(t.tilesRadius == 0);
            // 30 + 30×((16-10)/20) = 39 (seed INT 14 + 2)
            assert(t.damage == 39);
        });
        assert(seen);
        assert(!p.castAttuned(ts)); // cooldown
        assert(p.fp == 255.f);      // sem gasto no bloqueio
    }
    { // CastHeal (cura com teto, custa 40 FP)
        Player p;
        ThrowSystem ts;
        attuneAll(p);
        assert(p.attune("heal_light"));
        p.hp = 10;
        assert(p.castAttuned(ts));
        assert(p.hp == 10 + 50 + 28); // base + FÉ×2 (seed 12 + 2)
        assert(p.fp == 240.f); // 280 - 40
        assert(ts.activeCount() == 0u); // sem projétil
        p.hp = 90;
        p.throwCooldown.reset();
        assert(p.castAttuned(ts));
        assert(p.hp == p.hpMax); // trava no teto
    }
    { // CastFails (vazio, sem FP, sem req, def morto)
        Player p;
        ThrowSystem ts;
        p.attuned.clear(); // seed vem sintonizado
        assert(!p.castAttuned(ts)); // nada sintonizado
        attuneAll(p);
        assert(p.attune("soul_arrow"));
        p.fp = 10.f;
        assert(!p.castAttuned(ts)); // sem FP
        assert(p.fp == 10.f);
        Player weak;
        // Seed INT 14 cumpre todos os reqs: req-fail coberto em
        // test_player (attune). Aqui: cooldown bloqueia recast.
        weak.attuned.push_back("soul_arrow");
        assert(weak.castAttuned(ts));
        assert(!weak.castAttuned(ts)); // cooldown
        Player stale;
        stale.attuned.clear(); // seed vem sintonizado
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
        attuneAll(p); // seed 14 + 2 = INT 16 (req 14 ok)
        assert(p.attune("fireball"));
        assert(p.fp == 280.f);
        assert(p.castAttuned(ts));
        assert(p.fp == 245.f); // -35
        assert(ts.activeCount() == 1u);
        bool seen = false;
        ts.forEachActive([&](const Throwable& t) {
            seen = true;
            assert(t.kind == ThrowKind::Fireball);
            assert(t.damageType == core::DamageType::Fire);
            assert(t.fuse == -1.f && t.radius == 60.f);
            assert(t.tilesRadius == 0);
            // 40 + 40×((16-10)/20) = 52 (seed 14 + 2)
            assert(t.damage == 52);
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
        assert(p.fp == 250.f); // 280 - 30
        assert(p.weaponBuffType == core::DamageType::Frost);
        assert(p.weaponBuffTimer == 30.f);
        assert(ts.activeCount() == 0u); // buff: sem projétil
    }

    std::printf("spells test OK\n");
    return 0;
}
