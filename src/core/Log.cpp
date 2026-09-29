/**
 * @file src/core/Log.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementação do logger colorido com níveis e destinos separados.
 * @details Escreve Info e Debug em stdout e Warn e Error em stderr com flush, chamado pelas macros LOG via Log.h.
 */

#include "Log.h"

#include <iostream>

namespace core {

LogLevel Log::minLevel_ = LogLevel::Debug;

namespace {
// Debug=0..Error=3 (ordem do enum LogLevel, sem COUNT: tabela local).
constexpr int kLevelCount = 4;

const char *levelName(LogLevel l) {
    static constexpr const char *kNames[] = {"DEBUG", "INFO", "WARN",
                                             "ERROR"};
    static_assert(sizeof(kNames) / sizeof(kNames[0]) ==
                      static_cast<std::size_t>(kLevelCount),
                  "levelName: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(l);
    if (i < 0 || i >= kLevelCount) return "?";
    return kNames[i];
}

const char *levelColor(LogLevel l) {
    static constexpr const char *kColors[] = {
        "\033[36m", // Debug: cyan
        "\033[32m", // Info: green
        "\033[33m", // Warn: yellow
        "\033[31m", // Error: red
    };
    static_assert(sizeof(kColors) / sizeof(kColors[0]) ==
                      static_cast<std::size_t>(kLevelCount),
                  "levelColor: tabela fora de sincronia com o enum");
    const int i = static_cast<int>(l);
    if (i < 0 || i >= kLevelCount) return "";
    return kColors[i];
}
} // namespace

void Log::write(LogLevel level, const char *category, const std::string &msg) {
    if (level < minLevel_) return;
    // Regra do flush: Info/Debug vão para stdout bufferizado (barato);
    // Warn/Error vão para stderr com endl (visível mesmo sob crash/kill).
    if (level >= LogLevel::Warn) {
        std::cerr << levelColor(level) << "[" << levelName(level) << "]"
                  << "\033[0m[" << category << "] " << msg << std::endl;
    } else {
        std::cout << levelColor(level) << "[" << levelName(level) << "]"
                  << "\033[0m[" << category << "] " << msg << '\n';
    }
}

} // namespace core
