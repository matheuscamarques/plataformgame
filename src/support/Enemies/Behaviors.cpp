/**
 * @file src/support/Enemies/Behaviors.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define arquétipos slime e anão mais skills iniciais por dados.
 * @details Usa REGISTER_ENEMY_ARCHETYPE e REGISTER_SKILL para stats e drops, lido pela Factory e SpawnSystem sem editar código.
 */

#include "EnemyArchetype.h"
#include "support/Skills/Skill.h"

#include <cmath>

#include <SFML/System/Vector2.hpp>

#include "core/Config.h"
#include "entities/Player/Player.h"
#include "game/SoundBank.h"
#include "support/Enemies/EnemySystem.h"
#include "support/GameContext.h"
#include "support/Effects/ThrowSystem.h"
#include "world/World.h"
#include "core/Vec.h"
#include "core/Coords.h"

// Slime (trash) + anão básico (elite). Terceiro inimigo = append aqui
// + 1 arquivo Behavior. Zero edição em Factory/SpawnSystem.
REGISTER_ENEMY_ARCHETYPE("slime", [] {
    support::EnemyArchetype a;
    a.color = {0, 200, 0};
    a.hitboxSize = {40.f, 30.f};
    a.behaviorKind = "slime";
    a.kind = core::EntityKind::Slime;
    a.bodySchema = "humanoid";
    a.isTrash = true;
    a.hp = 60; // trash com corpo: 2 golpes de espada (era 30)
    a.postureMax = 20.f;
    a.postureRegen = 10.f;
    a.postureRegenDelay = 1.0f;
    a.minStratum = 0;
    a.maxStratum = 99;
    a.spawnWeight = 1.0f;
    a.maxAlive = 100;
    a.drops.entries.push_back({"slime_gel", 0.8f, 1, 2});
    a.drops.entries.push_back({"purple_moss", 0.3f, 1, 1});
    a.drops.entries.push_back({"bloodred_moss", 0.2f, 1, 1});
    a.drops.entries.push_back({"soul_lost", 0.35f, 1, 1});
    a.drops.entries.push_back({"soul_great", 0.1f, 1, 1});
    a.xp = 100; // slime vale 100 souls
    a.frameIdle = support::SpriteFrameId::SlimeIdle;
    a.frameWalkA = support::SpriteFrameId::SlimeSquash;
    a.frameWalkB = support::SpriteFrameId::SlimeSquash;
    a.frameMelee = support::SpriteFrameId::SlimeIdle;
    a.frameRanged = support::SpriteFrameId::SlimeIdle;
    return a;
}());

REGISTER_ENEMY_ARCHETYPE("dwarf", [] {
    support::EnemyArchetype a;
    a.color = {139, 90, 43};
    a.hitboxSize = {36.f, 44.f};
    a.behaviorKind = "dwarf";
    a.kind = core::EntityKind::Dwarf;
    a.bodySchema = "dwarf";
    a.isTrash = false;
    a.hp = 60;
    a.postureMax = 30.f;
    a.postureRegen = 15.f;
    a.postureRegenDelay = 1.2f;
    a.staminaMax = 40.f;
    a.staminaRegen = 20.f;
    a.staminaRegenDelay = 0.8f;
    a.manaMax = 20.f; // especial custa 20 (recursos da definição, §2)
    a.manaRegen = 1.0f;
    a.manaRegenDelay = 2.0f;
    a.minStratum = 3;
    a.maxStratum = 99;
    a.spawnWeight = 0.3f; // unificado S3+ (era 15/30 — tuning junto)
    a.maxAlive = 1;
    a.drops.entries.push_back({"iron_ore", 0.4f, 1, 1});
    a.skills = {"dwarf_dynamite", "dwarf_melee"};
    a.xp = 150; // elite vale mais que trash
    a.frameIdle = support::SpriteFrameId::DwarfIdle;
    a.frameWalkA = support::SpriteFrameId::DwarfWalkA;
    a.frameWalkB = support::SpriteFrameId::DwarfWalkB;
    a.frameMelee = support::SpriteFrameId::DwarfMelee;
    a.frameRanged = support::SpriteFrameId::DwarfThrow;
    return a;
}());

REGISTER_ENEMY_ARCHETYPE("skeleton", [] {
    support::EnemyArchetype a;
    a.color = {200, 195, 170}; // osso
    a.hitboxSize = {60.f, 100.f}; // reflexo do player (2 blocos)
    a.behaviorKind = "skeleton";
    a.kind = core::EntityKind::Skeleton;
    a.bodySchema = "skeleton";
    a.isTrash = false;
    a.hp = 45;
    a.postureMax = 25.f;
    a.postureRegen = 12.f;
    a.postureRegenDelay = 1.0f;
    a.staminaMax = 30.f;
    a.staminaRegen = 20.f;
    a.staminaRegenDelay = 0.8f;
    a.minStratum = 2;
    a.maxStratum = 99;
    a.spawnWeight = 0.4f;
    a.maxAlive = 4;
    a.drops.entries.push_back({"iron_ore", 0.25f, 1, 1});
    a.drops.entries.push_back({"soul_lost", 0.35f, 1, 1});
    a.drops.entries.push_back({"soul_great", 0.1f, 1, 1});
    a.skills = {"skeleton_slash"}; // melee-only: sem dynamite
    a.startingEquipment = {{"iron_helm", 0.35f}, {"iron_sword", 0.50f}};
    a.xp = 120;
    a.frameIdle = support::SpriteFrameId::SkeletonIdle;
    a.frameWalkA = support::SpriteFrameId::SkeletonWalkA;
    a.frameWalkB = support::SpriteFrameId::SkeletonWalkB;
    a.frameMelee = support::SpriteFrameId::SkeletonMelee;
    a.frameRanged = support::SpriteFrameId::SkeletonIdle; // sem throw
    return a;
}());

// Golpe do esqueleto: melee espelhado do dwarf_melee (mesmo alcance
// 40px, dano 10 × damageMult). Nome próprio p/ feed e debug.
REGISTER_SKILL("skeleton_slash", [] {
    support::SkillDef s;
    s.name = "Golpe Osseo";
    s.cooldown = 0.9f;
    s.telegraph = 0.25f;
    s.staminaCost = 15.f;
    s.isMelee = true;
    s.minRange = 0.f;
    s.maxRange = 40.f;
    s.baseWeight = 15.f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.player) return;
        const float dx = ctx.player->getCenterX() - self.body.getCenterX();
        const float dy = ctx.player->getCenterY() - self.body.getCenterY();
        if (dx * dx + dy * dy < 48.f * 48.f)
            ctx.player->hurt(static_cast<int>(10.f * self.damageMult));
    };
    return s;
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
        core::Vec2f from{self.body.getCenterX(), self.body.getCenterY()};
        core::Vec2f to{ctx.player->getCenterX(), ctx.player->getCenterY()};
        core::Vec2f dir = to - from;
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

// Dinamite do anão: arco na direção do player, fuse 0.8s.
REGISTER_SKILL("dwarf_dynamite", [] {
    support::SkillDef s;
    s.name = "Dinamite";
    s.cooldown = 1.8f;
    s.telegraph = 0.30f;
    s.staminaCost = 5.f;
    s.isRanged = true;
    s.minRange = 48.f;
    s.maxRange = 128.f;
    s.baseWeight = 10.f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.player || !ctx.throws) return;
        core::Vec2f from{self.body.getCenterX(), self.body.getCenterY()};
        core::Vec2f to{ctx.player->getCenterX(), ctx.player->getCenterY()};
        const float dir = (to.x < from.x) ? -1.f : 1.f;
        auto *t = ctx.throws->throwItem(from, {dir * 180.f, -320.f},
                                        support::ThrowKind::Dynamite);
        if (t) {
            t->fuse = 0.8f;
            t->damage = static_cast<int>(25.f * self.damageMult);
            t->radius = 40.f;
            t->gravity = 600.f;
        }
    };
    return s;
}());

// Picaretada: dano direto com re-cheque de alcance no impacto.
REGISTER_SKILL("dwarf_melee", [] {
    support::SkillDef s;
    s.name = "Picaretada";
    s.cooldown = 0.8f;
    s.telegraph = 0.25f;
    s.staminaCost = 15.f;
    s.isMelee = true;
    s.minRange = 0.f;
    s.maxRange = 40.f;
    s.baseWeight = 15.f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.player) return;
        const float dx = ctx.player->getCenterX() - self.body.getCenterX();
        const float dy = ctx.player->getCenterY() - self.body.getCenterY();
        if (dx * dx + dy * dy < 48.f * 48.f)
            ctx.player->hurt(static_cast<int>(12.f * self.damageMult));
    };
    return s;
}());

// Bomba de Fumaça: dash para trás (via knockbackLock: física integra,
// IA pausa 0.25s). Nuvem visual pendente (sem SmokeSystem).
REGISTER_SKILL("dwarf_smoke", [] {
    support::SkillDef s;
    s.name = "Fumaca";
    s.cooldown = 8.f;
    s.telegraph = 0.30f;
    s.manaCost = 5.f;
    s.staminaCost = 10.f;
    s.isRanged = true;
    s.minRange = 32.f;
    s.maxRange = 200.f;
    s.baseWeight = 6.f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.player) return;
        const float dir =
            (ctx.player->getCenterX() < self.body.getCenterX()) ? 1.f : -1.f;
        self.body.setVx(dir * 8.f);
        self.knockbackLock.trigger(0.25f);
    };
    return s;
}());

// Barril Rolante: bomba lenta que detona no fuse (sem quique ainda).
REGISTER_SKILL("dwarf_barrel", [] {
    support::SkillDef s;
    s.name = "Barril";
    s.cooldown = 6.f;
    s.telegraph = 0.50f;
    s.manaCost = 10.f;
    s.staminaCost = 20.f;
    s.isRanged = true;
    s.minRange = 48.f;
    s.maxRange = 240.f;
    s.baseWeight = 8.f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.throws || !ctx.player) return;
        core::Vec2f from{self.body.getCenterX(), self.body.getCenterY()};
        const float dir = (ctx.player->getCenterX() < from.x) ? -1.f : 1.f;
        auto *t = ctx.throws->throwItem(from, {dir * 200.f, 0.f},
                                        support::ThrowKind::Barrel);
        if (t) {
            t->fuse = 2.0f;
            t->damage = static_cast<int>(20.f * self.damageMult);
            t->radius = 32.f;
            t->gravity = 400.f;
        }
    };
    return s;
}());

// Escavação: teleporta 128px na direção do player (se houver ar) e
// quebra 3x3 nas duas pontas. Sem invencibilidade ainda.
REGISTER_SKILL("dwarf_dig", [] {
    support::SkillDef s;
    s.name = "Escavacao";
    s.cooldown = 12.f;
    s.telegraph = 0.60f;
    s.manaCost = 15.f;
    s.staminaCost = 25.f;
    s.isRanged = true;
    s.minRange = 0.f;
    s.maxRange = 400.f;
    s.baseWeight = 4.f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.player || !ctx.world) return;
        const float dir = (ctx.player->getCenterX() < self.body.getCenterX())
                              ? -1.f
                              : 1.f;
        const float nx = self.body.getCenterX() + dir * 128.f;
        const float ny = self.body.getCenterY();
        const core::TilePos ntp = core::worldToTile({nx, ny});
        const int ntx = ntp.x;
        const int nty = ntp.y;
        auto break3x3 = [&](int cx, int cy) {
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    ctx.world->breakTile(cx + dx, cy + dy);
        };
        break3x3(core::worldToTile({self.body.getCenterX(), self.body.getCenterY()}).x,
                 core::worldToTile({self.body.getCenterX(), self.body.getCenterY()}).y);
        // SFX escavação do anão (1 por evento, não por tile).
        if (ctx.audio) ctx.audio->play(game::keyOf(game::Sfx::TileBreak), 0.7f);
        if (!ctx.world->isSolid(ntx, nty)) {
            self.body.setX(nx - self.body.getW() * 0.5f);
            break3x3(ntx, nty);
        }
    };
    return s;
}());

// Desabamento: linha de destruição 6 tiles à frente + 40 de dano.
// Trigger por UtilityAI (isSpecial + hp<20%) ou nível 5.
REGISTER_SKILL("dwarf_collapse", [] {
    support::SkillDef s;
    s.name = "Desabamento";
    s.cooldown = 25.f;
    s.telegraph = 1.80f;
    s.manaCost = 20.f;
    s.staminaCost = 40.f;
    s.isSpecial = true;
    s.isRanged = true;
    s.minRange = 0.f;
    s.maxRange = 300.f;
    s.baseWeight = 5.f;
    s.execute = [](support::Enemy &self, support::GameContext &ctx) {
        if (!ctx.world || !ctx.player) return;
        const float dir = (ctx.player->getCenterX() < self.body.getCenterX())
                              ? -1.f
                              : 1.f;
        const float cx = self.body.getCenterX();
        const int ty = core::worldToTile({self.body.getCenterX(), self.body.getCenterY()}).y;
        for (int i = 1; i <= 6; ++i) {
            const int tx = core::worldToTile({cx + dir * i * core::kBlockSize, self.body.getCenterY()}).x;
            for (int dy = -2; dy <= 2; ++dy) ctx.world->breakTile(tx, ty + dy);
        }
        // SFX colapso (1 por evento, não por tile).
        if (ctx.audio) ctx.audio->play(game::keyOf(game::Sfx::TileBreak), 0.7f);
        ctx.player->hurt(static_cast<int>(40.f * self.damageMult));
    };
    return s;
}());
