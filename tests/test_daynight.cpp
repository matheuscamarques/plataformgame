/**
 * @file tests/test_daynight.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava dia e noite com skyColor e ambiente sem GL.
 * @details Cobre DayNightCycle e Lighting, roda com make test que compila em build/tests/test_daynight.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "core/DayNightCycle.h"
#include "core/RadialTexture.h"
#include "support/Lighting/LightingSystem.h"

// Dia/noite: lógica pura (sem GL — RenderTexture::create/display nunca
// chamados aqui). skyColor/ambientSky só leem cycle_ (amostra mista).
namespace {
bool near(float a, float b, float eps = 0.05f) {
    return std::fabs(a - b) < eps;
}
} // namespace

int main() {
    using namespace core;

    { // NoonIsBrightest (sol 1.0 vs 0.0 da meia-noite)
        auto noon = DayNightCycle::sampleAt(12.f);
        auto midn = DayNightCycle::sampleAt(0.f);
        assert(noon.sunIntensity > midn.sunIntensity);
        assert(near(noon.sunIntensity, 1.f));
        assert(near(midn.sunIntensity, 0.f));
    }
    { // MidnightHasMoon (luz fria mínima)
        auto s = DayNightCycle::sampleAt(0.f);
        assert(near(s.moonIntensity, 0.30f));
    }
    { // DawnIsWarm (tint mais vermelho que azul)
        auto s = DayNightCycle::sampleAt(6.5f);
        assert(s.tintR > s.tintB);
    }
    { // SkyColorCycles (azul pleno vs noite)
        auto noon = DayNightCycle::sampleAt(12.f);
        auto midn = DayNightCycle::sampleAt(0.f);
        assert(noon.skyB > midn.skyB);
    }
    { // HoursWrap (tick além do ciclo volta p/ [0,24))
        DayNightCycle c;
        c.setCycleDuration(10.f);
        c.setHour(0.f);
        c.tick(11.f);
        assert(c.hour() < 24.f && c.hour() >= 0.f);
        assert(near(c.hour(), 2.4f));
    }
    { // BootStartsMorning (nunca breu no boot) + setHour
        DayNightCycle c;
        assert(c.hour() >= 8.9f && c.hour() <= 9.1f);
        c.setHour(15.5f);
        assert(near(c.hour(), 15.5f));
        c.setHour(99.f);
        assert(near(c.hour(), 0.f));
    }
    { // RadialFalloff (centro branco, borda transparente, meio-termo)
        auto img = makeRadialImage(128, 2.0f);
        assert(img.getPixel(64, 64).a > 250u);
        assert(img.getPixel(0, 0).a == 0u);
        const auto mid = img.getPixel(64, 96).a; // d=0.5 → 1-0.25=0.75
        assert(mid > 150u && mid < 230u);
    }
    { // SkyColorFollowsCycle (meio-dia claro, meia-noite escura)
        DayNightCycle c;
        support::LightingSystem L;
        L.setDayNight(&c);
        c.setHour(12.f); // meio-dia
        const sf::Color noon = L.skyColor();
        c.setHour(0.f); // meia-noite
        const sf::Color midn = L.skyColor();
        assert(noon.b > midn.b);
    }
    { // AmbientSkyFollowsCycle (sem checagem de superfície: caverna
        // selada escurece via lightmap grid 0, não via clear color)
        DayNightCycle c;
        support::LightingSystem L;
        L.setDayNight(&c);
        const sf::Color sky = L.skyColor();
        const sf::Color deep = L.ambientSky(1000.f, 100.f);
        assert(deep.r == sky.r && deep.g == sky.g && deep.b == sky.b);
        const sf::Color air = L.ambientSky(50.f, 100.f);
        assert(air.r == sky.r && air.g == sky.g && air.b == sky.b);
    }

    { // SunFadesWithDepth (fade linear: 1 → 0.5 → 0; acima = pleno)
        using support::LightingSystem;
        const float s = 500.f;
        assert(near(LightingSystem::lightAt(s, s, 1.f, 300.f), 1.f));
        assert(near(LightingSystem::lightAt(s + 150.f, s, 1.f, 300.f), 0.5f));
        assert(near(LightingSystem::lightAt(s + 300.f, s, 1.f, 300.f), 0.f));
        assert(near(LightingSystem::lightAt(s - 50.f, s, 1.f, 300.f), 1.f));
        assert(near(LightingSystem::lightAt(s + 1000.f, s, 1.f, 300.f), 0.f));
    }

    std::printf("daynight test OK\n");
    return 0;
}
