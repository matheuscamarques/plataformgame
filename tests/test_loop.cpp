#include <cassert>
#include <cstdio>
#include "core/System.h"
#include "entities/player/player.h"
#include "support/BodySystem.h"
#include "support/DeathSystem.h"
#include "support/DropSystem.h"
#include "support/EnemySystem.h"
#include "support/ExplosionSystem.h"
#include "support/GameContext.h"
#include "support/ParticleSystem.h"
#include "support/ThrowSystem.h"
#include "support/World/World.h"

// Loop jogável ponta a ponta: throw → explode → slime morre →
// orb dropa → player coleta. Mesmas prioridades e fiação do Game.
// Também é stress do segfault do J (breakTile + removeDead no tick).
int main() {
    using namespace support;

    World world(1337u);
    Player player;
    player.setX(0.f);
    player.setY(0.f);
    world.update(0, 0);

    core::SystemScheduler scheduler;
    EnemySystem &enemies = scheduler.add<EnemySystem>();
    BodySystem &bodies = scheduler.add<BodySystem>();
    (void)bodies;
    ParticleSystem &particles = scheduler.add<ParticleSystem>();
    ThrowSystem &throws = scheduler.add<ThrowSystem>();
    ExplosionSystem &explodes = scheduler.add<ExplosionSystem>();
    DeathSystem &deaths = scheduler.add<DeathSystem>();
    DropSystem &drops = scheduler.add<DropSystem>();
    throws.setExplosionSystem(&explodes);
    throws.setParticleSystem(&particles);
    explodes.setParticleSystem(&particles);
    deaths.setDropSystem(&drops);
    deaths.setParticleSystem(&particles);

    // Slime colado no player: orb nasce dentro do raio de coleta.
    enemies.spawn("slime", player.getCenterX(), player.getCenterY());

    // 5 dinamites letais em sequência (fuse curto, dano 100).
    for (int round = 0; round < 5; ++round) {
        auto *t = throws.throwItem(
            {player.getCenterX(), player.getCenterY()}, {0.f, 0.f});
        assert(t != nullptr);
        t->fuse = 0.05f;
        t->damage = 100;

        for (int i = 0; i < 30; ++i) {
            std::vector<ExplosionTarget> targets;
            targets.push_back({{player.getCenterX(), player.getCenterY()},
                               &player.body, nullptr, true, &player, nullptr});
            enemies.forEach([&](Slime &s) {
                if (s.resources.isDead()) return;
                targets.push_back({{s.body.getCenterX(), s.body.getCenterY()},
                                   &s.bodyParts, &s.resources, false,
                                   &s.body, &s.knockbackLock});
            });
            GameContext ctx{&world, &player, nullptr, &enemies,
                            &throws, &explodes, &drops, &targets};
            scheduler.tick(1.f / 30.f, ctx);
            world.update(0, 0);
        }
    }

    assert(enemies.count() == 0u);   // morreu e foi removido
    assert(drops.totalCollected() >= 1); // orb coletada

    std::printf("loop test OK (slimes=0 collected=%d)\n", drops.totalCollected());
    return 0;
}
