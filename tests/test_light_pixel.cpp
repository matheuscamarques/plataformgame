#include <cassert>
#include <cmath>
#include <cstdio>

#include "core/DayNightCycle.h"
#include "core/RadialTexture.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Lighting/LightingSystem.h"

// Golden-image lógico (sem GL): invariantes matemáticas sobre pixels e
// parâmetros. Costura/raycast moram em test_light/test_raycast.
namespace {
bool near(float a, float b, float eps = 0.05f) {
    return std::fabs(a - b) < eps;
}
} // namespace

int main() {
    { // PlayerGlowIsSmoothNotFlat (monotônico centro→borda, sem platô)
        auto img = core::makeRadialImage(64, 3.0f);
        const auto c = img.getPixel(32, 32).a;
        const auto m = img.getPixel(48, 32).a;
        const auto e = img.getPixel(60, 32).a;
        assert(c > m && m > e);
        const auto c1 = img.getPixel(35, 32).a; // miolo varia, não é plano
        assert(std::abs(int(c) - int(c1)) < 30);
        assert(img.getPixel(0, 0).a == 0u);
    }
    { // VignetteFadesOutward (centro transparente, cantos pretos)
        auto img = core::makeVignetteImage(64);
        assert(img.getPixel(32, 32).a == 0u); // centro limpo
        assert(img.getPixel(0, 0).a > 200u);  // canto escuro
        assert(img.getPixel(32, 32) == sf::Color(0, 0, 0, 0));
        // Monotônico do centro à borda na horizontal.
        assert(img.getPixel(40, 32).a <= img.getPixel(48, 32).a);
        assert(img.getPixel(48, 32).a <= img.getPixel(60, 32).a);
    }
    { // NightTintDarkerThanDay (meio-dia > meia-noite; tint cru das âncoras)
        core::DayNightCycle noon, night;
        noon.setCycleDuration(24.f);
        night.setCycleDuration(24.f);
        noon.setHour(0.f);
        night.setHour(0.f);
        noon.tick(12.f);
        support::LightingSystem L;
        L.setDayNight(&noon);
        const sf::Color cn = L.lightTint();
        L.setDayNight(&night);
        const sf::Color ck = L.lightTint();
        const float bn = (cn.r + cn.g + cn.b) / 3.f;
        const float bk = (ck.r + ck.g + ck.b) / 3.f;
        assert(bn > bk * 1.2f); // sem × b: razão vem só das âncoras (~1.35×)
    }
    { // TntGlowGrowsWithFuse (mesmo t: perto do boom = maior + quente)
        const float t = 1.0f;
        const auto near = support::tntGlowParams(0.1f, t);
        const auto far = support::tntGlowParams(0.7f, t);
        assert(near.radius > far.radius);
        assert(near.g < far.g); // esquenta: verde cai
        assert(near.a > 0.f && far.a > 0.f);
    }

    { // NightTintFloor (meia-noite: piso 0.55, visível sem ser dia)
        core::DayNightCycle night;
        night.setCycleDuration(24.f);
        night.setHour(0.f); // 00:00 (default agora é 9h)
        support::LightingSystem L;
        L.setDayNight(&night);
        const sf::Color c = L.lightTint();
        const float avg = (c.r + c.g + c.b) / 3.f / 255.f;
        // (150,180,220)×0.55 = (82,99,121) ≈ 0.39
        assert(avg >= 0.35f);
        assert(avg < 0.6f); // mas longe do dia (~0.97)
    }
    { // PlayerGlowInverseToSun (dia fraco, noite forte, sem saturar)
        const float noon = support::LightingSystem::glowAlphaScale(
            support::LightingSystem::darknessOf(1.f, 0.f));
        const float midnight = support::LightingSystem::glowAlphaScale(
            support::LightingSystem::darknessOf(0.f, 0.3f));
        assert(near(noon, 0.35f));
        assert(near(midnight, 0.63f)); // 0.35 + 0.4 × 0.7
        assert(midnight > noon * 1.5f);
    }

    { // RadialTransformLocksConvention (origem=centro, escala=raio/metade)
        using support::LightingSystem;
        const auto t = LightingSystem::radialTransform(128.f, 50.f);
        assert(near(t.origin, 64.f));
        assert(near(t.scale, 50.f / 64.f));
        assert(near(128.f * t.scale, 100.f)); // largura = 2× raio visual
        const auto z = LightingSystem::radialTransform(0.f, 50.f);
        assert(z.scale == 1.f); // sem div0
    }

    std::printf("light pixel test OK\n");
    return 0;
}
