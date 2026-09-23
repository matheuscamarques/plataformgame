/**
 * @file tests/test_behavior_registry.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava slime registrado via macro e desconhecido nulo.
 * @details Cobre BehaviorRegistry com chaves, roda com make test que compila em build/tests/test_behavior_registry.
 */

#include <cassert>
#include <cstdio>
#include "support/Enemies/BehaviorRegistry.h"

// Registry: slime registrado via macro, desconhecido dá null.
int main() {
    using namespace support;

    assert(BehaviorRegistry::instance().has("slime"));

    bool foundSlime = false;
    for (const auto &k : BehaviorRegistry::instance().keys()) {
        if (k == "slime") foundSlime = true;
    }
    assert(foundSlime);

    auto b = BehaviorRegistry::instance().create("slime");
    assert(b != nullptr);

    auto none = BehaviorRegistry::instance().create("dragon");
    assert(none == nullptr);

    std::printf("behavior registry test OK\n");
    return 0;
}
