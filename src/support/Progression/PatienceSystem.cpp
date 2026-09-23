/**
 * @file src/support/Progression/PatienceSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Atualiza paciência do anão por mineração e pepitas.
 * @details Implementa patienceOnMine com raio de 4 tiles e dano por minério, patienceOnNugget por estágio e tick com regen, chamada por DwarfAI e ThrowSystem via PatienceSystem.h.
 */

#include "PatienceSystem.h"
#include "core/Config.h"

#include <algorithm>

namespace support {

namespace {
constexpr float kMineRadius = core::kBlockSize * 4.f; // 4 tiles (200px com bloco 50)
}

int patienceOnMine(PatienceState &p, bool isOre, float distance) {
    if (distance > kMineRadius) return p.warningLevel;
    if (p.passiveStage < 2) return p.warningLevel; // só após reconhecer
    p.value -= isOre ? 25.f : 8.f;
    p.regenDelay.trigger();
    p.value = std::max(0.f, p.value);
    if (p.value <= 70.f && p.warningLevel < 1) p.warningLevel = 1;
    if (p.value <= 40.f && p.warningLevel < 2) p.warningLevel = 2;
    if (p.value <= 15.f && p.warningLevel < 3) p.warningLevel = 3;
    if (p.value <= 0.f) {
        p.betrayed = true;
        p.passiveStage = 0;
    }
    return p.warningLevel;
}

int patienceOnNugget(PatienceState &p) {
    switch (p.passiveStage) {
        case 0: p.passiveStage = 1; break;
        case 1: p.passiveStage = 2; p.trustTimer = 0.f; break;
        case 2: p.trustTimer += 20.f; break;
        default: break;
    }
    p.regenDelay.trigger();
    return p.passiveStage;
}

void patienceTick(PatienceState &p, float dt) {
    p.regenDelay.tick(dt);
    if (p.passiveStage == 2) {
        p.trustTimer += dt;
        if (p.trustTimer >= 60.f) p.passiveStage = 3;
    }
    if (p.regenDelay.ready() && p.value < p.max) {
        float cap = p.max;
        if (p.warningLevel >= 3) cap = 40.f;
        else if (p.warningLevel >= 2) cap = 70.f;
        p.value = std::min(cap, p.value + dt * 1.f);
    }
}

bool patienceShouldBetray(const PatienceState &p) {
    return p.betrayed;
}

} // namespace support
