#include <cassert>
#include <cstdio>
#include <string_view>
#include "world/Block.h"

// Tabela completa: todo Tile tem nome e cor (menos Air).
// Bloco novo sem entrada aqui passa vazio pra produção.
int main() {
    using namespace support;

    for (std::size_t i = 0; i < TILE_COUNT; i++) {
        Tile t = static_cast<Tile>(i);
        const BlockDef &def = blockDef(t);
        assert(def.name != nullptr && std::string_view(def.name).size() > 0);
        if (def.kind == BlockKind::Air) continue; // sem cor (inclui unused_*)
        bool black = def.color.r == 0 && def.color.g == 0 && def.color.b == 0;
        assert(!black); // cor ausente? adiciona na tabela
    }

    // unknown cai no fallback Air
    assert(blockDef(static_cast<Tile>(255)).kind == BlockKind::Air);
    assert(blockDefByName("stone") != nullptr);
    assert(blockDefByName("nao_existe") == nullptr);

    // kinds coerentes com o jogo
    assert(isSolid(Tile::Stone) && !isSolid(Tile::Air) && !isSolid(Tile::Water));
    assert(isLiquid(Tile::Water) && !isLiquid(Tile::Stone));
    assert(isDeco(Tile::TreeTrunk) && !isDeco(Tile::Dirt));

    std::printf("blocks test OK (%zu tiles)\n", TILE_COUNT);
    return 0;
}
