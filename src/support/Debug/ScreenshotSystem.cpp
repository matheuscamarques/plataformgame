#include "ScreenshotSystem.h"

#include <algorithm>
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

// Recorte size×size centrado em focus (clampado no framebuffer),
// ampliado zoom× com nearest-neighbor (pixel-art sem blur).
sf::Image ScreenshotSystem::cropZoom(const sf::Image &src,
                                     sf::Vector2f focusPx) {
    const sf::Vector2u size = src.getSize();
    if (size.x == 0 || size.y == 0) return src;
    constexpr unsigned S = ScreenshotSystem::kFocusSize;
    constexpr unsigned Z = ScreenshotSystem::kFocusZoom;
    const int half = static_cast<int>(S) / 2;
    int x0 = static_cast<int>(focusPx.x) - half;
    int y0 = static_cast<int>(focusPx.y) - half;
    const int maxX0 = static_cast<int>(size.x) - static_cast<int>(S);
    const int maxY0 = static_cast<int>(size.y) - static_cast<int>(S);
    x0 = std::max(0, std::min(x0, std::max(0, maxX0)));
    y0 = std::max(0, std::min(y0, std::max(0, maxY0)));

    sf::Image out;
    out.create(S * Z, S * Z);
    for (unsigned y = 0; y < S * Z; ++y) {
        for (unsigned x = 0; x < S * Z; ++x) {
            // Clamp individual: framebuffer menor que o recorte.
            const unsigned sx =
                std::min(static_cast<unsigned>(x0) + x / Z, size.x - 1u);
            const unsigned sy =
                std::min(static_cast<unsigned>(y0) + y / Z, size.y - 1u);
            out.setPixel(x, y, src.getPixel(sx, sy));
        }
    }
    return out;
}

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

    sf::Image img = tex.copyToImage();
    // Foco no personagem: mundo→pixel na worldView armazenada (a view
    // ativa aqui é a default do HUD). Sem worldView, usa a atual.
    if (hasFocus_) {
        const sf::Vector2i px = hasWorldView_
                                    ? window_->mapCoordsToPixel(focus_,
                                                                worldView_)
                                    : window_->mapCoordsToPixel(focus_);
        img = cropZoom(img, sf::Vector2f(px));
    }

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

void ScreenshotSystem::notifyHurt(sf::Vector2f worldCenter) {
    if (!autoHurt_) return;
    setFocus(worldCenter);
    capture("hurt_" + std::to_string(counter_));
}

} // namespace support
