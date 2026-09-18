#include "Log.h"

#include <iostream>

namespace core {

LogLevel Log::minLevel_ = LogLevel::Debug;

namespace {
const char *levelName(LogLevel l) {
    switch (l) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO";
        case LogLevel::Warn:  return "WARN";
        case LogLevel::Error: return "ERROR";
    }
    return "?";
}

const char *levelColor(LogLevel l) {
    switch (l) {
        case LogLevel::Debug: return "\033[36m"; // cyan
        case LogLevel::Info:  return "\033[32m"; // green
        case LogLevel::Warn:  return "\033[33m"; // yellow
        case LogLevel::Error: return "\033[31m"; // red
    }
    return "";
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
