#pragma once
#include <cstddef>
#include <string>

#include "../core/System.h"

namespace support {

struct GameContext;

// Spawns contínuos ao descer: mantém o estrato populado até o budget,
// despawna longe (economia). Slime em todo lugar; anão S3+ (15/30%).
// Priority 410 (banda spawn, após drops).
class SpawnSystem : public core::System {
public:
    const char *name() const override { return "SpawnSystem"; }
    int priority() const override { return 410; }

    void tick(float dt, GameContext &ctx) override;

    // Budget de vivos por estrato (densidade cai com profundidade).
    static int budgetForStratum(int s);
    static constexpr std::size_t kGlobalCap = 100;

    // Sorteio puro p/ teste: roll em [0,1) → "dwarf" ou "slime".
    // S0-S2: só slime. S3-S4: 15% anão. S5+: 30% anão.
    static std::string pickKind(int stratum, float roll);

    // Cap de 1 anão vivo (O(n) por janela de spawn via ai->name();
    // aceitável até ~100 vivos — Q1).
    static bool hasLiveDwarf(class EnemySystem &enemies);

private:
    float timer_ = 0.f;
    static constexpr float kInterval = 0.5f;
    static constexpr float kSpawnMin = 600.f;  // px do player (fora da view)
    static constexpr float kSpawnMax = 1000.f; // dentro do carregado
    static constexpr float kDespawnRadius = 1600.f; // longe some (economia)
    static constexpr float kGroundScan = 800.f; // px para baixo até o chão
};

} // namespace support
