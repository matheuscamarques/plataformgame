/**
 * @file src/world/BlockRegistry.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Implementa registro global e caches por estrato de blocos.
 * @details Implementa singleton com add, find, baseFor, flavorsFor e raresFor com cache lazy por estrato, consumido pela Generation para escolher rochas.
 */

#include "BlockRegistry.h"

namespace support {

BlockRegistry &BlockRegistry::instance() {
    static BlockRegistry reg;
    return reg;
}

void BlockRegistry::add(BlockEntry e) {
    entries_.push_back(e);
}

const BlockEntry *BlockRegistry::find(Tile t) const {
    for (auto &e : entries_)
        if (e.tile == t) return &e;
    return nullptr;
}

void BlockRegistry::buildCache(int stratum) {
    if (stratum < 0 || stratum > 10) return;
    if (cached_[stratum]) return;
    cached_[stratum] = true;
    const uint16_t bit = static_cast<uint16_t>(1u << stratum);
    for (auto &e : entries_) {
        if (!e.generates || !(e.strataMask & bit)) continue;
        if (e.isBase && !baseCache_[stratum]) baseCache_[stratum] = &e;
        if (e.isFlavor) flavorCache_[stratum].push_back(&e);
        if (e.isRare) rareCache_[stratum].push_back(&e);
    }
}

const BlockEntry *BlockRegistry::baseFor(int stratum) {
    buildCache(stratum);
    if (stratum < 0 || stratum > 10) return nullptr;
    return baseCache_[stratum];
}

const std::vector<const BlockEntry *> &BlockRegistry::flavorsFor(int stratum) {
    static const std::vector<const BlockEntry *> empty;
    if (stratum < 0 || stratum > 10) return empty;
    buildCache(stratum);
    return flavorCache_[stratum];
}

const std::vector<const BlockEntry *> &BlockRegistry::raresFor(int stratum) {
    static const std::vector<const BlockEntry *> empty;
    if (stratum < 0 || stratum > 10) return empty;
    buildCache(stratum);
    return rareCache_[stratum];
}

} // namespace support
