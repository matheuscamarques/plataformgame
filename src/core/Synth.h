#pragma once
#include <SFML/Audio/SoundBuffer.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

// Síntese procedural de SFX/música: zero asset de áudio.
// Tudo em mono 44100Hz Int16. Determinístico por seed (testável).
// NOTA SFML 2.5: sf::SoundBuffer não é copiável — as funções
// preenchem um buffer por out-param, nunca retornam por valor.
namespace core {

inline constexpr unsigned kSampleRate = 44100;

struct ADSR {
    float attack  = 0.005f;
    float decay   = 0.05f;
    float sustain = 0.6f;
    float release = 0.08f;
};

inline float envelopeAt(const ADSR& e, float t, float dur) {
    if (t < e.attack) return t / e.attack;
    float d = t - e.attack;
    if (d < e.decay) return 1.f - (1.f - e.sustain) * (d / e.decay);
    float rStart = dur - e.release;
    if (t >= rStart) return e.sustain * (1.f - (t - rStart) / e.release);
    return e.sustain;
}

enum class Wave { Square, Sine, Triangle, Saw, Noise };

inline float sampleWave(Wave w, float phase, uint32_t& rng) {
    switch (w) {
        case Wave::Square:   return std::fmod(phase, 1.f) < 0.5f ? 1.f : -1.f;
        case Wave::Sine:     return std::sin(2.f * 3.14159265f * phase);
        case Wave::Triangle: { float p = std::fmod(phase, 1.f); return 4.f * std::abs(p - 0.5f) - 1.f; }
        case Wave::Saw:      return 2.f * std::fmod(phase, 1.f) - 1.f;
        case Wave::Noise: {
            rng = rng * 1103515245u + 12345u;
            return static_cast<int32_t>(rng) / 2147483648.f;
        }
    }
    return 0.f;
}

struct Note {
    float freq;
    float start;
    float duration;
    float volume = 0.4f;
};

inline void synth(float totalDuration,
                  const std::vector<Note>& notes,
                  Wave wave,
                  sf::SoundBuffer& out,
                  const ADSR& env = {},
                  float globalVol = 0.5f,
                  uint32_t seed = 0x12345678) {
    const unsigned n = static_cast<unsigned>(kSampleRate * totalDuration);
    std::vector<float> mix(n, 0.f);
    uint32_t rng = seed;

    for (const auto& note : notes) {
        if (note.freq <= 0.f) continue;
        const unsigned start = static_cast<unsigned>(note.start * kSampleRate);
        const unsigned len   = static_cast<unsigned>(note.duration * kSampleRate);
        for (unsigned i = 0; i < len && start + i < n; ++i) {
            float t = static_cast<float>(i) / kSampleRate;
            float s = sampleWave(wave, note.freq * t, rng);
            s *= envelopeAt(env, t, note.duration) * note.volume;
            mix[start + i] += s;
        }
    }

    std::vector<sf::Int16> buf(n);
    for (unsigned i = 0; i < n; ++i) {
        float s = std::max(-1.f, std::min(1.f, mix[i] * globalVol));
        buf[i] = static_cast<sf::Int16>(s * 32767.f);
    }
    out.loadFromSamples(buf.data(), n, 1, kSampleRate);
}

// Explosão real em 2 camadas (out-param: SoundBuffer não copia):
// 1. Crack (0-80ms): burst de noise, decay exponencial → o "BANG".
// 2. Rumble (0-dur): senoides graves 55+28Hz, attack 20ms → o "corpo".
// Normaliza pelo pico (sem clipping). duration maior = desabamento.
inline void explosionSound(sf::SoundBuffer& out,
                           float duration = 0.7f,
                           float crackTime = 0.08f,
                           float volume = 0.75f,
                           uint32_t seed = 0xBEEF) {
    const unsigned n = static_cast<unsigned>(kSampleRate * duration);
    std::vector<float> mix(n, 0.f);
    uint32_t rng = seed;

    for (unsigned i = 0; i < n; ++i) {
        const float t = static_cast<float>(i) / kSampleRate;

        float crack = 0.f;
        if (t < crackTime * 2.f) {
            float noise = sampleWave(Wave::Noise, 0.f, rng);
            crack = noise * std::exp(-t / crackTime);
        }

        const float attackEnv = (t < 0.02f) ? (t / 0.02f) : 1.f;
        const float decayEnv  = std::exp(-t * 3.5f);
        const float r1 = std::sin(2.f * 3.14159265f * 55.f * t);
        const float r2 = std::sin(2.f * 3.14159265f * 28.f * t);
        const float rumble = (r1 * 0.6f + r2 * 0.4f) * attackEnv * decayEnv * 0.7f;

        mix[i] = (crack + rumble) * volume;
    }

    float peak = 0.f;
    for (float v : mix) peak = std::max(peak, std::abs(v));
    const float norm = (peak > 0.f) ? (1.f / peak) : 1.f;

    std::vector<sf::Int16> buf(n);
    for (unsigned i = 0; i < n; ++i) {
        float s = std::max(-1.f, std::min(1.f, mix[i] * norm));
        buf[i] = static_cast<sf::Int16>(s * 32767.f);
    }
    out.loadFromSamples(buf.data(), n, 1, kSampleRate);
}

} // namespace core
