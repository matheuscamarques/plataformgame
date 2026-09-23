/**
 * @file src/support/Combat/WeaponRegistry.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Registra WeaponDefs por id com macro de auto-registro ordenado.
 * @details Expõe singleton com add, find, has e keys mais REGISTER_WEAPON, incluído por WeaponDefs e consultado no BodySystem.
 */

#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "WeaponDef.h"

namespace support {

// Registro string -> WeaponDef. Arma nova = 1 REGISTER_WEAPON em
// WeaponDefs.cpp. Mesmo padrão de ArchetypeRegistry/BlockRegistry.
class WeaponRegistry {
public:
    static WeaponRegistry &instance() {
        static WeaponRegistry r;
        return r;
    }

    void add(const std::string &key, WeaponDef d) {
        d.id = key;
        if (!has(key)) keys_.push_back(key);
        items_[key] = std::move(d);
    }

    bool has(const std::string &key) const { return items_.count(key) > 0; }

    const WeaponDef *find(const std::string &key) const {
        auto it = items_.find(key);
        return it == items_.end() ? nullptr : &it->second;
    }

    const std::vector<std::string> &keys() const { return keys_; }

private:
    std::unordered_map<std::string, WeaponDef> items_;
    std::vector<std::string> keys_; // ordem determinística de registro
};

} // namespace support

#define REG_WEAPON_CONCAT_IMPL(a, b) a##b
#define REG_WEAPON_CONCAT(a, b) REG_WEAPON_CONCAT_IMPL(a, b)

#define REGISTER_WEAPON(key, initLambda)                                       \
    namespace {                                                                \
    struct REG_WEAPON_CONCAT(AutoRegWpn_, __LINE__) {                          \
        REG_WEAPON_CONCAT(AutoRegWpn_, __LINE__)() {                           \
            ::support::WeaponRegistry::instance().add(key, initLambda());     \
        }                                                                      \
    } REG_WEAPON_CONCAT(autoRegWpnInstance_, __LINE__);                        \
    }
