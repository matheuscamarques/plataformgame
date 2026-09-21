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
        ss.notifyHurt();
        assert(ss.count() == 0);
        ss.setAutoHurt(true);
        ss.notifyHurt(); // sem janela: falha limpo
        assert(ss.count() == 0);
    }

    std::puts("screenshot test OK");
    return 0;
}
