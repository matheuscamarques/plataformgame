#include "ScreenshotSystem.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "core/Log.h"

namespace support {

namespace {

std::string timestamp() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto t = system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return ss.str();
}

} // namespace

bool ScreenshotSystem::capture(const std::string &tag) {
    if (!window_) return false;

    // Cria a pasta na primeira captura (nunca em teste headless:
    // sem janela, o return acima já saiu).
    static bool dirReady = false;
    if (!dirReady) {
        std::error_code ec;
        std::filesystem::create_directories("./screenshots", ec);
        dirReady = true;
    }

    const sf::Vector2u size = window_->getSize();
    if (size.x == 0 || size.y == 0) return false;

    sf::Texture tex;
    tex.create(size.x, size.y);
    tex.update(*window_); // copia o framebuffer atual

    const sf::Image img = tex.copyToImage();

    std::ostringstream name;
    name << "./screenshots/" << std::setw(4) << std::setfill('0')
         << counter_++ << "_" << timestamp() << "_" << tag << ".png";

    const bool ok = img.saveToFile(name.str());
    if (ok) LOG_INFO("Screenshot", name.str());
    return ok;
}

void ScreenshotSystem::maybeCaptureMelee(int swingId) {
    if (!autoMelee_) return;
    if (swingId == lastMeleeSwing_) return; // 1 captura por swing
    lastMeleeSwing_ = swingId;
    capture("melee_" + std::to_string(swingId));
}

void ScreenshotSystem::notifyHurt() {
    if (!autoHurt_) return;
    capture("hurt_" + std::to_string(counter_));
}

} // namespace support
