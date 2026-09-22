#include "EnemySystem.h"

#include <cmath>

#include <SFML/Graphics/Color.hpp>

#include "defines.h"
#include "BehaviorRegistry.h"
#include "support/Combat/BodySchemaRegistry.h"
#include "EnemyArchetype.h"
#include "SlimeAI.h"
#include "VariantRegistry.h"
#include "world/Stratum.h"

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

    Entity body(core::kIdSlime, x, y, a->hitboxSize.x, a->hitboxSize.y);
    body.setFillColor(a->color);

    auto e = std::make_unique<Enemy>(std::move(body), std::move(ai));
    e->archetypeId = kind; // p/ drops (DeathSystem rola pela tabela)
    if (const BodySchema *s = BodySchemaRegistry::instance().get(a->bodySchema))
        e->bodyParts.attach(s);

    e->resources.isTrash = a->isTrash;
    e->resources.hp = a->hp;
    e->resources.hpMax = a->hp;
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

    // Variante por profundidade (dano/hp/skills extras). Slime não tem
    // variantes: forDepth retorna null e nada muda.
    const int stratum = stratumAt(static_cast<int>(std::floor(y / core::kBlockSize)));
    if (const VariantDef *v = VariantRegistry::instance().forDepth(kind, stratum)) {
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
