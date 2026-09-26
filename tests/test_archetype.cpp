/**
 * @file tests/test_archetype.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava arquétipos slime e anão para Factory e Spawn.
 * @details Cobre EnemyArchetype e EnemySystem, roda com make test que compila em build/tests/test_archetype.
 */

#include <cassert>
#include <cstdio>
#include <string>
#include "support/Combat/BodySchemaRegistry.h"
#include "support/Enemies/EnemyArchetype.h"
#include "support/Enemies/EnemySystem.h"

// Archetypes: slime + anão registrados com dados que dirigem
// Factory (cor/hitbox/schema/recursos) e SpawnSystem (faixa/peso/cap).
int main() {
    using namespace support;

    { // SlimeRegisteredWithTrashDefaults
        const EnemyArchetype *a = ArchetypeRegistry::instance().find("slime");
        assert(a != nullptr);
        assert(a->behaviorKind == "slime" && a->kind == core::EntityKind::Slime);
        assert(a->bodySchema == "humanoid");
        assert(a->isTrash && a->hp == 40);
        assert(a->resistances.get(core::DamageType::Fire) == 1.25f);
        assert(a->minStratum == 0 && a->maxAlive == 100);
    }
    { // DwarfRegisteredAsElite
        const EnemyArchetype *a = ArchetypeRegistry::instance().find("dwarf");
        assert(a != nullptr);
        assert(a->behaviorKind == "dwarf" && a->kind == core::EntityKind::Dwarf);
        assert(a->bodySchema == "dwarf");
        assert(!a->isTrash && a->hp == 60);
        assert(a->resistances.get(core::DamageType::Physical) == 0.85f);
        assert(a->minStratum == 3 && a->maxStratum == 99);
        assert(a->maxAlive == 1);
    }
    { // SkeletonRegisteredAsMirror (reflexo do player, elite S2+)
        const EnemyArchetype *a = ArchetypeRegistry::instance().find("skeleton");
        assert(a != nullptr);
        assert(a->behaviorKind == "skeleton" &&
               a->kind == core::EntityKind::Skeleton);
        assert(a->bodySchema == "skeleton");
        assert(!a->isTrash && a->hp == 45);
        assert(a->minStratum == 2 && a->maxStratum == 99);
        assert(a->maxAlive == 4);
    }
    { // UnknownReturnsNull + KeysDeterministic
        assert(ArchetypeRegistry::instance().find("nope") == nullptr);
        const auto &k = ArchetypeRegistry::instance().keys();
        assert(k.size() == 15u && k[0] == "slime" && k[1] == "dwarf" &&
               k[2] == "skeleton");
    }
    { // BodySchemasResolve (auto-registro; sem chamada de boot)
        assert(BodySchemaRegistry::instance().get("humanoid") != nullptr);
        assert(BodySchemaRegistry::instance().get("dwarf") != nullptr);
        assert(BodySchemaRegistry::instance().get("skeleton") != nullptr);
        assert(BodySchemaRegistry::instance().get("nope") == nullptr);
    }
    { // VariantsShareSlimeKind (mesmo cap/SFX, arte e dados próprios)
        const EnemyArchetype *f =
            ArchetypeRegistry::instance().find("fire_slime");
        assert(f != nullptr && f->kind == core::EntityKind::Slime);
        assert(f->resistances.get(core::DamageType::Fire) == 0.5f);
        assert(f->tint != 0 && f->scale == 1.f);
        assert(f->skills.size() == 1u && f->skills[0] == "imp_fire_spit");
        auto fe = Factory::spawnEnemy("fire_slime", 0.f, 0.f);
        assert(fe && fe->resources.hp == 40);
        assert(fe->resources.resistances.get(core::DamageType::Fire) ==
               0.5f);
        const EnemyArchetype *b =
            ArchetypeRegistry::instance().find("baby_slime");
        auto be = Factory::spawnEnemy("baby_slime", 0.f, 0.f);
        assert(be && be->resources.hp == 28); // 40 × 0.7
        assert(be->body.getW() < 40.f && be->body.getH() < 30.f);
        auto ee = Factory::spawnEnemy("elder_slime", 0.f, 0.f);
        assert(ee && ee->resources.hp == 60); // 40 × 1.5
        assert(ee->body.getW() > 40.f && ee->body.getH() > 30.f);
        const EnemyArchetype *i =
            ArchetypeRegistry::instance().find("ice_slime");
        assert(i->skills.size() == 1u && i->skills[0] == "slime_frost_spit");
    }
    { // FactoryResolvesArchetype (slime/anão/esqueleto/desconhecido)
        auto s = Factory::spawnEnemy("slime", 0.f, 0.f);
        assert(s != nullptr && s->resources.hp == 40 && s->resources.isTrash);
        assert(s->bodyParts.schema != nullptr);
        auto d = Factory::spawnEnemy("dwarf", 0.f, 0.f);
        assert(d != nullptr && d->resources.hp == 60 && !d->resources.isTrash);
        assert(d->bodyParts.schema != nullptr);
        auto sk = Factory::spawnEnemy("skeleton", 0.f, 0.f);
        assert(sk != nullptr && sk->resources.hp == 45 &&
               !sk->resources.isTrash);
        assert(sk->bodyParts.schema != nullptr);
        assert(Factory::spawnEnemy("nope", 0.f, 0.f) == nullptr);
    }

    std::printf("archetype test OK\n");
    return 0;
}
