#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

namespace support {

// Screenshots de debug em ./screenshots/ (PNG).
// - F12: captura manual.
// - F11: toggle auto em melee Active (1 captura por swing).
// - F10: toggle auto em hurt (hurt() já faz gate por i-frame, sem
//   dedup extra necessário).
// Custo zero quando desligado; sem janela (teste headless),
// capture() retorna false sem efeito colateral.
class ScreenshotSystem {
public:
    void setWindow(sf::RenderWindow *w) { window_ = w; }
    void setAutoMelee(bool on) { autoMelee_ = on; }
    void setAutoHurt(bool on) { autoHurt_ = on; }
    bool autoMelee() const { return autoMelee_; }
    bool autoHurt() const { return autoHurt_; }
    int count() const { return counter_; }

    // Manual: sempre grava. Sem janela, retorna false.
    bool capture(const std::string &tag);

    // Auto melee: só grava com flag ligada; dedup por swing.
    void maybeCaptureMelee(int swingId);
    // Auto hurt: só grava com flag ligada (i-frame já deduplica).
    void notifyHurt();

private:
    sf::RenderWindow *window_ = nullptr;

    bool autoMelee_ = false;
    bool autoHurt_ = false;

    int lastMeleeSwing_ = -1;

    int counter_ = 0;
};

} // namespace support
