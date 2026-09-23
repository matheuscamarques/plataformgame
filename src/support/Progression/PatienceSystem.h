/**
 * @file src/support/Progression/PatienceSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara estado puro de paciência e vínculo do anão.
 * @details Define struct PatienceState com valor, avisos, cooldown e estágio passivo mais funções puras de mina, pepita e traição, incluída por DwarfAI sem ciclo de includes.
 */

#pragma once
#include "core/Cooldown.h"

namespace support {

// Pavio curto + vínculo: estado puro, sem Enemy (sem ciclo de include).
// Fiação: DwarfAI tica; ThrowSystem entrega pepita; mineração fura
// quando o player tiver mineração (sem BlockBreaker ainda).
struct PatienceState {
    float value = 100.f;
    float max = 100.f;
    int warningLevel = 0;
    core::Cooldown regenDelay{3.0f};

    int passiveStage = 0; // 0=hostil 1=suspeita 2=reconhecimento 3=passivo
    float trustTimer = 0.f;
    bool betrayed = false;
};

int patienceOnMine(PatienceState &p, bool isOre, float distance);
int patienceOnNugget(PatienceState &p);
void patienceTick(PatienceState &p, float dt);
bool patienceShouldBetray(const PatienceState &p);

} // namespace support
