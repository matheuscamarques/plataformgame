/**
 * @file src/core/Time.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Cabeçalho do relógio global com delta e acumulador de ticks.
 * @details Declara Time estático com beginFrame, deltaTime e consumeTicks, incluído pelo loop principal em Game.
 */

#pragma once

#include <cstdint>

namespace core {

// Relógio do loop: delta por frame + acumulador de fixed step.
// Tudo em segundos (float). Nunca use float para tempo absoluto fora daqui.
class Time {
public:
    // Passo fixo padrão 1/60. O jogo atual roda a 30 TPS e trava
    // o próprio valor via setFixedStep() — o default não muda ninguém.
    static void setFixedStep(float seconds) { fixedStep_ = seconds; }
    static float fixedStep() { return fixedStep_; }

    // Topo do frame: mede o delta desde o beginFrame anterior.
    static void beginFrame();

    // Segundos desde o último beginFrame (clampado anti-espiral).
    static float deltaTime() { return delta_; }
    // Segundos desde o primeiro beginFrame.
    static float elapsed() { return elapsed_; }
    static uint64_t frameCount() { return frames_; }

    // Consome o acumulador: quantos ticks fixos rodar neste frame.
    static int consumeTicks();

private:
    static float fixedStep_;
    static float delta_;
    static float elapsed_;
    static float accumulator_;
    static uint64_t frames_;
};

} // namespace core
