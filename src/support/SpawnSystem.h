#pragma once
#include <cstddef>

#include "../core/System.h"

namespace support {

struct GameContext;

// Spawns contínuos ao descer: mantém o estrato populado até o budget,
// despawna longe (economia). Slime por enquanto; DwarfAI entra no S3+
// quando existir (tabela pronta). Priority 410 (banda spawn, após drops).
class SpawnSystem : public core::System {
public:
    const char *name() const override { return "SpawnSystem"; }
    int priority() const override { return 410; }

    void tick(float dt, GameContext &ctx) override;

    // Budget de vivos por estrato (só slime hoje).
    static int budgetForStratum(int s);
    static constexpr std::size_t kGlobalCap = 10;

private:
    float timer_ = 0.f;
    static constexpr float kInterval = 1.5f;
    static constexpr float kSpawnMin = 600.f;  // px do player (fora da view)
    static constexpr float kSpawnMax = 1000.f; // dentro do carregado
    static constexpr float kDespawnRadius = 1600.f;
    static constexpr float kGroundScan = 800.f; // px para baixo até o chão
};

} // namespace support
