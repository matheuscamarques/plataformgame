#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "world/World.h"
#include "entities/player/player.h"
#include "defines.h"

// 50 slimes x 600 ticks: sem crash, todos pousados, nenhum caiu do mundo.
int main() {
    using namespace support;

    World world(1337u);
    Player player;
    player.setX(750.0f);
    player.setY(0.0f);
    GameContext ctx{&world, &player, nullptr};

    EnemySystem enemies;
    for (int i = 0; i < 50; i++) {
        enemies.spawn("slime", static_cast<float>(i * 30), 0.0f);
    }
    assert(enemies.count() == 50);

    for (int t = 0; t < 600; t++) {
        // mundo segue cada slime (como o Game segue o player)
        enemies.forEach([&](Enemy &s) {
            world.update(static_cast<int>(std::floor(s.body.getX() / BLOCK_SIZE)),
                         static_cast<int>(std::floor(s.body.getY() / BLOCK_SIZE)));
        });
        enemies.tick(1.0f / 30.0f, ctx);
    }

    int grounded = 0;
    enemies.forEach([&](Enemy &s) {
        assert(s.body.getY() > 0.0f && s.body.getY() < 20000.0f);
        if (s.grounded) grounded++;
    });
    std::printf("slime stress OK (grounded=%d/50)\n", grounded);
    assert(enemies.count() == 50);
    assert(grounded >= 40); // a maioria pousada (pulos dessincronizam alguns)
    return 0;
}
