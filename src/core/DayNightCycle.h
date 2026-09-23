/**
 * @file src/core/DayNightCycle.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Relógio de ciclo dia e noite com interpolação de cores e luz.
 * @details Avança horas com tick, expõe hour e sample com sol e lua, usado por Game e LightingSystem para céu e iluminação.
 */

#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace core {

// Ciclo dia/noite. `hours` vai de 0 a 24 e wrapa.
// Interpola entre 12 fases-âncora (midnight ... midnight).
// Lógica pura, zero SFML (testável headless).
//
// Regras de design:
//   - Sol é 0 durante a noite; lua dá luz fria mínima.
//   - Cores de céu mudam (azul-noite → laranja-amanhecer → azul-dia).
//   - Subterrâneo não vê ciclo — só o sol afeta a superfície.
struct DayNightSample {
    float sunIntensity;   // 0..1 — multiplica o gradiente do sol
    float moonIntensity;  // 0..1 — luz fria ambiente (0.0 a 0.3)
    uint8_t skyR, skyG, skyB;      // cor base do céu (superfície)
    uint8_t tintR, tintG, tintB;   // tonalidade do sol/lua (aplicada ao vertex)
};

class DayNightCycle {
public:
    // Duração real de um ciclo completo (segundos).
    // 600s = 10 min. Ajustável.
    void setCycleDuration(float seconds) { cycleDuration_ = seconds; }

    // Hora inicial (p/ teste e boot diurno). Clamp em [0,24).
    void setHour(float h) {
        if (h < 0.f) h = 0.f;
        if (h >= 24.f) h = 0.f;
        t_ = (h / 24.f) * cycleDuration_;
    }

    // Avança o relógio. Chamado por frame.
    void tick(float dt) {
        t_ += dt;
        while (t_ >= cycleDuration_) t_ -= cycleDuration_;
        if (t_ < 0.f) t_ = 0.f;
    }

    // Hora atual em [0, 24).
    float hour() const {
        return (t_ / cycleDuration_) * 24.f;
    }

    // Amostra interpolada pra hora atual.
    DayNightSample sample() const {
        return sampleAt(hour());
    }

    // Amostra pra uma hora arbitrária (útil pra teste).
    static DayNightSample sampleAt(float hour);

private:
    float t_             = (9.f / 24.f) * 600.f; // boot 9h (manhã, não breu)
    float cycleDuration_ = 600.f; // 10 min = 1 dia
};

// ─── Implementação inline (header-only como Cooldown) ────────────

namespace detail {

struct Anchor {
    float    hour;
    float    sun;
    float    moon;
    uint8_t  skyR, skyG, skyB;
    uint8_t  tintR, tintG, tintB;
};

// 12 âncoras. Ordem cronológica. Não precisa ser simétrico.
inline constexpr Anchor kAnchors[] = {
    // hour  sun   moon   sky (R,G,B)         tint (R,G,B)
    {  0.0f, 0.00f, 0.30f, 5, 10, 24,    150,180,220},  // midnight
    {  4.0f, 0.00f, 0.25f, 15, 20, 45,    150,180,220},  // pre-dawn
    {  5.5f, 0.20f, 0.10f, 60, 50, 80,    220,150,180},  // first light
    {  6.5f, 0.55f, 0.00f, 200,100, 60,    255,180,140},  // dawn
    {  7.0f, 0.75f, 0.00f, 130,140,180,    255,220,190},  // mid-morning (quebra o marrom dawn→morning)
    {  8.0f, 0.90f, 0.00f, 120,170,220,    255,230,200},  // morning
    { 12.0f, 1.00f, 0.00f, 135,195,235,    255,250,240},  // noon
    { 16.0f, 0.95f, 0.00f, 130,180,220,    255,235,210},  // afternoon
    { 18.0f, 0.60f, 0.00f, 200,100, 60,    255,180,140},  // dusk
    { 20.0f, 0.15f, 0.10f, 60, 50, 80,    200,150,180},  // twilight
    { 22.0f, 0.00f, 0.28f, 15, 20, 45,    150,180,220},  // night
    { 24.0f, 0.00f, 0.30f, 5, 10, 24,    150,180,220},  // midnight (wrap)
};
inline constexpr int kAnchorCount = sizeof(kAnchors) / sizeof(kAnchors[0]);

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline uint8_t lerpU8(uint8_t a, uint8_t b, float t) {
    return static_cast<uint8_t>(a + (b - a) * t);
}

} // namespace detail

inline DayNightSample DayNightCycle::sampleAt(float hour) {
    hour = std::fmod(hour, 24.f);
    if (hour < 0.f) hour += 24.f;

    // Acha o segmento entre âncoras.
    for (int i = 0; i < detail::kAnchorCount - 1; ++i) {
        const auto& A = detail::kAnchors[i];
        const auto& B = detail::kAnchors[i + 1];
        if (hour < A.hour || hour > B.hour) continue;
        if (B.hour <= A.hour) continue;
        const float t = (hour - A.hour) / (B.hour - A.hour);

        DayNightSample s;
        s.sunIntensity  = detail::lerp(A.sun,  B.sun,  t);
        s.moonIntensity = detail::lerp(A.moon, B.moon, t);
        s.skyR = detail::lerpU8(A.skyR, B.skyR, t);
        s.skyG = detail::lerpU8(A.skyG, B.skyG, t);
        s.skyB = detail::lerpU8(A.skyB, B.skyB, t);
        s.tintR = detail::lerpU8(A.tintR, B.tintR, t);
        s.tintG = detail::lerpU8(A.tintG, B.tintG, t);
        s.tintB = detail::lerpU8(A.tintB, B.tintB, t);
        return s;
    }

    // Fallback (não deveria chegar)
    return {0.f, 0.3f, 5, 10, 24, 150, 180, 220};
}

} // namespace core
