/**
 * @file tests/test_telegraph.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava windup só em contato com rearma cheio.
 * @details Cobre ContactDamageSystem, roda com make test que compila em build/tests/test_telegraph.
 */

#include <cassert>
#include <cstdio>
#include "entities/Player/Player.h"
#include "support/Combat/ContactDamageSystem.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// Telegraph 0.35s: windup conta só em contato, recupera sem, morde e
// rearma; 2ª mordida exige windup cheio de novo.
int main() {
    using namespace support;
    auto recol = [](EnemySystem &e) {
        e.forEach([](Enemy &s) { s.body.setX(10.f); s.body.setY(10.f); });
    };
    auto windup = [](EnemySystem &e) {
        float w = -1.f;
        e.forEach([&](Enemy &s) { w = s.biteWindup; });
        return w;
    };

    { // WindupTicksDownInContact (vermelho = windup < cheio, sem dano)
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 3; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        assert(windup(enemies) < 0.35f && p.hp == 100);
    }
    { // BiteResetsWindup (mordeu → rearma cheio)
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 11; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        assert(p.hp == 90);
        assert(windup(enemies) == 0.35f);
    }
    { // WindupRecoversWithoutContact (saiu de perto: sobe, não zera)
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 5; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        const float partial = windup(enemies);
        assert(partial < 0.35f);
        enemies.forEach([](Enemy &s) { s.body.setX(500.f); s.body.setY(500.f); });
        cs.tick(1.f / 30.f, ctx);
        const float after = windup(enemies);
        assert(after > partial && after <= 0.35f);
    }
    { // TintRestoresWhenFull (longe por tempo: volta ao cheio)
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 5; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        enemies.forEach([](Enemy &s) { s.body.setX(500.f); s.body.setY(500.f); });
        for (int i = 0; i < 60; ++i) cs.tick(1.f / 30.f, ctx);
        assert(windup(enemies) == 0.35f);
    }
    { // SecondBiteNeedsFullWindup (5 ticks pós-mordida: sem dano novo)
        Player p;
        EnemySystem enemies;
        enemies.spawn("slime", 10.f, 10.f);

        ContactDamageSystem cs;
        GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;

        for (int i = 0; i < 11; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        assert(p.hp == 90);
        p.hurtIframes.tick(1.f); // expira i-frame: próxima morde quando windup
        for (int i = 0; i < 5; ++i) { recol(enemies); cs.tick(1.f / 30.f, ctx); }
        assert(p.hp == 90); // windup (0.35) ainda não esgotou de novo
    }

    std::printf("telegraph test OK\n");
    return 0;
}
