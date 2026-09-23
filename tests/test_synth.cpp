/**
 * @file tests/test_synth.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava síntese com lógica pura sem OpenAL.
 * @details Cobre Music e MusicBank, roda com make test que compila em build/tests/test_synth.
 */

#include <cassert>
#include <cmath>
#include <cstdio>

#include "core/Music.h"
#include "game/MusicBank.h"

// Síntese procedural: só lógica pura (sem sf::SoundBuffer — AlResource
// exige device OpenAL, ausente em headless). Buffers se validam na
// audição (PREVIEW_MUSIC=1), não no CI.
namespace {
bool near(float a, float b) { return std::fabs(a - b) < 1e-3f; }
} // namespace

int main() {
    using namespace core;

    { // EnvelopeADSR (ataque 0, sustain no meio, 0 no fim)
        ADSR e;
        assert(near(envelopeAt(e, 0.f, 1.f), 0.f));
        assert(near(envelopeAt(e, 0.5f, 1.f), e.sustain));
        assert(envelopeAt(e, 1.f, 1.f) < 0.01f);
    }
    { // ScaleNoteA440 (A4 = 440Hz exato)
        assert(near(scaleNote(Scale::Major, 69, 0, 4), 440.f));
        assert(near(scaleNote(Scale::Minor, 69, 0, 4), 440.f));
        // Oitava acima = dobro.
        assert(near(scaleNote(Scale::Major, 69, 0, 5), 880.f));
        // Grau 7+ sobe oitava (coreTheme usa grau 9).
        assert(near(scaleNote(Scale::Major, 69, 7, 4), 880.f));
    }
    { // WaveDeterminism (mesmo seed = mesma sequência de noise)
        uint32_t r1 = 42u, r2 = 42u;
        for (int i = 0; i < 16; ++i)
            assert(sampleWave(Wave::Noise, 0.f, r1)
                   == sampleWave(Wave::Noise, 0.f, r2));
        uint32_t r3 = 7u;
        assert(sampleWave(Wave::Square, 0.25f, r3) == 1.f);
        assert(sampleWave(Wave::Square, 0.75f, r3) == -1.f);
    }
    { // BarsConcat (4 compassos de 16 = 64 steps)
        auto v = bars({0,1}, {2,3}, {4,5}, {6,7});
        assert(v.size() == 8u && v[0] == 0 && v[7] == 7);
    }
    { // AllThemes64Steps (track válida: 64 por pattern, bpm positivo)
        const MusicTrack tracks[] = {
            game::surfaceTheme(), game::shallowCavesTheme(),
            game::fungalWoodsTheme(), game::oldMinesTheme(),
            game::moltenHallsTheme(), game::coreTheme(),
        };
        for (const auto& t : tracks) {
            assert(t.bpm > 0);
            assert(t.bass.size() == 64u);
            assert(t.melody.size() == 64u);
            assert(t.harmony.size() == 64u);
            assert(t.perc.size() == 64u);
        }
        // Temas distintos (não é o mesmo pattern 6×).
        assert(tracks[0].bpm != tracks[5].bpm);
        assert(tracks[0].scale != tracks[3].scale);
    }

    std::printf("synth test OK\n");
    return 0;
}
