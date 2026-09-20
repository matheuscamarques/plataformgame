#include "EnemyArchetype.h"
#include "Skill.h"

#include <cmath>

#include <SFML/System/Vector2.hpp>

#include "../entities/player/player.h"
#include "EnemySystem.h"
#include "GameContext.h"
#include "ThrowSystem.h"

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

// Cuspe de slime: projétil linear (sem gravidade/fuse), dano no impacto.
// Valida o mecanismo SkillRegistry; anão ganha as dele no Elite.
REGISTER_SKILL("slime_spit", [] {
    support::SkillDef s;
    s.name = "Spit";
    s.cooldown = 1.8f;
    s.telegraph = 0.20f;
    s.staminaCost = 5.f;
    s.isRanged = true;
    s.maxRange = 220.f;
    s.baseWeight = 1.0f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.player || !ctx.throws) return;
        sf::Vector2f from{self.body.getCenterX(), self.body.getCenterY()};
        sf::Vector2f to{ctx.player->getCenterX(), ctx.player->getCenterY()};
        sf::Vector2f dir = to - from;
        const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 1.f) return;
        dir /= len;
        auto *t = ctx.throws->throwItem(from, dir * 260.f, support::ThrowKind::Spit);
        if (t) {
            t->gravity = 0.f;
            t->fuse = -1.f; // sem fuse: expira parado (ThrowSystem libera)
            t->damage = 8;
            t->radius = 0.f;
            t->tilesRadius = 0;
        }
    };
    return s;
}());
