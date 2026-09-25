/**
 * @file tests/test_skeleton.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava o esqueleto (arquétipo, skill, frames, equipment).
 * @details Cobre registro skeleton/skeleton_slash, spawn via Factory com schema próprio, identidade SkeletonAI (kind próprio, sem dynamite) e equipamento vestido, roda com make test que compila em build/tests/test_skeleton.
 */

#include <cassert>
#include <cstdio>
#include <string>

#include "assets/Sprites/EnemySprites.h"
#include "entities/Player/Player.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Enemies/SkeletonAI.h"
#include "support/Combat/BodySchemaRegistry.h"
#include "support/Combat/SpriteFrame.h"
#include "assets/SpriteFrameRegistry.h"
#include "support/Enemies/VariantRegistry.h"
#include "support/GameContext.h"
#include "support/Skills/Skill.h"
#include "support/Skills/SkillSystem.h"

int main() {
    using namespace support;

    { // ArchetypeRegistered (base: hp45, xp120, estrato 2+, melee-only)
        const EnemyArchetype *a = ArchetypeRegistry::instance().find("skeleton");
        assert(a != nullptr);
        assert(a->behaviorKind == "skeleton");
        assert(a->kind == core::EntityKind::Skeleton);
        assert(a->bodySchema == "skeleton");
        assert(!a->isTrash && a->hp == 45 && a->xp == 120);
        assert(a->minStratum == 2 && a->maxStratum == 99);
        assert(a->maxAlive == 4);
        assert(a->skills.size() == 1u && a->skills[0] == "skeleton_slash");
    }
    { // SkillRegistered (melee 40px, sem throw)
        const SkillDef *s = SkillRegistry::instance().find("skeleton_slash");
        assert(s != nullptr && s->isMelee && s->maxRange == 40.f);
    }
    { // IdentityIsSkeleton (kind próprio; DwarfAI crua continua anã)
        SkeletonAI sk;
        assert(sk.kind() == core::EntityKind::Skeleton);
        assert(std::string(sk.name()) == "SkeletonAI");
        DwarfAI dw;
        assert(dw.kind() == core::EntityKind::Dwarf);
    }
    { // FramesResolve12x40 (reflexo do player, paleta osso)
        assert(sprites::kSkeletonW == 12 && sprites::kSkeletonH == 40);
        assert(sprites::kSkeletonPalCount == 8u);
        for (auto id :
             {SpriteFrameId::SkeletonIdle, SpriteFrameId::SkeletonWalkA,
              SpriteFrameId::SkeletonWalkB, SpriteFrameId::SkeletonMelee}) {
            const auto f = assets::frameData(id);
            assert(f.rows != nullptr && f.w == 12 && f.h == 40);
        }
    }
    { // FactorySpawnsWithOwnSchema (spawn + equipment vestível)
        auto e = Factory::spawnEnemy("skeleton", 100.f, 200.f);
        assert(e != nullptr);
        assert(e->archetypeId == "skeleton");
        assert(e->resources.hp == 45 && !e->resources.isTrash);
        assert(e->bodyParts.schema != nullptr);
        assert(e->skillIds.size() == 1u && e->skillIds[0] == "skeleton_slash");
        // Equipment veste e desequipa (drop do vestido é DeathSystem).
        assert(e->equipment.equip(core::Item{"iron_sword", 1}));
        assert(e->equipment.isOccupied(core::EquipSlot::RightHand));
        const core::Item back =
            e->equipment.unequip(core::EquipSlot::RightHand);
        assert(back.defId == "iron_sword");
    }

    { // DeathSfxPerKind (esqueleto não morre com SFX de slime)
        assert(deathSfxFor(core::EntityKind::Skeleton) ==
               game::Sfx::SkeletonDeath);
        assert(deathSfxFor(core::EntityKind::Dwarf) ==
               game::Sfx::DwarfDeath);
        assert(deathSfxFor(core::EntityKind::Slime) ==
               game::Sfx::SlimeDeath);
        assert(std::string(game::keyOf(game::Sfx::SkeletonDeath)) ==
               "skeleton_death");
    }
    { // StartingEquipmentIsData (tabela no arquétipo, sem branch)
        const EnemyArchetype *a =
            ArchetypeRegistry::instance().find("skeleton");
        assert(a->startingEquipment.size() == 2u);
        assert(a->startingEquipment[0].itemId == "iron_helm");
        assert(a->startingEquipment[1].itemId == "iron_sword");
        const EnemyArchetype *s =
            ArchetypeRegistry::instance().find("slime");
        assert(s->startingEquipment.empty()); // slime nasce nu
    }
    { // FactoryEquipDeterministic (mesma posição = mesmo vestido)
        auto e1 = Factory::spawnEnemy("skeleton", 100.f, 200.f);
        auto e2 = Factory::spawnEnemy("skeleton", 100.f, 200.f);
        assert(e1 && e2);
        for (int i = 1; i < core::kEquipSlotCount; ++i) {
            const auto slot = static_cast<core::EquipSlot>(i);
            assert(e1->equipment.get(slot).defId ==
                   e2->equipment.get(slot).defId);
        }
    }

    { // FramesFromData (App sem branch: tudo vem do arquétipo)
        const EnemyArchetype *a = ArchetypeRegistry::instance().find("skeleton");
        assert(a->frameIdle == SpriteFrameId::SkeletonIdle);
        assert(a->frameWalkA == SpriteFrameId::SkeletonWalkA);
        assert(a->frameWalkB == SpriteFrameId::SkeletonWalkB);
        assert(a->frameMelee == SpriteFrameId::SkeletonMelee);
        const EnemyArchetype *d = ArchetypeRegistry::instance().find("dwarf");
        assert(d->frameMelee == SpriteFrameId::DwarfMelee);
        assert(d->frameRanged == SpriteFrameId::DwarfThrow);
        const EnemyArchetype *s = ArchetypeRegistry::instance().find("slime");
        assert(s->frameIdle == SpriteFrameId::SlimeIdle);
        assert(s->frameWalkA == SpriteFrameId::SlimeSquash);
    }

    { // FlameVariantByDepth (S2-3 base; S4+ flamejante via dado)
        const VariantDef *l1 =
            VariantRegistry::instance().forDepth("skeleton", 3);
        assert(l1 != nullptr && l1->level == 1);
        assert(l1->extraSkills.empty());
        const VariantDef *l2 =
            VariantRegistry::instance().forDepth("skeleton", 4);
        assert(l2 != nullptr && l2->level == 2);
        assert(l2->extraSkills.size() == 1u &&
               l2->extraSkills[0] == "skeleton_flame_slash");
        const SkillDef *fs =
            SkillRegistry::instance().find("skeleton_flame_slash");
        assert(fs != nullptr && fs->isMelee);
        assert(fs->damageType == core::DamageType::Fire);
        // S4 = ty 3800+: spawn aplica variante (hp 55, skill extra).
        auto e = Factory::spawnEnemy("skeleton", 100.f, 3800.f * 50.f);
        assert(e != nullptr && e->variantLevel == 2);
        assert(e->resources.hp == 55 && e->resources.hpMax == 55);
        bool hasFlame = false;
        for (auto &sk : e->skillIds)
            if (sk == "skeleton_flame_slash") hasFlame = true;
        assert(hasFlame);
    }
    { // FlameHurtsFire (golpe flamejante queima o player)
        Player p;
        p.setX(100.f);
        p.setY(100.f);
        p.hp = 100;
        support::EnemySystem enemies;
        enemies.spawn("skeleton", 100.f, 100.f);
        support::GameContext ctx{};
        ctx.player = &p;
        ctx.enemies = &enemies;
        bool ran = false;
        enemies.forEach([&](support::Enemy &e) {
            ran = support::SkillSystem::tryUse(e, ctx, "skeleton_flame_slash");
        });
        assert(ran);
        assert(p.hp == 88); // 12 fogo, resist 1.0
    }

    std::printf("skeleton test OK\n");
    return 0;
}
