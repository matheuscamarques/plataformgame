/**
 * @file tests/test_skill.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava skill cara com gate de stamina insuficiente.
 * @details Cobre SkillSystem e custo, roda com make test que compila em build/tests/test_skill.
 */

#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Skills/Skill.h"
#include "support/Skills/SkillSystem.h"
#include "support/Effects/ThrowSystem.h"
#include "world/World.h"

// Skill impossível de pagar (custo 9999) p/ testar gate de recursos.
REGISTER_SKILL("test_expensive", [] {
    support::SkillDef s;
    s.name = "Expensive";
    s.staminaCost = 9999.f;
    s.execute = [](support::Enemy &, support::GameContext &,
                    const support::SkillDef &) {};
    return s;
}());

// slime_spit valida SkillRegistry + SkillSystem (3b): registro, pay,
// cooldown, execução, impacto no player, expiração sem fuse.
int main() {
    using namespace support;
    auto pinY = [](EnemySystem &e) {
        e.forEach([](Enemy &s) { s.body.setY(100.f); s.body.setVy(0.f); });
    };

    { // SlimeSpitRegistered
        const SkillDef *s = SkillRegistry::instance().find("slime_spit");
        assert(s != nullptr && s->isRanged);
        assert(s->cooldown == 1.8f && s->maxRange == 220.f);
        assert(SkillRegistry::instance().find("nope") == nullptr);
        assert(SkillRegistry::instance().keys().size() == 10u); // +dwarfs+esqueleto+frost+test
    }
    { // TryUseFailsWithoutResources (custo 9999, nem Elite paga)
        EnemySystem enemies;
        enemies.spawn("dwarf", 0.f, 0.f); // Elite: paga de verdade
        GameContext ctx{};
        bool ran = true;
        enemies.forEach([&](Enemy &e) {
            ran = SkillSystem::tryUse(e, ctx, "test_expensive");
        });
        assert(!ran);
    }
    { // TryUseFailsUnknown
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);
        GameContext ctx{};
        bool ran = true;
        enemies.forEach([&](Enemy &e) { ran = SkillSystem::tryUse(e, ctx, "nope"); });
        assert(!ran);
    }
    { // CooldownBlocksSecondUseThenTicksReady
        EnemySystem enemies;
        enemies.spawn("slime", 0.f, 0.f);
        GameContext ctx{};
        bool first = false, second = true;
        enemies.forEach([&](Enemy &e) {
            first = SkillSystem::tryUse(e, ctx, "slime_spit");
            second = SkillSystem::tryUse(e, ctx, "slime_spit");
            assert(!second);
            SkillSystem::tick(e, 2.0f);
            assert(SkillSystem::tryUse(e, ctx, "slime_spit"));
        });
        assert(first);
    }
    { // SlimeSpitsInRange (integração: AI → throw ativo)
        Player p;
        p.setX(200.f);
        p.setY(100.f);
        EnemySystem enemies;
        enemies.spawn("slime", 100.f, 100.f);
        ThrowSystem throws;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.throws = &throws;
        for (int i = 0; i < 90; ++i) { pinY(enemies); enemies.tick(1.f / 30.f, ctx); }
        assert(throws.activeCount() >= 1u);
    }
    { // SpitHurtsPlayerOnImpact (dano 8, sem gravidade/fuse)
        Player p;
        p.setX(200.f);
        p.setY(100.f);
        EnemySystem enemies;
        enemies.spawn("slime", 100.f, 100.f);
        ThrowSystem throws;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        ctx.throws = &throws;
        for (int i = 0; i < 150; ++i) {
            pinY(enemies);
            enemies.tick(1.f / 30.f, ctx);
            throws.tick(1.f / 30.f, ctx);
        }
        assert(p.hp < 100); // cuspe acertou (hurt respeita i-frame; ≥1 passou)
    }
    { // SpitWithoutFuseNeverExplodesAndExpiresResting (mundo real)
        World world(1337u);
        world.update(0, 0);
        ThrowSystem throws;
        // Rocha garantida: varre até achar tile sólido (pedra, não caverna).
        int rtx = 0, rty = 0;
        bool rock = false;
        for (int tx = -20; tx < 20 && !rock; tx++) {
            for (int ty = 0; ty < 60 && !rock; ty++) {
                if (world.isSolid(tx, ty)) { rtx = tx; rty = ty; rock = true; }
            }
        }
        assert(rock);
        // Nasce DENTRO da rocha, parado: colide no 1º tick, expira sem explodir.
        auto *t = throws.throwItem({rtx * 50.f + 25.f, rty * 50.f + 25.f},
                                   {0.f, 0.f}, ThrowKind::Spit);
        assert(t != nullptr);
        t->gravity = 0.f;
        t->fuse = -1.f;
        t->damage = 8;
        t->radius = 0.f;
        t->tilesRadius = 0;
        GameContext ctx{};
        ctx.world = &world;
        for (int i = 0; i < 120; ++i) throws.tick(1.f / 30.f, ctx);
        assert(throws.activeCount() == 0u); // expirou parado, pool livre
        // E sem mundo: parado no ar também expira (sem crash, sem partículas).
        auto *u = throws.throwItem({0.f, 0.f}, {0.f, 0.f}, ThrowKind::Spit);
        u->gravity = 0.f;
        u->fuse = -1.f;
        GameContext empty{};
        for (int i = 0; i < 120; ++i) throws.tick(1.f / 30.f, empty);
        assert(throws.activeCount() == 0u);
    }

    std::printf("skill test OK\n");
    return 0;
}
