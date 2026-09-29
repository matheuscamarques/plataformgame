/**
 * @file tests/test_ik.cpp
 * @brief Teste headless da Fase B: solveIK 2-bone + FK do Limb.
 * @details Puro, sem GL. Roda com make test em build/tests/test_ik.
 */
#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/Combat/IK.h"
#include "support/Combat/Limb.h"

static bool near(float a, float b, float eps) {
    return std::fabs(a - b) <= eps;
}

int main() {
    using support::Limb;
    using support::solveIK;

    { // EsticadoNoAlcance (alvo a 20px, braço 10+10: dobra ~0)
        float up = 999.f, lo = 999.f;
        solveIK({0.f, 0.f}, {20.f, 0.f}, 10.f, 10.f, true, up, lo);
        assert(std::isfinite(up) && std::isfinite(lo));
        assert(near(up, 0.f, 5.f));
        assert(near(lo, 0.f, 5.f));
    }
    { // ClampaSemNaN (alvo além do alcance: estica na direção)
        float up = 999.f, lo = 999.f;
        solveIK({0.f, 0.f}, {100.f, 0.f}, 10.f, 10.f, true, up, lo);
        assert(std::isfinite(up) && std::isfinite(lo));
        assert(near(up, 0.f, 5.f));
        assert(near(lo, 0.f, 5.f));
    }
    { // DobraProLadoCerto (mesmo alvo, bend flipa o sinal)
        float upF = 0.f, loF = 0.f, upB = 0.f, loB = 0.f;
        solveIK({0.f, 0.f}, {10.f, 0.f}, 10.f, 10.f, true, upF, loF);
        solveIK({0.f, 0.f}, {10.f, 0.f}, 10.f, 10.f, false, upB, loB);
        assert(near(loF, 120.f, 1.f));
        assert(near(loB, -120.f, 1.f));
        assert(!near(upF, upB, 1e-3f)); // ombro compensa pro outro lado
    }
    { // AlvoNoOmbroNaoQuebra (dist 0: repouso para baixo)
        float up = 999.f, lo = 999.f;
        solveIK({5.f, 5.f}, {5.f, 5.f}, 10.f, 10.f, true, up, lo);
        assert(near(up, 90.f, 1e-3f) && near(lo, 0.f, 1e-3f));
    }
    { // ComprimentoInvalidoNaoQuebra
        float up = 999.f, lo = 999.f;
        solveIK({0.f, 0.f}, {10.f, 0.f}, 0.f, 10.f, true, up, lo);
        assert(up == 0.f && lo == 0.f);
    }
    { // FKTocaOAlvo (roundtrip IK→Limb: mão chega no alvo)
        float up = 0.f, lo = 0.f;
        solveIK({0.f, 0.f}, {10.f, 0.f}, 10.f, 10.f, true, up, lo);
        Limb limb;
        limb.upper.h = 10; limb.upper.pivot = {0.f, 0.f};
        limb.lower.h = 10; limb.lower.pivot = {0.f, 0.f};
        limb.angleUpper = up;
        limb.angleLower = lo;
        const core::Vec2f hand = limb.handPos({0.f, 0.f});
        assert(near(hand.x, 10.f, 1e-3f) && near(hand.y, 0.f, 1e-3f));
        const core::Vec2f elbow = limb.elbowPos({0.f, 0.f});
        assert(near(elbow.x, 5.f, 1e-3f));
    }

    std::printf("ik test OK\n");
    return 0;
}
