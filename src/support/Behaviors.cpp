#include "EnemyArchetype.h"

// Slime (trash) + anão básico (elite). Terceiro inimigo = append aqui
// + 1 arquivo Behavior. Zero edição em Factory/SpawnSystem.
REGISTER_ENEMY_ARCHETYPE("slime", [] {
    support::EnemyArchetype a;
    a.color = {0, 200, 0};
    a.hitboxSize = {40.f, 30.f};
    a.behaviorKind = "slime";
    a.behaviorName = "SlimeAI";
    a.bodySchema = "humanoid";
    a.isTrash = true;
    a.hp = 30;
    a.postureMax = 20.f;
    a.postureRegen = 10.f;
    a.postureRegenDelay = 1.0f;
    a.minStratum = 0;
    a.maxStratum = 99;
    a.spawnWeight = 1.0f;
    a.maxAlive = 100;
    return a;
}());

REGISTER_ENEMY_ARCHETYPE("dwarf", [] {
    support::EnemyArchetype a;
    a.color = {139, 90, 43};
    a.hitboxSize = {36.f, 44.f};
    a.behaviorKind = "dwarf";
    a.behaviorName = "DwarfAI";
    a.bodySchema = "dwarf";
    a.isTrash = false;
    a.hp = 60;
    a.postureMax = 30.f;
    a.postureRegen = 15.f;
    a.postureRegenDelay = 1.2f;
    a.staminaMax = 40.f;
    a.staminaRegen = 20.f;
    a.staminaRegenDelay = 0.8f;
    a.manaMax = 0.f;
    a.manaRegen = 1.0f;
    a.manaRegenDelay = 2.0f;
    a.minStratum = 3;
    a.maxStratum = 99;
    a.spawnWeight = 0.3f; // unificado S3+ (era 15/30 — tuning junto)
    a.maxAlive = 1;
    return a;
}());
