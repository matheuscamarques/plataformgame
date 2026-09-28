/**
 * @file src/support/Enemies/Factory.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Cria Enemy a partir de arquétipo, behavior, schema e variante.
 * @details Implementa Factory spawnEnemy com ArchetypeRegistry, BehaviorRegistry e VariantRegistry, chamado por EnemySystem e SpawnSystem.
 */

#include "EnemySystem.h"

#include <cmath>
#include <cstring>

#include <SFML/Graphics/Color.hpp>

#include "defines.h"
#include "BehaviorRegistry.h"
#include "core/DayNightCycle.h"
#include "core/DropTable.h"
#include "entities/Player/Player.h"
#include "support/Combat/BodySchemaRegistry.h"
#include "world/World.h"
#include "EnemyArchetype.h"
#include "SlimeAI.h"
#include "VariantRegistry.h"
#include "world/Stratum.h"
#include "core/Coords.h"

namespace support {

// Factory consome ArchetypeRegistry: zero branch por tipo.
// kind desconhecido OU behavior ausente -> nullptr (nunca crash).
// ctx opcional: com ctx, dispara onSpawn (SpawnSystem passa; Game passa null).
std::unique_ptr<Enemy> Factory::spawnEnemy(const std::string &kind,
                                           float x, float y,
                                           GameContext *ctx) {
    const EnemyArchetype *a = ArchetypeRegistry::instance().find(kind);
    if (!a) return nullptr;
    auto ai = BehaviorRegistry::instance().create(a->behaviorKind);
    if (!ai) return nullptr;

    Entity body(core::kIdSlime, x, y, a->hitboxSize.x * a->scale,
                a->hitboxSize.y * a->scale);
    body.setFillColor(a->color);

    auto e = std::make_unique<Enemy>(std::move(body), std::move(ai));
    e->archetypeId = kind; // p/ drops (DeathSystem rola pela tabela)
    if (const BodySchema *s = BodySchemaRegistry::instance().get(a->bodySchema))
        e->bodyParts.attach(s);

    e->resources.isTrash = a->isTrash;
    // Tarô híbrido: Diabo engorda o HP dos nascidos; lua de sangue
    // (+25%) soma. Sem player no ctx = intacto (teste/fallback).
    float foeHpMult = 1.f;
    float eliteMult = 1.f;
    bool bloodNight = false;
    if (ctx && ctx->player) {
        foeHpMult = ctx->player->tarotFx.enemyHpMult;
        eliteMult = ctx->player->tarotFx.eliteChanceMult;
        bloodNight = ctx->player->tarotFx.bloodMoon && ctx->dayNight &&
                     ctx->dayNight->sample().sunIntensity < 0.20f;
        if (bloodNight) foeHpMult *= 1.25f;
    }
    e->resources.hp = static_cast<int>(a->hp * a->scale * foeHpMult);
    e->resources.hpMax = static_cast<int>(a->hp * a->scale * foeHpMult);
    e->resources.posture = a->postureMax;
    e->resources.postureMax = a->postureMax;
    e->resources.postureRegen = a->postureRegen;
    e->resources.postureRegenDelay = core::Cooldown(a->postureRegenDelay);
    e->resources.stamina = a->staminaMax;
    e->resources.staminaMax = a->staminaMax;
    e->resources.staminaRegen = a->staminaRegen;
    e->resources.staminaRegenDelay = core::Cooldown(a->staminaRegenDelay);
    e->resources.mana = a->manaMax;
    e->resources.manaMax = a->manaMax;
    e->resources.manaRegen = a->manaRegen;
    e->resources.manaRegenDelay = core::Cooldown(a->manaRegenDelay);

    e->skillIds = a->skills;
    e->resources.resistances = a->resistances;

    // Equipamento inicial da tabela do arquétipo (vazio = nasce nu).
    // RNG determinístico por posição: bit-cast do float (memcpy, seguro
    // p/ negativos) + LCG local. Nunca global em gameplay (Random.h).
    if (!a->startingEquipment.empty()) {
        uint32_t xb = 0, yb = 0;
        static_assert(sizeof(float) == sizeof(uint32_t), "float 32 bits");
        std::memcpy(&xb, &x, sizeof(float));
        std::memcpy(&yb, &y, sizeof(float));
        uint32_t rng = (xb * 13u) ^ (yb * 71u) ^ 0x9E3779B9u;
        for (const auto &se : a->startingEquipment) {
            rng = rng * 1103515245u + 12345u;
            if ((rng & 0xFFFF) / 65536.f < se.chance)
                e->equipment.equip(core::Item{se.itemId, 1});
        }
    }

    // Variante por profundidade (dano/hp/skills extras). Slime não tem
    // variantes: forDepth retorna null e nada muda.
    // Elite do fado (Torre): 5% base × eliteChanceMult de subir +1 nível
    // (teto 5), rolado no salt de (tile, seed) — sem mundo = sem elite.
    const int stratum = stratumAt(core::worldToTile({x, y}).y);
    const VariantDef *baseVar =
        VariantRegistry::instance().forDepth(kind, stratum);
    const VariantDef *v = baseVar;
    if (ctx && ctx->world && baseVar && baseVar->level < 5) {
        const core::TilePos tp = core::worldToTile({x, y});
        const uint32_t salt =
            core::dropSalt(tp.x, tp.y, ctx->world->getSeed());
        if ((salt % 100u) < 5u * eliteMult)
            v = VariantRegistry::instance().forLevel(kind,
                                                     baseVar->level + 1);
        if (!v) v = baseVar;
    }
    if (v) {
        e->variantLevel = v->level;
        e->damageMult = v->damageMult;
        e->resources.hp += v->hpBonus;
        e->resources.hpMax += v->hpBonus;
        e->resources.posture += v->postureBonus;
        e->resources.postureMax += v->postureBonus;
        for (auto &sk : v->extraSkills) e->skillIds.push_back(sk);
    }

    if (ctx && e->ai) e->ai->onSpawn(*e, *ctx);
    return e;
}

} // namespace support
