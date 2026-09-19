#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Behavior.h"

namespace support {

// Registro string -> fábrica de Behavior. Inimigo novo = 1 arquivo novo
// com SUPPORT_REGISTER_BEHAVIOR; zero edição em Factory/Game/Combat.
class BehaviorRegistry {
public:
    using Factory = std::function<std::unique_ptr<Behavior>()>;

    static BehaviorRegistry &instance() {
        static BehaviorRegistry r;
        return r;
    }

    void add(const std::string &key, Factory f) {
        if (!has(key)) keys_.push_back(key);
        items_[key] = std::move(f);
    }

    bool has(const std::string &key) const { return items_.count(key) > 0; }

    std::unique_ptr<Behavior> create(const std::string &key) const {
        auto it = items_.find(key);
        if (it == items_.end()) return nullptr;
        return it->second();
    }

    const std::vector<std::string> &keys() const { return keys_; }

private:
    std::unordered_map<std::string, Factory> items_;
    std::vector<std::string> keys_; // ordem determinística de registro
};

#define SUPPORT_REGISTER_BEHAVIOR(key, Type)                               \
    namespace {                                                             \
    struct AutoReg_##Type {                                                 \
        AutoReg_##Type() {                                                  \
            ::support::BehaviorRegistry::instance().add(                    \
                key, []() -> std::unique_ptr<::support::Behavior> {         \
                    return std::make_unique<Type>();                        \
                });                                                         \
        }                                                                   \
    };                                                                      \
    static AutoReg_##Type autoRegInstance_##Type;                           \
    }

} // namespace support
