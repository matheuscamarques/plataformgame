#include "SpawnSystem.h"

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "core/Random.h"
#include "defines.h"
#include "entities/Player/Player.h"
#include "EnemySystem.h"
#include "EnemyArchetype.h"
#include "support/Debug/DebugFeed.h"
#include "support/GameContext.h"
#include "world/Stratum.h"
#include "world/World.h"

namespace support {

int SpawnSystem::budgetForStratum(int s) {
    // Densidade cai com a profundidade. Tabela real (era stub 100).
    switch (s) {
        case 0: return 3;
        case 1: return 4;
        case 2: return 4;
        case 3: return 3;
        case 4: return 3;
        case 5: return 2;
        default: return 2;
    }
}

namespace {
// Vivos do kind (compara Behavior::kind(); O(n) por janela).
int countAlive(EnemySystem &enemies, core::EntityKind kind) {
    int n = 0;
    enemies.forEach([&](Enemy &s) {
        if (!s.resources.isDead() && s.ai && s.ai->kind() == kind) ++n;
    });
    return n;
}
} // namespace

void SpawnSystem::tick(float dt, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;

    // Economia primeiro: longe some, todo tick (barato, sem timer).
    const float px = p->getCenterX(), py = p->getCenterY();
    ctx.enemies->despawnFar(px, py, kDespawnRadius);

    timer_ += dt;
    if (timer_ < kInterval) return;
    timer_ = 0.f;

    if (ctx.enemies->count() >= kGlobalCap) return;
    const int ty = static_cast<int>(std::floor(py / core::kBlockSize));
    const int stratum = stratumAt(ty);
    if (static_cast<std::size_t>(ctx.enemies->count()) >=
        static_cast<std::size_t>(budgetForStratum(stratum)))
        return;

    // Candidatos data-driven: faixa + maxAlive por archetype, peso
    // ponderado. Sorteio único por janela.
    struct Cand {
        std::string key;
        float weight;
    };
    std::vector<Cand> cands;
    float totalW = 0.f;
    for (const auto &key : ArchetypeRegistry::instance().keys()) {
        const EnemyArchetype *a = ArchetypeRegistry::instance().find(key);
        if (!a) continue;
        if (stratum < a->minStratum || stratum > a->maxStratum) continue;
        if (countAlive(*ctx.enemies, a->kind) >= a->maxAlive) continue;
        cands.push_back({key, a->spawnWeight});
        totalW += a->spawnWeight;
    }
    if (cands.empty()) return;
    float r = core::randRange(0.f, totalW);
    std::string kind = cands.front().key;
    for (auto &c : cands) {
        r -= c.weight;
        if (r <= 0.f) {
            kind = c.key;
            break;
        }
    }

    // Posição: anel 600-1000px ao lado, depois chão para baixo.
    // Sem mundo (teste): spawna no ar na altura do player.
    const float side = core::randRange(0.f, 1.f) < 0.5f ? -1.f : 1.f;
    float sx = px + side * core::randRange(kSpawnMin, kSpawnMax);
    float sy = py + core::randRange(-100.f, 100.f);
    if (ctx.world) {
        int tx = static_cast<int>(std::floor(sx / core::kBlockSize));
        int tyy = static_cast<int>(std::floor(sy / core::kBlockSize));
        const int top = tyy;
        // Desce até achar topo sólido (pés no chão, não dentro da rocha).
        while (tyy * core::kBlockSize < top * core::kBlockSize + kGroundScan) {
            if (ctx.world->isSolid(tx, tyy + 1) && !ctx.world->isSolid(tx, tyy))
                break;
            ++tyy;
        }
        if (tyy * core::kBlockSize >= top * core::kBlockSize + kGroundScan) return; // sem chão
        sy = static_cast<float>(tyy) * core::kBlockSize;
    }
    ctx.enemies->spawn(kind, sx, sy, &ctx);
    if (ctx.debug)
        ctx.debug->pushLog("spawn " + kind + " S" +
                           std::to_string(stratum));
}

} // namespace support
