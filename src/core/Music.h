#pragma once
#include "core/Synth.h"
#include <initializer_list>
#include <vector>

// Composição numérica: tracks como dados (escala/root/BPM/patterns de
// 64 steps), sintetizadas em runtime para sf::SoundBuffer em RAM.
// Zero arquivo de áudio. Ajuste musical = 1 número (BPM, root, grau).
namespace core {

enum class Scale { Major, Minor, Dorian, Phrygian, PentatonicMinor };

inline const std::vector<int>& scaleSteps(Scale s) {
    static const std::vector<int> major  = {0,2,4,5,7,9,11};
    static const std::vector<int> minor  = {0,2,3,5,7,8,10};
    static const std::vector<int> dorian = {0,2,3,5,7,9,10};
    static const std::vector<int> phryg  = {0,1,3,5,7,8,10};
    static const std::vector<int> penta  = {0,3,5,7,10};
    switch (s) {
        case Scale::Major:           return major;
        case Scale::Minor:           return minor;
        case Scale::Dorian:          return dorian;
        case Scale::Phrygian:        return phryg;
        case Scale::PentatonicMinor: return penta;
    }
    return minor;
}

// rootMidi: MIDI da tônica em oitava 4 (C4=60, A4=69).
// degree: grau da escala (pode passar de 7 → sobe oitava).
// octave: oitava absoluta (2=grave, 4=médio, 5=agudo).
inline float scaleNote(Scale s, int rootMidi, int degree, int octave) {
    const auto& steps = scaleSteps(s);
    const int sz = static_cast<int>(steps.size());
    int idx = degree % sz;
    int oct = degree / sz;
    if (idx < 0) { idx += sz; --oct; }
    int midi = rootMidi + steps[idx] + oct * 12 + (octave - 4) * 12;
    return 440.f * std::pow(2.f, (midi - 69) / 12.f);
}

struct MusicTrack {
    Scale scale;
    int   rootMidi;
    int   bpm;
    std::vector<int> bass;     // 64 steps; -1 = silêncio
    std::vector<int> melody;
    std::vector<int> harmony;
    std::vector<int> perc;     // 0/1 = sem hit / hit
};

inline void synthesizeTrack(const MusicTrack& t, sf::SoundBuffer& out) {
    const float secPerBeat = 60.f / t.bpm;
    const float secPerStep = secPerBeat / 4.f;   // 16 steps por compasso
    const int   totalSteps = 64;
    const float duration   = totalSteps * secPerStep;

    std::vector<Note> bass, mel, harm, perc;

    auto emit = [&](std::vector<Note>& dst, const std::vector<int>& pat,
                    int octave, float vol, float stepHold) {
        for (int i = 0; i < totalSteps && i < (int)pat.size(); ++i) {
            if (pat[i] < 0) continue;
            const float f = scaleNote(t.scale, t.rootMidi, pat[i], octave);
            dst.push_back({f, i * secPerStep, secPerStep * stepHold, vol});
        }
    };

    emit(bass,   t.bass,    3, 0.55f, 0.95f);
    emit(mel,    t.melody,  5, 0.42f, 0.85f);
    emit(harm,   t.harmony, 4, 0.28f, 1.40f);

    for (int i = 0; i < totalSteps && i < (int)t.perc.size(); ++i) {
        if (t.perc[i] <= 0) continue;
        perc.push_back({1.f, i * secPerStep, secPerStep * 0.4f, 0.35f});
    }

    sf::SoundBuffer b1, b2, b3, b4;
    synth(duration, bass, Wave::Triangle, b1, {0.01f,0.1f,0.7f,0.1f}, 0.6f);
    synth(duration, mel,  Wave::Square,   b2, {0.005f,0.05f,0.5f,0.1f}, 0.4f);
    synth(duration, harm, Wave::Sine,     b3, {0.05f,0.15f,0.7f,0.2f}, 0.3f);
    synth(duration, perc, Wave::Noise,    b4, {0.001f,0.02f,0.1f,0.03f}, 0.5f);

    // Mix por média (÷4), não por clamp: 4 camadas cheias clipavam.
    const unsigned n = b1.getSampleCount();
    std::vector<sf::Int16> mix(n);
    const auto *s1 = b1.getSamples();
    const auto *s2 = b2.getSamples();
    const auto *s3 = b3.getSamples();
    const auto *s4 = b4.getSamples();
    for (unsigned i = 0; i < n; ++i) {
        int v = (static_cast<int>(s1[i]) + s2[i] + s3[i] + s4[i]) / 4;
        v = std::max(-32767, std::min(32767, v));
        mix[i] = static_cast<sf::Int16>(v);
    }
    out.loadFromSamples(mix.data(), n, 1, kSampleRate);
}

// Helper: concatena 4 compassos de 16 steps cada (total 64).
inline std::vector<int> bars(std::initializer_list<int> b1, std::initializer_list<int> b2,
                             std::initializer_list<int> b3, std::initializer_list<int> b4) {
    std::vector<int> out;
    out.reserve(64);
    out.insert(out.end(), b1.begin(), b1.end());
    out.insert(out.end(), b2.begin(), b2.end());
    out.insert(out.end(), b3.begin(), b3.end());
    out.insert(out.end(), b4.begin(), b4.end());
    return out;
}

} // namespace core
