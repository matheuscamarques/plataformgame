/**
 * @file src/support/Combat/BodySchemaRegistry.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra schemas corporais humanoid e dwarf com auto-registro no load.
 * @details Expõe singleton com add e get mais registerDefaultBodySchemas, usado pela Factory para anexar BodySchema ao Enemy.
 */

#pragma once
#include <string>
#include <unordered_map>

#include "Body.h"

namespace support {

// "humanoid"/"dwarf" resolvem para schemas estáticos. Auto-registro no
// load (mesmo padrão do BehaviorRegistry): zero chamada de boot,
// zero churn em teste. Schemas function-local static (sem fiasco).
class BodySchemaRegistry {
public:
    static BodySchemaRegistry &instance() {
        static BodySchemaRegistry r;
        return r;
    }

    void add(const std::string &key, const BodySchema *schema) {
        schemas_[key] = schema;
    }

    const BodySchema *get(const std::string &key) const {
        auto it = schemas_.find(key);
        return it == schemas_.end() ? nullptr : it->second;
    }

private:
    std::unordered_map<std::string, const BodySchema *> schemas_;
};

inline void registerDefaultBodySchemas() {
    static BodySchema humanoid = BodySchema::humanoid(30.0f, 40.0f);
    static BodySchema dwarf = BodySchema::dwarf();
    static BodySchema skeleton = BodySchema::humanoid(100.f, 60.f);
    auto &r = BodySchemaRegistry::instance();
    r.add("humanoid", &humanoid);
    r.add("dwarf", &dwarf);
    r.add("skeleton", &skeleton);
}

namespace {
struct BodySchemaAutoReg {
    BodySchemaAutoReg() { registerDefaultBodySchemas(); }
};
static BodySchemaAutoReg bodySchemaAutoRegInstance;
} // namespace

} // namespace support
