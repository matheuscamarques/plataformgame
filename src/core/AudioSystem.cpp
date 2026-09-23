/**
 * @file src/core/AudioSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementação do mixer de efeitos sonoros com pool de 16 canais simultâneos.
 * @details Registra buffers com registerSound, toca com play e playAt com atenuação quadrática e libera vozes em tick, usado por Game e SoundBank.
 */

#include "core/AudioSystem.h"
#include <cmath>

namespace core {

void AudioSystem::registerSound(const std::string& key, std::unique_ptr<sf::SoundBuffer> buf) {
    if (!buf) return;
    buffers_[key] = std::move(buf);
}

void AudioSystem::ensureChannels() {
    if (!channels_.empty()) return;
    for (int i = 0; i < kChannels; ++i)
        channels_.push_back(std::make_unique<Channel>());
}

AudioSystem::Channel* AudioSystem::acquireChannel() {
    ensureChannels();
    for (auto& c : channels_)
        if (!c->busy) return c.get();
    return nullptr; // todos ocupados → descarta o som
}

void AudioSystem::play(const std::string& key, float volume) {
    auto it = buffers_.find(key);
    if (it == buffers_.end()) return;
    Channel* ch = acquireChannel();
    if (!ch) return;

    ch->sound.setBuffer(*it->second); // referência: sem cópia
    ch->sound.setVolume(volume * master_ * 100.f);
    ch->sound.play();
    ch->busy = true;
}

void AudioSystem::playAt(const std::string& key, sf::Vector2f pos,
                         sf::Vector2f listener, float maxDist) {
    const float dx = pos.x - listener.x;
    const float dy = pos.y - listener.y;
    const float d  = std::sqrt(dx * dx + dy * dy);
    if (d > maxDist) return;

    const float att = 1.f - (d / maxDist);
    play(key, att * att); // atenuação quadrática
}

void AudioSystem::tick() {
    for (auto& c : channels_)
        if (c->busy && c->sound.getStatus() == sf::Sound::Stopped)
            c->busy = false;
}

bool AudioSystem::has(const std::string& key) const {
    return buffers_.find(key) != buffers_.end();
}

} // namespace core
