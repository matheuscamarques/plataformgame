/**
 * @file tests/test_explosion.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava raio, whiff, headshot 2x e knockback.
 * @details Cobre ExplosionSystem e Body, roda com make test que compila em build/tests/test_explosion.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
#include "support/Combat/ExplosionSystem.h"
#include "support/Combat/Body.h"
#include "support/Enemies/EnemyResources.h"
#include "support/GameContext.h"
#include "entities/Entity.hpp"

// Explosão: raio, whiff, headshot 2x, knockback no mover + lock.
int main() {
    using namespace support;

    { // HitsTargetInsideRadius
        ExplosionSystem es;
        Body b;
        auto s = BodySchema::humanoid(24.f, 12.f);
        b.attach(&s);
        b.rebuild({100.f, 100.f}, 1);

        EnemyResources res;
        res.hp = res.hpMax = 100;

        std::vector<ExplosionTarget> targets;
        targets.push_back({{106.f, 112.f}, &b, &res, false});

        GameContext ctx{};
        ctx.explosionTargets = &targets;

        ExplosionDef def;
        def.radius = 30.f;
        def.damage = 20;

        int hit = es.explode({106.f, 112.f}, def, ctx);
        assert(hit == 1 && res.hp < 100);
    }
    { // MissesTargetOutsideRadius
        ExplosionSystem es;
        Body b;
        auto s = BodySchema::humanoid();
        b.attach(&s);
        b.rebuild({500.f, 500.f}, 1);

        EnemyResources res;
        res.hp = res.hpMax = 100;

        std::vector<ExplosionTarget> targets;
        targets.push_back({{506.f, 512.f}, &b, &res, false});

        GameContext ctx{};
        ctx.explosionTargets = &targets;

        ExplosionDef def;
        def.radius = 20.f;

        int hit = es.explode({0.f, 0.f}, def, ctx);
        assert(hit == 0 && res.hp == 100);
    }
    { // HeadHitMultipliesDamage (head 2.0x)
        ExplosionSystem es;
        Body b;
        auto s = BodySchema::humanoid(24.f, 12.f);
        b.attach(&s);
        b.rebuild({100.f, 100.f}, 1);

        const PartState *head = b.find(BodyPartId::Head);
        assert(head != nullptr);
        sf::Vector2f headCenter{
            head->worldBox.left + head->worldBox.width * 0.5f,
            head->worldBox.top + head->worldBox.height * 0.5f
        };

        EnemyResources res;
        res.hp = res.hpMax = 200;

        std::vector<ExplosionTarget> targets;
        targets.push_back({{106.f, 112.f}, &b, &res, false});

        GameContext ctx{};
        ctx.explosionTargets = &targets;

        ExplosionDef def;
        def.radius = 40.f;
        def.damage = 20;

        es.explode(headCenter, def, ctx);
        assert(res.hp <= 160); // 20 * 2.0 = 40
    }
    { // WhiffPicksNothing (attackBox longe de tudo = 0 hits)
        ExplosionSystem es;
        Body b;
        auto s = BodySchema::humanoid();
        b.attach(&s);
        b.rebuild({1000.f, 1000.f}, 1);

        EnemyResources res;
        res.hp = res.hpMax = 100;

        std::vector<ExplosionTarget> targets;
        targets.push_back({{1006.f, 1012.f}, &b, &res, false});

        GameContext ctx{};
        ctx.explosionTargets = &targets;

        ExplosionDef def;
        def.radius = 5.f;

        // Centro longe do corpo: nenhuma parte no raio.
        int hit = es.explode({0.f, 0.f}, def, ctx);
        assert(hit == 0 && res.hp == 100);
    }
    { // KnockbackAppliedWhenMoverProvided (impulso + lock da IA)
        ExplosionSystem es;
        Body b;
        auto s = BodySchema::humanoid(24.f, 12.f);
        b.attach(&s);
        b.rebuild({100.f, 100.f}, 1);

        EnemyResources res;
        res.hp = res.hpMax = 200;

        Entity mover(0, 100.f, 100.f, 50.f, 50.f);
        core::Cooldown lock;

        std::vector<ExplosionTarget> targets;
        targets.push_back({{106.f, 112.f}, &b, &res, false, &mover, &lock});

        GameContext ctx{};
        ctx.explosionTargets = &targets;

        ExplosionDef def;
        def.radius = 40.f;
        def.knockback = 300.f;

        int hit = es.explode({100.f, 100.f}, def, ctx);
        assert(hit == 1);
        assert(std::fabs(mover.getVx()) + std::fabs(mover.getVy()) > 0.f);
        assert(lock.running()); // IA não apaga o impulso no próximo tick
    }

    std::printf("explosion test OK\n");
    return 0;
}
