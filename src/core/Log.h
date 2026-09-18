#pragma once

#include <sstream>
#include <string>

namespace core {

enum class LogLevel { Debug = 0, Info, Warn, Error };

class Log {
public:
    static void setMinLevel(LogLevel level) { minLevel_ = level; }
    static LogLevel minLevel() { return minLevel_; }

    static void write(LogLevel level, const char *category, const std::string &msg);

private:
    static LogLevel minLevel_;
};

} // namespace core

// Uso: LOG_INFO("World", "seed=" << seed);
// A macro monta a mensagem via stream; nada é avaliado se o nível filtra.
#define LOG_DEBUG(cat, msg) do { std::ostringstream _o; _o << msg; ::core::Log::write(::core::LogLevel::Debug, cat, _o.str()); } while (0)
#define LOG_INFO(cat, msg)  do { std::ostringstream _o; _o << msg; ::core::Log::write(::core::LogLevel::Info, cat, _o.str()); } while (0)
#define LOG_WARN(cat, msg)  do { std::ostringstream _o; _o << msg; ::core::Log::write(::core::LogLevel::Warn, cat, _o.str()); } while (0)
#define LOG_ERROR(cat, msg) do { std::ostringstream _o; _o << msg; ::core::Log::write(::core::LogLevel::Error, cat, _o.str()); } while (0)
