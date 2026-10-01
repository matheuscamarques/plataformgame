/**
 * @file src/support/Enemies/SpawnSystem.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Controla budget por estrato, spawn ponderado e despawn distante.
 * @details Implementa tick com janela de spawn e budgetForStratum usando ArchetypeRegistry, chamado pelo loop com World e player.
 */

#include "SpawnSystem.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "core/Random.h"
#include "core/DayNightCycle.h"
#include "defines.h"
#include "entities/Player/Player.h"
#include "EnemySystem.h"
#include "EnemyArchetype.h"
#include "support/Debug/DebugFeed.h"
#include "support/GameContext.h"
#include "world/Generation.h"
#include "world/Stratum.h"
#include "world/World.h"
#include "core/Coords.h"

namespace support {

int SpawnSystem::rollPackSize(const EnemyArchetype &a, float roll01) {
    if (a.packMax <= a.packMin) return a.packMin;
    const float r = std::clamp(roll01, 0.f, 0.9999f);
    return a.packMin +
           static_cast<int>(r * (a.packMax - a.packMin + 1));
}

int SpawnSystem::budgetForStratum(int s) {
    // Horda grande: até 100 inimigos por estrato.
    switch (s) {
        case 0: return 100;
        case 1: return 100;
        case 2: return 100;
        case 3: return 100;
        case 4: return 100;
        case 5: return 100;
        default: return 100;
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
    const int ty = core::worldToTile({px, py}).y;
    const int stratum = stratumAt(ty);

    // Determina orçamento efetivo considerando dia/noite no estrato 0
    int effectiveBudget = budgetForStratum(stratum);
    if (stratum == 0 && ctx.dayNight) {
        const auto sample = ctx.dayNight->sample();
        bool isNight = sample.sunIntensity < 0.20f;
        if (!isNight) {
            // Dia: muito poucos spawns no estrato 0
            effectiveBudget = 2;
        }
    }
    // Tarô cósmico: Diabo/Mundo engordam o orçamento; Sol de dia,
    // Lua de noite; lua de sangue dobra a noite. Teto global manda.
    if (ctx.player) {
        const auto &fx = ctx.player->tarotFx;
        float dayNightMult = 1.f;
        if (ctx.dayNight) {
            const bool night =
                ctx.dayNight->sample().sunIntensity < 0.20f;
            dayNightMult = night ? fx.nightSpawnMult : fx.daySpawnMult;
            if (night && fx.bloodMoon) dayNightMult *= 2.f;
        }
        effectiveBudget =
            scaledBudget(effectiveBudget, fx.spawnRateMult, dayNightMult);
    }
    if (static_cast<std::size_t>(ctx.enemies->count()) >=
        static_cast<std::size_t>(effectiveBudget))
        return;

    // Roster por bioma × horário no tile do player (biomas são
    // regionais; o anel de spawn cruza a borda, vale o centro).
    // Sem mundo: bit tudo-1 (bioma passa; estrato/horário valem).
    uint8_t biomeBit = 0xFF;
    if (ctx.world) {
        const uint32_t seed = ctx.world->getSeed();
        const core::TilePos tp = core::worldToTile({px, py});
        const int surfY = support::surfaceHeight(tp.x, seed);
        const support::Biome b = support::pickBiome(
            support::temperature(tp.x, tp.y, seed),
            support::humidity(tp.x, tp.y, seed),
            support::isOceanColumn(tp.x, seed),
            support::isCoastal(surfY));
        biomeBit = static_cast<uint8_t>(1u << static_cast<int>(b));
    }
    const bool isNight =
        ctx.dayNight
            ? ctx.dayNight->sample().sunIntensity < 0.20f
            : false;

    // Candidatos data-driven: faixa (sem bypass de S0) + bioma +
    // horário + maxAlive por archetype, peso ponderado. Sorteio único
    // por janela.
    struct Cand {
        std::string key;
        float weight;
    };
    std::vector<Cand> cands;
    float totalW = 0.f;
    for (const auto &key : ArchetypeRegistry::instance().keys()) {
        const EnemyArchetype *a = ArchetypeRegistry::instance().find(key);
        if (!a) continue;
        if (!a->allowsSpawn(biomeBit, isNight, stratum)) continue;
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
        const core::TilePos stp = core::worldToTile({sx, sy});
        int tx = stp.x;
        int tyy = stp.y;
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
    // Matilha: companheiros do mesmo kind ao redor (respeita caps
    // e o budget do estrato — senão o budget vira enfeite).
    if (const EnemyArchetype *pa = ArchetypeRegistry::instance().find(kind)) {
        const int pack =
            rollPackSize(*pa, core::randRange(0.f, 1.f));
        for (int i = 1; i < pack; ++i) {
            if (ctx.enemies->count() >= kGlobalCap) break;
            if (static_cast<std::size_t>(ctx.enemies->count()) >=
                static_cast<std::size_t>(effectiveBudget))
                break;
            if (countAlive(*ctx.enemies, pa->kind) >= pa->maxAlive) break;
            ctx.enemies->spawn(
                kind, sx + core::randRange(-40.f, 40.f), sy, &ctx);
        }
    }
    if (ctx.debug)
        ctx.debug->pushLog("spawn " + kind + " S" +
                           std::to_string(stratum));
}

} // namespace support
