#pragma once
#include <cstdint>
#include <vector>

#include "Block.h"

namespace support {

// Registro de blocos por estrato (mesmo padrão do BehaviorRegistry).
// Adicionar bloco = 1 REGISTER_BLOCK num .cpp de Blocks/ — zero edição
// central. Geração consulta por estrato; BLOCKS[] continua dono de
// cor/kind (teste de consistência trava a sincronia).
struct BlockEntry {
    const char *name;
    Tile tile;
    BlockKind kind;
    uint8_t r, g, b;
    uint16_t strataMask; // bit s = aparece no estrato s
    float chance;        // flavor/raro: P por tile (0 = base/sem roll)
    bool isBase = false;
    bool isFlavor = false;
    bool isRare = false;
    bool isOre = false;  // metadado: gera via pickOre, nunca pelo hook
    bool generates = true;
    uint32_t salt = 0;   // raro/flavor: domínio próprio (const por tipo)
};

class BlockRegistry {
public:
    static BlockRegistry &instance();

    void add(BlockEntry e);

    const std::vector<BlockEntry> &all() const { return entries_; }
    const BlockEntry *find(Tile t) const;

    // Base rock do estrato (nullptr = Stone legado). Cache lazy.
    const BlockEntry *baseFor(int stratum);
    // Flavors do estrato (só generates). Cache lazy.
    const std::vector<const BlockEntry *> &flavorsFor(int stratum);
    // Raros do estrato (só generates). Cache lazy.
    const std::vector<const BlockEntry *> &raresFor(int stratum);

private:
    std::vector<BlockEntry> entries_;
    // Cache por estrato (11): registry congela após static-init.
    const BlockEntry *baseCache_[11] = {};
    std::vector<const BlockEntry *> flavorCache_[11];
    std::vector<const BlockEntry *> rareCache_[11];
    bool cached_[11] = {};
    void buildCache(int stratum);
};

struct BlockRegistrar {
    explicit BlockRegistrar(BlockEntry e) { BlockRegistry::instance().add(e); }
};

} // namespace support

#define _BLOCK_REG_CONCAT(a, b) a##b
#define _BLOCK_REG_NAME(line) _BLOCK_REG_CONCAT(_block_reg_, line)
#define REGISTER_BLOCK(...) \
    static ::support::BlockRegistrar _BLOCK_REG_NAME(__LINE__)(__VA_ARGS__)
