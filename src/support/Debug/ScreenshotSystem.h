#pragma once
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

namespace support {

// Screenshots de debug em ./screenshots/ (PNG), sempre com foco no
// personagem: recorte centrado nele + zoom inteiro (pixel-art nítida).
// - F12: captura manual.
// - F11: toggle auto em melee Active (1 captura por swing).
// - F10: toggle auto em hurt (hurt() já faz gate por i-frame, sem
//   dedup extra necessário).
// Custo zero quando desligado; sem janela (teste headless),
// capture() retorna false sem efeito colateral.
class ScreenshotSystem {
public:
    // Recorte do foco: janela size×size px, ampliada zoom×.
    // 240px @ 3x = 720px: player (30x50) + espada + hitboxes próximas.
    static constexpr unsigned kFocusSize = 240;
    static constexpr unsigned kFocusZoom = 3;

    void setWindow(sf::RenderWindow *w) { window_ = w; }
    void setAutoMelee(bool on) { autoMelee_ = on; }
    void setAutoHurt(bool on) { autoHurt_ = on; }
    bool autoMelee() const { return autoMelee_; }
    bool autoHurt() const { return autoHurt_; }
    int count() const { return counter_; }

    // Centro do recorte, em coords de MUNDO. A conversão p/ pixel
    // acontece na captura (view mais atual). CPU-side: testável
    // headless via cropZoom.
    void setFocus(sf::Vector2f worldCenter) {
        focus_ = worldCenter;
        hasFocus_ = true;
    }
    void clearFocus() { hasFocus_ = false; }

    // Recorte size×size centrado no foco + zoom nearest-neighbor.
    // Estático p/ teste headless (sf::Image é CPU, sem GL).
    static sf::Image cropZoom(const sf::Image &src, sf::Vector2f focusPx);

    // Manual: sempre grava. Sem janela, retorna false.
    bool capture(const std::string &tag);

    // Auto melee: só grava com flag ligada; dedup por swing.
    void maybeCaptureMelee(int swingId);
    // Auto hurt: só grava com flag ligada (i-frame já deduplica).
    void notifyHurt(sf::Vector2f worldCenter);

private:
    sf::RenderWindow *window_ = nullptr;

    bool autoMelee_ = false;
    bool autoHurt_ = false;

    int lastMeleeSwing_ = -1;

    int counter_ = 0;

    sf::Vector2f focus_{0.f, 0.f};
    bool hasFocus_ = false;
};

} // namespace support
