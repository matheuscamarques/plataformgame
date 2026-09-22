#include <cassert>
#include <cstdio>

#include "core/Config.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Effects/Throwable.h"
#include "support/GameContext.h"
#include "world/Chunk.h"
#include "world/Tile.h"
#include "world/World.h"

// TNT como fonte de grid (item 15): dinamite com fuse aceso escreve
// blockLight no chunk; ao explodir, o grid zera. Headless (sem GL).
int main() {
    using namespace support;

    World world(1337u);
    world.update(0, 0);
    Chunk* c = world.findChunk(0, 0);
    assert(c);
    for (auto& t : c->tiles) t = Tile::Air; // queda livre de obstáculos

    ThrowSystem throws;
    GameContext ctx{};
    ctx.world = &world;

    // Dinamite estática no tile (8,8): sem gravidade p/ não sair do chunk.
    Throwable* t = throws.throwItem({8 * core::kBlockSize + 25.f,
                                     8 * core::kBlockSize + 25.f},
                                    {0.f, 0.f}, ThrowKind::Dynamite);
    assert(t && t->active);
    t->gravity = 0.f;
    assert(t->fuse > 0.f);

    for (int i = 0; i < 5; ++i) throws.tick(1.f / 60.f, ctx);
    // Fuse ~1.0 → heat 0 → nível 6 na fonte.
    assert(c->blockLight[8 * Chunk::W + 8] == 6);
    assert(c->blockLight[8 * Chunk::W + 9] > 0); // decaiu p/ o vizinho

    // Avança até explodir (fuse 1.0 a 60fps) e confirma a limpeza.
    for (int i = 0; i < 120 && throws.activeCount() > 0; ++i)
        throws.tick(1.f / 60.f, ctx);
    assert(throws.activeCount() == 0); // explodiu
    int sum = 0;
    for (auto v : c->blockLight) sum += v;
    assert(sum == 0); // grid zerado com o objeto

    std::printf("tnt_light test OK\n");
    return 0;
}
