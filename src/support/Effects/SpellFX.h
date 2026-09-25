/**
 * @file src/support/Effects/SpellFX.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Efeitos visuais de spells (instância, sem lógica de dano).
 * @details Cobre o que rastro/halo/faísca não cobrem: burst espiral da cura, aura orbital do frost_weapon e fogo Doom da fireball. Visual puro (dano mora em Throw/Melee); instância no Game (sem global, sem poluir teste), incluído por Renderer e App.
 */

#pragma once

#include <cstdint>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>

#include "core/Vec.h"

namespace support {

// Instância: um por Game (dono limpa no restart junto dos sistemas).
class SpellFX {
public:
    // Rajada verde em espiral (cura). Uma dose, 0.6s.
    void burstHeal(core::Vec2f center);
    // Fogo Doom 16x16 (chama procedural atrás da fireball).
    void drawFire(core::Vec2f center, float intensity,
                  sf::RenderTarget &target);
    // Cristais orbitando a arma (frost_weapon ativo).
    void drawFrostAura(core::Vec2f weaponPos, sf::RenderTarget &target);

    void tick(float dt);
    void render(sf::RenderTarget &target);
    void clear();

    // Observável p/ teste headless (sem GL: só contadores e calor).
    std::size_t activeSparks() const { return sparks_.size(); }
    float heatAt(int x, int y) const;

private:
    struct Spark {
        core::Vec2f pos{0.f, 0.f};
        core::Vec2f vel{0.f, 0.f};
        float age = 0.f;
        float life = 0.6f;
        uint32_t color = 0;
    };

    // Doom-style: calor sobe com jitter e esfria (16x16 fixo).
    static constexpr int kFireW = 16;
    static constexpr int kFireH = 16;
    float heat_[kFireW * kFireH] = {0.f};
    float fireSeed_ = 0.f; // base reacesa a cada tick de chama

    std::vector<Spark> sparks_;
    float auraT_ = 0.f;
};

} // namespace support
