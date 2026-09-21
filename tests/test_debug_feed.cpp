#include <cassert>
#include <cstdio>

#include "support/Debug/DebugFeed.h"

// DebugFeed CPU puro (sem GL, sem jogo): números expiram, log capa.
int main() {
    using namespace support;

    { // PushNumberKeepsTtl (0.5s ao nascer)
        DebugFeed f;
        f.pushNumber("-16", {100.f, 200.f});
        assert(f.numbers.size() == 1u);
        assert(f.numbers[0].text == "-16");
        assert(f.numbers[0].ttl == DebugFeed::kNumberTtl);
    }
    { // TickExpiresNumbers (0.5s depois, some)
        DebugFeed f;
        f.pushNumber("-8", {0.f, 0.f});
        f.tick(0.25f);
        assert(f.numbers.size() == 1u);
        f.tick(0.25f);
        assert(f.numbers.empty());
    }
    { // LogCapsAt5 (6º push derruba o 1º)
        DebugFeed f;
        for (int i = 0; i < 7; ++i)
            f.pushLog("e" + std::to_string(i));
        assert(f.log.size() == DebugFeed::kLogCap);
        assert(f.log.front() == "e2");
        assert(f.log.back() == "e6");
    }

    std::puts("debug feed test OK");
    return 0;
}
