#include "EnemySystem.h"

#include <SFML/Graphics/Color.hpp>

#include "../defines.h"
#include "BehaviorRegistry.h"
#include "BodySchemaRegistry.h"
#include "EnemyArchetype.h"
#include "SlimeAI.h"

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

    Entity body(SLIME, x, y, a->hitboxSize.x, a->hitboxSize.y);
    body.setFillColor(a->color);

    auto e = std::make_unique<Enemy>(std::move(body), std::move(ai));
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

    if (ctx && e->ai) e->ai->onSpawn(*e, *ctx);
    return e;
}

} // namespace support
