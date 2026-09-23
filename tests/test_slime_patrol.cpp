/**
 * @file tests/test_slime_patrol.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava patrulha sem aggro com pouso e distância.
 * @details Cobre EnemySystem e World, roda com make test que compila em build/tests/test_slime_patrol.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "world/World.h"
#include "entities/Player/Player.h"
#include "defines.h"

// Enemy patrulha sem player por perto: anda (distância acumulada) e pousa.
int main() {
    using namespace support;

    World world(1337u);
    Player player; // longe: sem aggro
    player.setX(5000.0f);
    player.setY(0.0f);
    GameContext ctx{&world, &player, nullptr};

    EnemySystem enemies;
    enemies.spawn("slime", 100.0f, 0.0f);
    assert(enemies.count() == 1);

    float traveled = 0.0f, prevX = 100.0f;
    for (int t = 0; t < 300; t++) {
        float px = 0.0f, py = 0.0f;
        enemies.forEach([&](Enemy &s) { px = s.body.getX(); py = s.body.getY(); });
        world.update(static_cast<int>(std::floor(px / core::kBlockSize)),
                     static_cast<int>(std::floor(py / core::kBlockSize)));
        enemies.tick(1.0f / 30.0f, ctx);
        enemies.forEach([&](Enemy &s) {
            traveled += std::fabs(s.body.getX() - prevX);
            prevX = s.body.getX();
            assert(s.body.getY() > 0.0f && s.body.getY() < 20000.0f);
        });
    }
    bool grounded = false;
    enemies.forEach([&](Enemy &s) { grounded = s.grounded; });
    std::printf("slime patrol test OK (traveled=%.1f grounded=%d)\n",
                traveled, (int)grounded);
    assert(traveled > 300.0f); // andou de verdade (flips não zeram distância)
    assert(grounded);
    return 0;
}
