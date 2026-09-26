/**
 * @file src/support/Enemies/EnemyArchetype.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define dados do inimigo e registro de arquétipos por chave.
 * @details Expõe struct EnemyArchetype mais ArchetypeRegistry e REGISTER_ENEMY_ARCHETYPE, consumido por Factory, Spawn e Death.
 */

#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/DropTable.h"
#include "core/EntityKind.h"
#include "core/Resistances.h"
#include "core/Vec.h"
#include "support/Combat/SpriteFrame.h"

namespace support {

// Tudo que varia entre inimigos e não é comportamento. Adicionar
// inimigo = 1 REGISTER_ENEMY_ARCHETYPE (dado) + 1 Behavior (código).
// Física/combate/morte operam em Enemy e nunca conhecem o tipo.
struct EnemyArchetype {
    sf::Color color{100, 200, 100};
    core::Vec2f hitboxSize{36.f, 44.f};

    std::string behaviorKind = "slime"; // chave no BehaviorRegistry
    core::EntityKind kind = core::EntityKind::Slime; // p/ contagem (cap)
    std::string bodySchema = "humanoid"; // chave no BodySchemaRegistry

    bool isTrash = true;
    int hp = 30;
    float postureMax = 20.f;
    float postureRegen = 10.f;
    float postureRegenDelay = 1.0f;
    float staminaMax = 0.f;
    float staminaRegen = 20.f;
    float staminaRegenDelay = 0.8f;
    float manaMax = 0.f;
    float manaRegen = 1.0f;
    float manaRegenDelay = 2.0f;

    int minStratum = 0;
    int maxStratum = 99;
    float spawnWeight = 1.0f;
    int maxAlive = 100;

    // Matilha (Minecraft-style): quantos nascem juntos no mesmo ponto.
    // 1/1 = solo. Rato 2-3 (caça em grupo), olho 1-2.
    int packMin = 1;
    int packMax = 1;

    // Skills disponíveis (IDs no SkillRegistry). Vazio = behavior manual.
    std::vector<std::string> skills;

    // Frames por estado (Fase 3: sem branch por archetypeId no App).
    // idle = parado; walkA/B = andando (alterna por tick); melee =
    // ataque corpo-a-corpo; ranged = telegraph de arremesso.
    // Slime: walkA/B = Squash, melee/ranged = Idle (não tem).
    support::SpriteFrameId frameIdle = support::SpriteFrameId::None;
    support::SpriteFrameId frameWalkA = support::SpriteFrameId::None;
    support::SpriteFrameId frameWalkB = support::SpriteFrameId::None;
    support::SpriteFrameId frameMelee = support::SpriteFrameId::None;
    support::SpriteFrameId frameRanged = support::SpriteFrameId::None;

    // Souls da morte (orbe XP). Slime 100, anão 150.
    int xp = 1;

    // Drops ao morrer (rolados em DeathSystem via rollDrops). Vazio = só XP.
    core::DropTable drops;

    // Resistências por tipo (default 1.0 em tudo; esqueleto: osso
    // resiste a físico e queima fácil — dado, sem branch).
    core::Resistances resistances;

    // Peça vestida no spawn + chance (0..1). Rolado na Factory com RNG
    // determinístico por posição. Vazio = nasce nu (slime, anão).
    struct StartingEquip {
        std::string itemId;
        float chance = 1.f;
    };
    std::vector<StartingEquip> startingEquipment;
};

class ArchetypeRegistry {
public:
    static ArchetypeRegistry &instance() {
        static ArchetypeRegistry r;
        return r;
    }

    void add(const std::string &key, EnemyArchetype a) {
        if (!has(key)) keys_.push_back(key);
        items_[key] = std::move(a);
    }

    bool has(const std::string &key) const { return items_.count(key) > 0; }

    const EnemyArchetype *find(const std::string &key) const {
        auto it = items_.find(key);
        return it == items_.end() ? nullptr : &it->second;
    }

    const std::vector<std::string> &keys() const { return keys_; }

private:
    std::unordered_map<std::string, EnemyArchetype> items_;
    std::vector<std::string> keys_; // ordem determinística de registro
};

} // namespace support

#define _REG_ARCH_CONCAT(a, b) a##b
#define _REG_ARCH_TYPE(line) _REG_ARCH_CONCAT(AutoRegArch_, line)
#define _REG_ARCH_INST(line) _REG_ARCH_CONCAT(autoRegArchInstance_, line)
#define REGISTER_ENEMY_ARCHETYPE(key, ...)                                  \
    namespace {                                                             \
    struct _REG_ARCH_TYPE(__LINE__) {                                       \
        _REG_ARCH_TYPE(__LINE__)() {                                        \
            ::support::ArchetypeRegistry::instance().add(key, __VA_ARGS__); \
        }                                                                   \
    };                                                                      \
    static _REG_ARCH_TYPE(__LINE__) _REG_ARCH_INST(__LINE__);               \
    }
