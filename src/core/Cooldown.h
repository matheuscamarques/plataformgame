/**
 * @file src/core/Cooldown.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Temporizador genérico com trigger, tick e consulta de prontidão.
 * @details Controla remaining com tick sem negativar e expõe ready, ratio e reset, usado por inimigos, skills e sistemas de combate.
 */

#pragma once
#include <algorithm>

namespace core {

// Timer genérico com trigger/tick/ready/ratio.
// Header-only por ser hot path e trivial.
// Contrato:
//   - Cooldown novo está READY (remaining = 0).
//   - trigger() reinicia a contagem.
//   - tick() nunca deixa remaining negativo.
//   - ratio() = 0 no início, 1 no fim.
class Cooldown {
public:
    Cooldown() = default;
    explicit Cooldown(float duration) : duration_(duration), remaining_(0.f) {}

    void tick(float dt) {
        if (remaining_ <= 0.f) return;
        remaining_ -= dt;
        if (remaining_ < 0.f) remaining_ = 0.f;
    }

    bool ready()   const { return remaining_ <= 0.f; }
    bool running() const { return remaining_ >  0.f; }

    void trigger() { remaining_ = duration_; }

    void trigger(float duration) {
        duration_ = duration;
        remaining_ = duration;
    }

    void reset() { remaining_ = 0.f; }

    // 0 no instante do trigger, 1 quando pronto.
    // Se duration <= 0, sempre retorna 1.
    float ratio() const {
        if (duration_ <= 0.f) return 1.f;
        return 1.f - (remaining_ / duration_);
    }

    float remaining() const { return remaining_; }
    float duration()  const { return duration_;  }

private:
    float duration_  = 0.f;
    float remaining_ = 0.f;
};

} // namespace core
