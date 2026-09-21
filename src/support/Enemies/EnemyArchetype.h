#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/EntityKind.h"

namespace support {

// Tudo que varia entre inimigos e não é comportamento. Adicionar
// inimigo = 1 REGISTER_ENEMY_ARCHETYPE (dado) + 1 Behavior (código).
// Física/combate/morte operam em Enemy e nunca conhecem o tipo.
struct EnemyArchetype {
    sf::Color color{100, 200, 100};
    sf::Vector2f hitboxSize{36.f, 44.f};

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

    // Skills disponíveis (IDs no SkillRegistry). Vazio = behavior manual.
    std::vector<std::string> skills;
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
