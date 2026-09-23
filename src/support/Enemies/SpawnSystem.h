/**
 * @file src/support/Enemies/SpawnSystem.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara spawn contínuo com caps global e por estrato.
 * @details Define classe SpawnSystem prioridade 410 com budgetForStratum e intervalos, registrado no loop via GameContext.
 */

#pragma once
#include <cstddef>

#include "core/System.h"

namespace support {

struct GameContext;

// Spawns contínuos: candidatos vêm do ArchetypeRegistry (faixa +
// maxAlive por tipo, peso ponderado). Adicionar inimigo = append em
// Behaviors.cpp. Budget total por estrato continua valendo.
class SpawnSystem : public core::System {
public:
    const char *name() const override { return "SpawnSystem"; }
    int priority() const override { return 410; }

    void tick(float dt, GameContext &ctx) override;

    // Budget total de vivos por estrato (densidade cai com profundidade).
    static int budgetForStratum(int s);
    static constexpr std::size_t kGlobalCap = 100;

private:
    float timer_ = 0.f;
    static constexpr float kInterval = 0.5f;
    static constexpr float kSpawnMin = 600.f;  // px do player (fora da view)
    static constexpr float kSpawnMax = 1000.f; // dentro do carregado
    static constexpr float kDespawnRadius = 1600.f; // longe some (economia)
    static constexpr float kGroundScan = 800.f; // px para baixo até o chão
};

} // namespace support
