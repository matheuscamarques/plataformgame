#pragma once
#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace support {

// Feed de debug em 3 partes, dono Game, via ctx.debug (nulo em teste).
// - Números de dano flutuantes (mundo, ttl 0.5s): valida per-part.
// - Log de eventos em memória (canto, cap 5): "quando começou?".
// - Log em arquivo (logs/debug.log, F9): o que o agente lê depois —
//   sem arquivo, dado de runtime não sai do jogo.
// Tick com dt fixo no Game::tick (gated por !paused no caller).
// CPU puro (fstream, sem GL): testável headless.
struct DamageNumber {
    std::string text;
    sf::Vector2f pos{0.f, 0.f};
    float ttl = 0.5f;
};

struct DebugFeed {
    static constexpr float kNumberTtl = 0.5f;
    static constexpr std::size_t kLogCap = 5;
    static constexpr const char *kLogPath = "logs/debug.log";

    std::vector<DamageNumber> numbers;
    std::vector<std::string> log;

    void pushNumber(const std::string &text, sf::Vector2f pos) {
        numbers.push_back({text, pos, kNumberTtl});
    }
    void pushLog(const std::string &line) {
        log.push_back(line);
        while (log.size() > kLogCap) log.erase(log.begin());
        // Arquivo carrega a linha com stamp de tick (determinístico,
        // correlaciona com replay). Flush imediato: eventos são esparsos.
        if (fileEnabled_) {
            ensureFile();
            if (file_.is_open()) {
                file_ << "[t=" << tick_ << "] " << line << "\n" << std::flush;
            }
        }
    }
    void tick(float dt) {
        ++tick_;
        for (auto it = numbers.begin(); it != numbers.end();) {
            it->ttl -= dt;
            if (it->ttl <= 0.f)
                it = numbers.erase(it);
            else
                ++it;
        }
    }

    // Arquivo: 1 run = 1 arquivo (trunca no 1º push), sem acumular.
    // setFileEnabled(false) fecha. Headless sem enable: zero arquivo.
    // setLogPath: isola teste em /tmp (nunca encosta em logs/ real).
    void setLogPath(const std::string &path) { path_ = path; }
    void setFileEnabled(bool on) {
        fileEnabled_ = on;
        if (!on && file_.is_open()) file_.close();
    }
    bool fileEnabled() const { return fileEnabled_; }
    int tickCount() const { return tick_; }

private:
    int tick_ = 0;
    bool fileEnabled_ = false;
    std::string path_ = kLogPath;
    std::ofstream file_;

    void ensureFile() {
        if (file_.is_open()) return;
        if (path_ == kLogPath) {
            std::error_code ec;
            std::filesystem::create_directories("logs", ec);
        }
        file_.open(path_, std::ios::out | std::ios::trunc);
    }
};

} // namespace support
