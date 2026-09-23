/**
 * @file src/core/DropTable.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Tabela de drops com chances, quantidades e filtro por nível.
 * @details Resolve rolls determinísticos com LCG e salt por posição via rollDrops, usado por EnemyArchetype e sistemas de loot.
 */

#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace core {

// Entrada de drop: o que pode cair, com chance e quantidade.
// minLevel/maxLevel filtram por variantLevel do inimigo.
struct DropEntry {
    std::string itemId;
    float       chance = 1.f; // 0..1
    int         minQty = 1;
    int         maxQty = 1;
    int         minLevel = 1;
    int         maxLevel = 99;
};

struct DropTable {
    std::vector<DropEntry> entries;
};

// Resolve a tabela p/ um nível: lista de (itemId, qty).
// RNG local com salt (LCG): determinístico p/ (posição, seed do mundo).
// NUNCA core::randRange aqui — Random.h proíbe global em gameplay.
inline std::vector<std::pair<std::string, int>>
rollDrops(const DropTable& table, int level, uint32_t salt) {
    std::vector<std::pair<std::string, int>> out;
    uint32_t rng = salt;
    for (const auto& e : table.entries) {
        if (level < e.minLevel || level > e.maxLevel) continue;
        rng = rng * 1103515245u + 12345u;
        const float roll = (rng & 0xFFFF) / 65536.f;
        if (roll >= e.chance) continue;
        rng = rng * 1103515245u + 12345u;
        const int range = e.maxQty - e.minQty + 1;
        const int qty = e.minQty + static_cast<int>((rng >> 16) % range);
        out.emplace_back(e.itemId, qty);
    }
    return out;
}

// Salt estável p/ (tile, seed): mesma posição + mesmo mundo = mesmo drop.
inline uint32_t dropSalt(int tx, int ty, uint32_t worldSeed) {
    uint32_t h = worldSeed ^ (static_cast<uint32_t>(tx) * 73856093u) ^
                 (static_cast<uint32_t>(ty) * 19349663u);
    h ^= h >> 13;
    h *= 1274126177u;
    return h ^ (h >> 16);
}

} // namespace core
