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
