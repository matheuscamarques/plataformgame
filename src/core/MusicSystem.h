#pragma once
#include "core/Music.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <map>

namespace core {

// Música procedural por estrato: tracks sintetizadas no boot (RAM,
// zero arquivo), crossfade na troca de estrato.
// NOTA SFML 2.5: sf::SoundBuffer/Sound não copiam nem movem —
// buffers vivem em std::map (nós estáveis, sem realocação) e as
// vozes alternam por ponteiro (swap de ponteiro, nunca de objeto).
class MusicSystem {
public:
    // Registra a track de um estrato. Síntese imediata (~20ms CPU).
    // Chamado no boot, 1× por estrato. Índices esparsos ok.
    void registerTrack(int stratum, const MusicTrack& track);

    // Troca para a track do estrato (crossfade de crossfadeSec).
    // Estrato sem track registrada = ignora (mantém atual).
    void playStratum(int stratum, float crossfadeSec = 0.5f);

    // Avança o crossfade. Chamado 1× por tick.
    void tick(float dt);

    void setMasterVolume(float v) { master_ = v; }
    // Idempotentes: chamar todo tick é seguro (sem restart).
    void pause();
    void resume();

    int currentStratum() const { return currentStratum_; }

private:
    struct Track {
        sf::SoundBuffer buffer;
        bool valid = false;
    };

    std::map<int, Track> tracks_; // nós estáveis: buffer nunca se move
    sf::Sound voiceA_;
    sf::Sound voiceB_;
    sf::Sound *cur_ = &voiceA_; // voz audível (ou principal no fade)
    sf::Sound *nxt_ = &voiceB_; // voz entrando (só durante crossfade)
    int   currentStratum_ = -1;
    int   nextStratum_    = -1;
    float crossfade_      = 0.f;
    float crossfadeDur_   = 0.5f;
    bool  nexting_        = false;
    bool  paused_         = false;
    float master_         = 0.5f;
};

} // namespace core
