/**
 * @file tests/test_screenshot.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava screenshot sem pasta com flags e dedup.
 * @details Cobre ScreenshotSystem sem janela, roda com make test que compila em build/tests/test_screenshot.
 */

#include <cassert>
#include <cstdio>

#include "support/Debug/ScreenshotSystem.h"

// ScreenshotSystem sem janela (headless): sem efeito colateral,
// sem pasta criada, flags e dedup operáveis.
int main() {
    using namespace support;

    { // DefaultsOff (nada automático ao nascer)
        ScreenshotSystem ss;
        assert(!ss.autoMelee() && !ss.autoHurt());
        assert(ss.count() == 0);
    }
    { // CaptureWithoutWindowFailsClean (sem GL, sem pasta, sem throw)
        ScreenshotSystem ss;
        assert(!ss.capture("manual"));
        assert(ss.count() == 0);
    }
    { // TogglesFlip (F11/F10 ligam e desligam)
        ScreenshotSystem ss;
        ss.setAutoMelee(true);
        ss.setAutoHurt(true);
        assert(ss.autoMelee() && ss.autoHurt());
        ss.setAutoMelee(false);
        assert(!ss.autoMelee() && ss.autoHurt());
    }
    { // AutoMeleeDedupsPerSwingWithoutWindow (2x mesmo id = 1 tentativa)
        ScreenshotSystem ss;
        ss.setAutoMelee(true);
        ss.maybeCaptureMelee(7);
        ss.maybeCaptureMelee(7); // dedup: 2ª não tenta
        ss.maybeCaptureMelee(8);
        // Sem janela nada grava; o que importa: sem crash, sem pasta.
        assert(ss.count() == 0);
    }
    { // NotifyHurtRespectsFlag (desligado = no-op)
        ScreenshotSystem ss;
        ss.notifyHurt({100.f, 200.f});
        assert(ss.count() == 0);
        ss.setAutoHurt(true);
        ss.notifyHurt({100.f, 200.f}); // sem janela: falha limpo
        assert(ss.count() == 0);
    }
    { // CropZoomCentersAndScales (CPU puro, sem GL)
        sf::Image src;
        src.create(800, 800, sf::Color::Black);
        src.setPixel(400, 300, sf::Color::Red);
        const sf::Image out =
            ScreenshotSystem::cropZoom(src, {400.f, 300.f});
        assert(out.getSize().x == ScreenshotSystem::kFocusSize *
                                     ScreenshotSystem::kFocusZoom);
        assert(out.getSize().y == ScreenshotSystem::kFocusSize *
                                     ScreenshotSystem::kFocusZoom);
        // Foco no centro do recorte ampliado.
        const unsigned c = ScreenshotSystem::kFocusSize *
                           ScreenshotSystem::kFocusZoom / 2;
        assert(out.getPixel(c, c) == sf::Color::Red);
        // Canto superior esquerdo = origem do recorte (400-120, 300-120).
        assert(out.getPixel(0, 0) == sf::Color::Black);
    }
    { // CropZoomClampsAtEdges (foco no canto não lê fora)
        sf::Image src;
        src.create(800, 800, sf::Color::Green);
        const sf::Image out =
            ScreenshotSystem::cropZoom(src, {0.f, 0.f});
        assert(out.getSize().x == 720u);
        assert(out.getPixel(0, 0) == sf::Color::Green);
        assert(out.getPixel(719, 719) == sf::Color::Green);
    }

    std::puts("screenshot test OK");
    return 0;
}
