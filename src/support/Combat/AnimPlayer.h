/**
 * @file src/support/Combat/AnimPlayer.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Player genérico de AnimClip: tempo, goto e eventos.
 * @details Fase C do plano de profundidade. Dois modos, sem dois
 * relógios brigando: locomotion usa tick(dt) em clips time-driven;
 * melee usa gotoFrame() sincronizado às transições de updateMelee
 * (relógio único = meleeTimer, frost-slow de graça). Eventos de nível
 * (Hitbox) valem enquanto o frame está ativo; de borda, ao entrar.
 * Puro, sem SFML/GL: testável headless.
 */

#pragma once

#include <algorithm>
#include <cstdint>

#include "support/Combat/AnimClip.h"

namespace support {

class AnimPlayer {
public:
    // Toca o clip. Mesmo clip sem force = continua (locomotion não
    // reinicia a cada tick); force ou clip novo = frame 0 + eventos.
    void play(const AnimClip &clip, bool forceRestart = false) {
        if (!forceRestart && clip_ == &clip) return;
        clip_ = &clip;
        frameIdx_ = 0;
        timer_ = 0.f;
        finished_ = false;
        pending_ = edgeOf(currentIndex());
    }

    // Avança por tempo (só frames com duration > 0; state-driven
    // param aqui — usam gotoFrame).
    void tick(float dt) {
        if (!clip_ || finished_ || dt <= 0.f) return;
        if (clip_->frames[frameIdx_].duration <= 0.f) return;
        timer_ += dt;
        while (!finished_ &&
               clip_->frames[frameIdx_].duration > 0.f &&
               timer_ >= clip_->frames[frameIdx_].duration) {
            timer_ -= clip_->frames[frameIdx_].duration;
            advance();
        }
    }

    // Salto state-driven (melee): emite eventos de borda ao entrar.
    // Mesmo frame = sem re-emissão (Active dura N ticks, 1 evento).
    void gotoFrame(int i) {
        if (!clip_ || clip_->count <= 0) return;
        i = std::max(0, std::min(i, clip_->count - 1));
        if (i == frameIdx_) return;
        frameIdx_ = i;
        timer_ = 0.f;
        pending_ |= edgeOf(i);
        if (!clip_->loop && i == clip_->count - 1) finished_ = true;
    }

    SpriteFrameId currentFrame() const {
        if (!clip_ || clip_->count <= 0) return SpriteFrameId::None;
        return clip_->frames[frameIdx_].frame;
    }

    // Nível: eventos do frame atual filtrados pela máscara de nível.
    uint32_t liveEvents() const {
        if (!clip_ || clip_->count <= 0) return 0;
        return clip_->frames[frameIdx_].events & AnimEvent::kLevelMask;
    }

    // Borda: eventos acumulados desde o último consumo (drena).
    uint32_t consumeEvents() {
        const uint32_t e = pending_;
        pending_ = 0;
        return e;
    }

    bool playing() const { return clip_ != nullptr; }
    bool finished() const { return finished_; }
    int frameIndex() const { return frameIdx_; }

private:
    uint32_t edgeOf(int i) const {
        if (!clip_ || clip_->count <= 0) return 0;
        return clip_->frames[i].events & ~AnimEvent::kLevelMask;
    }
    int currentIndex() const { return frameIdx_; }

    void advance() {
        int next = frameIdx_ + 1;
        if (next >= clip_->count) {
            if (clip_->loop) {
                next = 0;
            } else {
                finished_ = true; // segura o último frame
                timer_ = 0.f;
                return;
            }
        }
        frameIdx_ = next;
        pending_ |= edgeOf(next);
        if (!clip_->loop && next == clip_->count - 1) finished_ = true;
    }

    const AnimClip *clip_ = nullptr;
    int frameIdx_ = 0;
    float timer_ = 0.f;
    bool finished_ = false;
    uint32_t pending_ = 0;
};

} // namespace support
