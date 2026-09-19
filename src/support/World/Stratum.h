#pragma once

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

} // namespace support
