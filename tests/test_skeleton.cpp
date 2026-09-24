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
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"
#include "support/Enemies/SkeletonAI.h"
#include "support/Combat/BodySchemaRegistry.h"
#include "support/Combat/SpriteFrame.h"
#include "assets/SpriteFrameRegistry.h"
#include "support/Skills/Skill.h"

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

    std::printf("skeleton test OK\n");
    return 0;
}
