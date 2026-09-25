/**
 * @file src/support/Skills/Skill.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define Skill como dado mais callback com registro por chave.
 * @details Expõe struct SkillDef com custos e ranges mais SkillRegistry e REGISTER_SKILL, usado por DwarfAI e SlimeAI via SkillSystem.
 */

#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/DamageType.h"

namespace support {

struct Enemy;
struct GameContext;

// Skill = dado + callback. Zero herança. execute mora no arquivo do
// behavior que usa a skill; UtilityAI (futuro) só escolhe qual chamar.
struct SkillDef {
    std::string id;
    std::string name;

    float manaCost = 0.f;
    float staminaCost = 0.f;
    float postureCost = 0.f;

    float cooldown = 1.f;
    float telegraph = 0.f;

    std::function<void(Enemy &, GameContext &, const SkillDef &)> execute;

    // Tipo de dano da skill (Fase 1 elementais; default físico).
    // Propagado via def no execute (Opção B, sem estado global).
    core::DamageType damageType = core::DamageType::Physical;

    // Metadados p/ UtilityAI.
    float baseWeight = 1.f;
    bool isRanged = false;
    bool isMelee = false;
    bool isSpecial = false;
    float minRange = 0.f;
    float maxRange = 99999.f;
};

class SkillRegistry {
public:
    static SkillRegistry &instance() {
        static SkillRegistry r;
        return r;
    }

    void add(const std::string &key, SkillDef s) {
        s.id = key;
        if (!has(key)) keys_.push_back(key);
        items_[key] = std::move(s);
    }

    bool has(const std::string &key) const { return items_.count(key) > 0; }

    const SkillDef *find(const std::string &key) const {
        auto it = items_.find(key);
        return it == items_.end() ? nullptr : &it->second;
    }

    const std::vector<std::string> &keys() const { return keys_; }

private:
    std::unordered_map<std::string, SkillDef> items_;
    std::vector<std::string> keys_; // ordem determinística de registro
};

} // namespace support

#define _REG_SKILL_CONCAT(a, b) a##b
#define _REG_SKILL_TYPE(line) _REG_SKILL_CONCAT(AutoRegSkill_, line)
#define _REG_SKILL_INST(line) _REG_SKILL_CONCAT(autoRegSkillInstance_, line)
#define REGISTER_SKILL(key, ...)                                            \
    namespace {                                                             \
    struct _REG_SKILL_TYPE(__LINE__) {                                       \
        _REG_SKILL_TYPE(__LINE__)() {                                       \
            ::support::SkillRegistry::instance().add(key, __VA_ARGS__);     \
        }                                                                   \
    };                                                                      \
    static _REG_SKILL_TYPE(__LINE__) _REG_SKILL_INST(__LINE__);             \
    }
