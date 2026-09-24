#include <cassert>
#include <cmath>
#include <cstdio>

#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"

// Separação (EnemySystem::separate): pares sobrepostos se empurram,
// meio a meio, pelo menor eixo. Sem mundo (física integra e retorna).
int main() {
    using namespace support;

    auto dist = [](EnemySystem& enemies) {
        float ax = 0.f, ay = 0.f, bx = 0.f, by = 0.f;
        bool first = true;
        enemies.forEach([&](Enemy& s) {
            if (first) {
                ax = s.body.getCenterX();
                ay = s.body.getCenterY();
                first = false;
            } else {
                bx = s.body.getCenterX();
                by = s.body.getCenterY();
            }
        });
        const float dx = ax - bx, dy = ay - by;
        return std::sqrt(dx * dx + dy * dy);
    };

    { // PushApart (mesmo spawn: depois de ticks, sem overlap em X)
        EnemySystem enemies;
        enemies.spawn("slime", 100.f, 0.f);
        enemies.spawn("slime", 100.f, 0.f);
        assert(dist(enemies) == 0.f);
        GameContext ctx{};
        for (int i = 0; i < 3; ++i) enemies.tick(1.f / 30.f, ctx);
        assert(dist(enemies) > 20.f); // 40x30: encostados (~30px)
    }
    { // KnockbackRespected (lock rodando: não separa)
        EnemySystem enemies;
        enemies.spawn("slime", 100.f, 0.f);
        enemies.spawn("slime", 100.f, 0.f);
        enemies.forEach(
            [](Enemy& s) { s.knockbackLock.trigger(5.f); });
        GameContext ctx{};
        for (int i = 0; i < 3; ++i) enemies.tick(1.f / 30.f, ctx);
        assert(dist(enemies) == 0.f); // travados juntos
    }

    std::printf("separation test OK\n");
    return 0;
}
