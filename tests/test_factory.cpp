#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/EnemySystem.h"
#include "defines.h"

// Factory cria slime 40x30 verde nomeado; kind inválido dá null.
int main() {
    using namespace support;

    auto s = Factory::spawnEnemy("slime", 100.0f, 0.0f);
    assert(s != nullptr);
    assert(s->body.getName() == SLIME);
    assert(std::fabs(s->body.getW() - 40.0f) < 0.001f);
    assert(std::fabs(s->body.getH() - 30.0f) < 0.001f);
    assert(std::fabs(s->body.getX() - 100.0f) < 0.001f);
    assert(s->ai != nullptr);

    auto none = Factory::spawnEnemy("dragon", 0.0f, 0.0f);
    assert(none == nullptr);

    std::printf("factory test OK\n");
    return 0;
}
