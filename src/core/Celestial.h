/**
 * @file src/core/Celestial.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Funções puras que calculam posição do sol, lua e estrelas por hora.
 * @details Calcula ângulos com sol às 6h, lua defasada 12h e offsets determinísticos de estrelas, usado pelo Renderer para desenhar o céu.
 */

#pragma once
#include <cmath>
#include <cstdint>

namespace core {

// Astros em coords de mundo (testável headless; desenho no Renderer).
// Sol nasce às 6h (0°), culmina ao meio-dia (90°), se põe às 18h (180°).
// Fora do arco: invisível (abaixo do horizonte).
// Lua defasada 12h (cheia à meia-noite). Fase lunar (8 dias) não é
// rastreada — sem contador de dias no Game; lua sempre cheia por ora.
inline float sunAngle(float hour) {
    return (hour - 6.f) / 12.f * 180.f;
}

// Lua 12h defasada do sol.
inline float moonAngle(float hour) {
    float h = hour + 12.f;
    while (h >= 24.f) h -= 24.f;
    return (h - 6.f) / 12.f * 180.f;
}

// Fade nas bordas do arco: 0 fora, rampa 0→1 nas pontas, 1 no meio.
inline float arcVisibility(float angle) {
    if (angle < 0.f || angle > 180.f) return 0.f;
    const float t = angle / 180.f;
    const float ramp = t < 0.5f ? t : 1.f - t;
    const float v = ramp * 6.f;
    return v > 1.f ? 1.f : v;
}

// Estrela i: offset determinístico em [0,1)×[0,0.55) (faixa superior).
// Dois irracionais independentes (proporção áurea e plástica): espalham
// sem padrão nem correlação — 137.5/79.3 davam 2 valores de x, e 0.618/
// 0.382 somam 1.0 (diagonal). 100% reproduzível.
inline float starOffsetX(int i) {
    const float v = std::fmod(static_cast<float>(i) * 0.61803398875f, 1.f);
    return v < 0.f ? v + 1.f : v;
}
inline float starOffsetY(int i) {
    const float v = std::fmod(static_cast<float>(i) * 0.75487766625f, 1.f) * 0.55f;
    return v < 0.f ? v + 0.55f : v;
}

} // namespace core
