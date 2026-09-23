/**
 * @file tests/test_debug_feed.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava números que expiram e log com teto sem GL.
 * @details Cobre DebugFeed puro de CPU, roda com make test que compila em build/tests/test_debug_feed.
 */

#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

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

    { // FileDisabledCreatesNothing (sem enable: zero arquivo)
        const std::string tmp = "/tmp/test_debug_feed_off.log";
        std::remove(tmp.c_str());
        DebugFeed f;
        f.setLogPath(tmp);
        f.pushLog("ghost");
        assert(!std::filesystem::exists(tmp));
    }
    { // FileRoundTrip (linhas com stamp de tick, flush imediato)
        const std::string tmp = "/tmp/test_debug_feed.log";
        std::remove(tmp.c_str());
        {
            DebugFeed f;
            assert(!f.fileEnabled());
            f.setLogPath(tmp);
            f.setFileEnabled(true);
            f.pushLog("melee slime head -16"); // t=0
            assert(f.tickCount() == 0);
            f.tick(1.f / 30.f); // t=1
            f.pushLog("spawn dwarf S3"); // t=1
            assert(f.tickCount() == 1);
            f.setFileEnabled(false); // fecha (flush já foi imediato)
        }
        std::ifstream in(tmp);
        assert(in.is_open());
        std::string l1, l2, extra;
        std::getline(in, l1);
        std::getline(in, l2);
        assert(l1 == "[t=0] melee slime head -16");
        assert(l2 == "[t=1] spawn dwarf S3");
        assert(!std::getline(in, extra));
        in.close();
        std::remove(tmp.c_str());
    }

    std::puts("debug feed test OK");
    return 0;
}
