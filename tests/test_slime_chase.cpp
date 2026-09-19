#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/EnemySystem.h"
#include "support/GameContext.h"
#include "support/World/World.h"
#include "entities/player/player.h"
#include "defines.h"

// Slime persegue player próximo: fase 1 pousa longe, fase 2 aproxima.
int main() {
    using namespace support;

    World world(1337u);
    Player player;
    player.setX(5000.0f); // fase 1: longe, só pousa
    player.setY(0.0f);
    GameContext ctx{&world, &player, nullptr};

    EnemySystem enemies;
    enemies.spawn("slime", 100.0f, 0.0f);

    auto step = [&](int n) {
        for (int t = 0; t < n; t++) {
            float px = 0.0f, py = 0.0f;
            enemies.forEach([&](Slime &s) { px = s.body.getX(); py = s.body.getY(); });
            world.update(static_cast<int>(std::floor(px / BLOCK_SIZE)),
                         static_cast<int>(std::floor(py / BLOCK_SIZE)));
            enemies.tick(1.0f / 30.0f, ctx);
        }
    };
    auto slimePos = [&]() {
        float x = 0.0f, y = 0.0f;
        enemies.forEach([&](Slime &s) { x = s.body.getX(); y = s.body.getY(); });
        return std::make_pair(x, y);
    };

    step(200); // pousa
    auto [sx, sy] = slimePos();
    assert(sy > 500.0f); // caiu no chão de verdade

    // fase 2: player aparece 150px ao lado, na mesma altura
    player.setX(sx + 150.0f);
    player.setY(sy);
    float d0 = 150.0f;
    step(200);
    auto [sx1, sy1] = slimePos();
    float d1 = std::fabs(sx1 - player.getX());
    std::printf("slime chase test OK (d=%.1f -> %.1f)\n", d0, d1);
    assert(d1 < d0 * 0.5f);
    return 0;
}
