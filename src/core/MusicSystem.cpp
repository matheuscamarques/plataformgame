#include "core/MusicSystem.h"
#include <algorithm>

namespace core {

void MusicSystem::registerTrack(int stratum, const MusicTrack& track) {
    Track& t = tracks_[stratum]; // map: constrói o nó in-place, sem mover
    synthesizeTrack(track, t.buffer);
    t.valid = true;
}

void MusicSystem::playStratum(int stratum, float crossfadeSec) {
    if (stratum == currentStratum_) return;
    auto it = tracks_.find(stratum);
    if (it == tracks_.end() || !it->second.valid) return;

    // Primeira música: começa direto.
    if (currentStratum_ == -1) {
        cur_->setBuffer(it->second.buffer);
        cur_->setLoop(true);
        cur_->setVolume(master_ * 100.f);
        if (!paused_) cur_->play();
        currentStratum_ = stratum;
        return;
    }

    // Crossfade: próxima voz entra em 0.
    nxt_->stop();
    nxt_->setBuffer(it->second.buffer);
    nxt_->setLoop(true);
    nxt_->setVolume(0.f);
    if (!paused_) nxt_->play();

    nextStratum_ = stratum;
    crossfade_   = crossfadeDur_ = crossfadeSec;
    nexting_     = true;
}

void MusicSystem::tick(float dt) {
    if (!nexting_) return;

    crossfade_ -= dt;
    const float u = 1.f - std::max(0.f, crossfade_ / crossfadeDur_);
    // u: 0 no começo, 1 no fim.

    cur_->setVolume((1.f - u) * master_ * 100.f);
    nxt_->setVolume(u * master_ * 100.f);

    if (crossfade_ <= 0.f) {
        cur_->stop();
        // Troca de papéis por ponteiro: a voz que entrou vira atual
        // SEM reiniciar (setBuffer + play() recomeçaria do 0).
        std::swap(cur_, nxt_);
        cur_->setVolume(master_ * 100.f);
        currentStratum_ = nextStratum_;
        nextStratum_    = -1;
        nexting_        = false;
    }
}

void MusicSystem::pause() {
    if (paused_) return;
    paused_ = true;
    cur_->pause();
    if (nexting_) nxt_->pause();
}

void MusicSystem::resume() {
    if (!paused_) return;
    paused_ = false;
    // play() após pause() retoma da posição (não reinicia).
    if (currentStratum_ != -1) cur_->play();
    if (nexting_) nxt_->play();
}

} // namespace core
