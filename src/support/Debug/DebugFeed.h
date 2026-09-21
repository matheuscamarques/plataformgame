#pragma once
#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace support {

// Feed de debug em 2 partes, dono Game, via ctx.debug (nulo em teste).
// - Números de dano flutuantes (mundo, ttl 0.5s): valida per-part.
// - Log de eventos (canto, cap 5): "quando começou?" em bug caçado.
// Tick com dt fixo no Game::tick. CPU puro: testável headless.
struct DamageNumber {
    std::string text;
    sf::Vector2f pos{0.f, 0.f};
    float ttl = 0.5f;
};

struct DebugFeed {
    static constexpr float kNumberTtl = 0.5f;
    static constexpr std::size_t kLogCap = 5;

    std::vector<DamageNumber> numbers;
    std::vector<std::string> log;

    void pushNumber(const std::string &text, sf::Vector2f pos) {
        numbers.push_back({text, pos, kNumberTtl});
    }
    void pushLog(const std::string &line) {
        log.push_back(line);
        while (log.size() > kLogCap) log.erase(log.begin());
    }
    void tick(float dt) {
        for (auto it = numbers.begin(); it != numbers.end();) {
            it->ttl -= dt;
            if (it->ttl <= 0.f)
                it = numbers.erase(it);
            else
                ++it;
        }
    }
};

} // namespace support
