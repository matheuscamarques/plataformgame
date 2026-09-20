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
    auto &r = BodySchemaRegistry::instance();
    r.add("humanoid", &humanoid);
    r.add("dwarf", &dwarf);
}

namespace {
struct BodySchemaAutoReg {
    BodySchemaAutoReg() { registerDefaultBodySchemas(); }
};
static BodySchemaAutoReg bodySchemaAutoRegInstance;
} // namespace

} // namespace support
