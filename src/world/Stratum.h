/**
 * @file src/world/Stratum.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Divide a vertical em 11 estratos com nomes e cores.
 * @details Define STRATUM_TOP, stratumAt O1, checkpointTy e stratumBg por faixa de ty, usado pela Generation, BlockRegistry e render de fundo.
 */

#pragma once
#include <cstdint>

namespace support {

// Estrato = faixa vertical por ty. Separado de Biome (horizontal,
// climático, só superfície). 10 estratos de ~1200 + superfície e fundo.
inline constexpr int STRATUM_COUNT = 11;
// Tetos: estrato i cobre [TOP[i-1], TOP[i]) (estrato 0 começa em ty<0).
inline constexpr int STRATUM_TOP[STRATUM_COUNT] = {
    200, 1400, 2600, 3800, 5000, 6200, 7400, 8600, 9800, 11000, 12000
};

inline const char *stratumName(int s) {
    switch (s) {
        case 0:  return "Superficie";
        case 1:  return "Cavernas Rasas";
        case 2:  return "Bosque Fungico";
        case 3:  return "Veios de Prata";
        case 4:  return "Minas Antigas";
        case 5:  return "Cristais";
        case 6:  return "Saloes Derretidos";
        case 7:  return "Vazio Primordial";
        case 8:  return "Coracao Partido";
        case 9:  return "Nucleo";
        case 10: return "O Fundo";
        default: return "?";
    }
}

// ty negativo (céu) = 0; ty além do fundo = 10 (clamp).
// O(1) aritmético: passo uniforme de 1200 a partir de 200.
// Idêntico ao loop sobre STRATUM_TOP (teste test_strata trava).
inline int stratumAt(int ty) {
    if (ty < 200) return 0;
    int s = 1 + (ty - 200) / 1200;
    return s > 10 ? 10 : s;
}

// Checkpoint do estrato = ty de entrada (topo). Estrato 0 = superfície.
inline int checkpointTy(int s) {
    if (s <= 0) return 0;
    if (s >= STRATUM_COUNT) s = STRATUM_COUNT - 1;
    return STRATUM_TOP[s - 1];
}

// Cor chapada de fundo por estrato (1 draw no render; Scaler futuro
// reusa a mesma cor nas extensões). S0 = céu atual (superfície intacta).
struct StratumBg { uint8_t r, g, b; };
inline StratumBg stratumBg(int s) {
    switch (s) {
        case 0:  return {135, 206, 235};
        case 1:  return {60, 40, 30};
        case 2:  return {40, 50, 35};
        case 3:  return {45, 45, 55};
        case 4:  return {55, 40, 30};
        case 5:  return {40, 55, 65};
        case 6:  return {70, 25, 20};
        case 7:  return {25, 20, 35};
        case 8:  return {55, 15, 25};
        case 9:  return {80, 70, 40};
        case 10: return {15, 15, 20};
        default: return {20, 20, 30};
    }
}

// Metade da faixa de transição nas fronteiras (tiles p/ cada lado).
// Queda livre (~15 tiles/s): 60 = ~4s de gradiente por fronteira.
inline constexpr float kStratumBlend = 60.f;

inline StratumBg lerpBg(StratumBg a, StratumBg b, float t) {
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;
    const auto mix = [](uint8_t x, uint8_t y, float t) {
        return static_cast<uint8_t>(x + (y - x) * t + 0.5f);
    };
    return {mix(a.r, b.r, t), mix(a.g, b.g, t), mix(a.b, b.b, t)};
}

// Cor com gradiente: chapada no miolo, interpola ±blend nas fronteiras.
// Contínua por construção (na fronteira vale a cor de cima dos 2 lados).
inline StratumBg stratumBgSmooth(float ty) {
    const int tyi = ty < 0.f ? static_cast<int>(ty) - 1 : static_cast<int>(ty);
    const int s = stratumAt(tyi);
    // Borda inferior do estrato s (ty onde ele começa; s=0 não tem).
    if (s >= 1) {
        const float ty0 = 200.f + (s - 1) * 1200.f;
        const float d = ty - ty0; // >= 0 dentro do estrato
        if (d >= 0.f && d < kStratumBlend)
            return lerpBg(stratumBg(s - 1), stratumBg(s), d / kStratumBlend);
    }
    // Borda superior (ty onde o próximo começa; s=10 não tem).
    if (s <= 9) {
        const float ty1 = 200.f + s * 1200.f;
        const float d = ty1 - ty; // > 0 dentro do estrato
        if (d > 0.f && d < kStratumBlend)
            return lerpBg(stratumBg(s), stratumBg(s + 1), 1.f - d / kStratumBlend);
    }
    return stratumBg(s);
}

} // namespace support
