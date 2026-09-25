/**
 * @file src/support/Enemies/VariantRegistry.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Define variantes do anão por profundidade com bônus e skills extras.
 * @details Expõe VariantDef e singleton com forDepth mais registerDefaultVariants, aplicado pela Factory de S3 a S7.
 */

#pragma once
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace support {

// Variante por profundidade: S3→nv1 ... S7+→nv5. Puramente dado;
// Factory aplica (hp/dano/skills), DwarfAI não conhece nível.
struct VariantDef {
    int level = 1;
    int hpBonus = 0;
    float damageMult = 1.f;
    float postureBonus = 0.f;
    std::vector<std::string> extraSkills;
    sf::Color auraColor{0, 0, 0, 0};
    bool eyesGlow = false;
    int beardTier = 0;
};

class VariantRegistry {
public:
    static VariantRegistry &instance() {
        static VariantRegistry r;
        return r;
    }

    void add(const std::string &archetype, VariantDef v) {
        byArch_[archetype].push_back(std::move(v));
    }

    const VariantDef *forDepth(const std::string &archetype, int stratum) const {
        auto it = byArch_.find(archetype);
        if (it == byArch_.end()) return nullptr;
        const int level = std::clamp(stratum - 2, 1, 5);
        for (auto &v : it->second)
            if (v.level == level) return &v;
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::vector<VariantDef>> byArch_;
};

inline void registerDefaultVariants() {
    auto &r = VariantRegistry::instance();
    // Base tem {dynamite, melee}: L2 só escala número (extras {}).
    r.add("dwarf", {1, 0, 1.00f, 0.f, {}, {}, false, 0});
    r.add("dwarf", {2, 10, 1.10f, 2.f, {}, {}, false, 1});
    r.add("dwarf", {3, 20, 1.20f, 4.f, {"dwarf_smoke"}, {60, 60, 60, 80}, false, 2});
    r.add("dwarf", {4, 30, 1.35f, 6.f, {"dwarf_barrel"}, {120, 40, 40, 80}, false, 3});
    r.add("dwarf",
          {5, 40, 1.50f, 8.f, {"dwarf_dig", "dwarf_collapse"}, {200, 40, 40, 100}, true, 4});
    // Esqueleto (Fase 4): S2-3 base; S4+ flamejante (slash Fire + aura).
    // Nível = clamp(stratum-2, 1, 5): S4→L2 ... S6+→L5.
    r.add("skeleton", {1, 0, 1.00f, 0.f, {}, {}, false, 0});
    r.add("skeleton", {2, 10, 1.15f, 2.f, {"skeleton_flame_slash"},
                       {200, 80, 30, 90}, false, 0});
    r.add("skeleton", {3, 20, 1.25f, 4.f, {"skeleton_flame_slash"},
                       {220, 70, 25, 110}, true, 0});
    r.add("skeleton", {4, 30, 1.35f, 6.f, {"skeleton_flame_slash"},
                       {240, 60, 20, 130}, true, 0});
    r.add("skeleton", {5, 40, 1.50f, 8.f, {"skeleton_flame_slash"},
                       {255, 50, 15, 150}, true, 0});
}

namespace {
struct VariantAutoReg {
    VariantAutoReg() { registerDefaultVariants(); }
};
static VariantAutoReg variantAutoRegInstance;
} // namespace

} // namespace support
