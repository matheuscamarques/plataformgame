#include <cassert>
#include <cmath>
#include <cstdio>

#include "core/Celestial.h"

// Matemática dos astros (pura, headless). Estilo do repo: main + assert.
namespace {
bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}
} // namespace

int main() {
    using namespace core;
    { // Sol visível ao meio-dia, fora à meia-noite
        assert(near(sunAngle(12.f), 90.f));
        assert(near(arcVisibility(sunAngle(12.f)), 1.f));
        assert(arcVisibility(sunAngle(0.f)) == 0.f);
        assert(arcVisibility(sunAngle(18.f)) == 0.f); // limite: rampa zera
        assert(arcVisibility(sunAngle(6.f)) == 0.f);
    }
    { // Lua cheia à meia-noite, fora ao meio-dia
        assert(near(moonAngle(0.f), 90.f));
        assert(arcVisibility(moonAngle(0.f)) > 0.9f);
        assert(arcVisibility(moonAngle(12.f)) == 0.f);
    }
    { // Rampa de visibilidade (fade nas bordas, 1 no meio)
        assert(arcVisibility(90.f) == 1.f);
        assert(arcVisibility(9.f) > 0.f && arcVisibility(9.f) < 1.f);
        assert(arcVisibility(-5.f) == 0.f && arcVisibility(200.f) == 0.f);
    }
    { // Estrelas determinísticas e contidas na faixa superior
        assert(near(starOffsetX(3), starOffsetX(3)));
        for (int i = 0; i < 200; ++i) {
            const float x = starOffsetX(i), y = starOffsetY(i);
            assert(x >= 0.f && x < 1.f);
            assert(y >= 0.f && y < 0.55f);
        }
        // Espalha (não concentra num canto): amostra cobre os quadrantes.
        bool q[4] = {false, false, false, false};
        for (int i = 0; i < 80; ++i)
            q[(starOffsetX(i) > 0.5f ? 1 : 0) + (starOffsetY(i) > 0.275f ? 2 : 0)] = true;
        assert(q[0] && q[1] && q[2] && q[3]);
    }

    std::printf("celestial test OK\n");
    return 0;
}
