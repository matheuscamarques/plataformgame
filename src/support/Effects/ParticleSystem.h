/**
 * @file src/support/Effects/ParticleSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara sistema de partículas em dois pools otimizados.
 * @details Define classe System com pools debris e dust, tick, render e emissores para tiles e faíscas, incluída pelo Bootstrapper e usada via GameContext.
 */

#pragma once

#include "core/Vec.h"
#include "core/Pool.h"
#include "core/System.h"
#include "support/GameContext.h"
#include "Particle.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstddef>
#include <vector>

namespace support {

// Partículas em 2 pools: debris (colide, pesado) e dust (leve).
// MVP = retângulos coloridos; sprite vem na fase de arte.
class ParticleSystem : public core::System {
public:
    const char *name() const override { return "ParticleSystem"; }
    int priority() const override { return 350; }

    void tick(float dt, GameContext &ctx) override;
    void render(sf::RenderTarget &target);

    // Quebra de tile: detritos do material + poeira.
    // oreMask: 8 bits, cada bit = 1 sub-bloco de minério.
    void spawnTileBreak(core::Vec2f center, int primaryKind,
                        int secondaryKind, uint8_t oreMask);

    // Impacto de ataque: só faísca + poeira.
    void spawnHitSpark(core::Vec2f point);

    std::size_t activeDebris() const { return debris_.activeCount(); }
    std::size_t activeDust()   const { return dust_.activeCount(); }

    // Limpa tudo (restart da run).
    void clear() { debris_.releaseAll(); dust_.releaseAll(); }

private:
    core::Pool<Particle> debris_{512};
    core::Pool<Particle> dust_{4096};

    void emitDebris(core::Vec2f origin, int kind, int count, float spread);
    void emitDust(core::Vec2f origin, int count, float spread);
};

} // namespace support
