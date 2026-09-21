#pragma once
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// SFX procedural: N sons curtos simultâneos por evento (pool 16).
// Coexiste com MusicSystem (2 vozes de fundo) — SFML mixa por baixo.
// NOTA SFML 2.5: SoundBuffer/Sound não copiam nem movem — buffers em
// unique_ptr (map), vozes em unique_ptr<Channel> (vector).
namespace core {

class AudioSystem {
public:
    static constexpr int kChannels = 16;

    // Registra um som (síntese na hora, boot). Chave ausente em play = no-op.
    void registerSound(const std::string& key, std::unique_ptr<sf::SoundBuffer> buf);

    // Toca uma vez, ocupa 1 canal. Pool cheio = descarta (não trava o frame).
    void play(const std::string& key, float volume = 1.f);

    // Com atenuação quadrática por distância 2D. Fora de maxDist = no-op.
    void playAt(const std::string& key, sf::Vector2f pos,
                sf::Vector2f listener, float maxDist = 600.f);

    // Libera canais terminados. Chamado 1× por tick.
    void tick();

    void setMasterVolume(float v) { master_ = v; }
    float masterVolume() const { return master_; }

    // Introspecção p/ teste (sem device: buffers vazios mas presentes).
    bool has(const std::string& key) const;
    std::size_t soundCount() const { return buffers_.size(); }

private:
    struct Channel {
        sf::Sound sound;
        bool      busy = false;
    };

    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> buffers_;
    std::vector<std::unique_ptr<Channel>> channels_;
    float master_ = 0.7f;

    Channel* acquireChannel();
    void     ensureChannels();
};

} // namespace core
