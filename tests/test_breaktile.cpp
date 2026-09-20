#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>

#include "defines.h"
#include "entities/entity/entity.hpp"
#include "world/Block.h"
#include "world/World.h"

// Regressão do segfault ao apertar J: breakTile destruía a Entity do
// chunk mas deixava o ponteiro em activePlatforms_/activeColides_ até
// o próximo update() — o render do mesmo frame dereferenciava.
// Nenhum deref pós-quebra aqui: só comparação de endereços.
int main() {
    using namespace support;

    { // BreakExpungesActiveLists
        World world(1337u);
        world.update(0, 26);

        // Acha um tile quebrável que tenha entidade (deref ANTES: seguro).
        int btx = 0, bty = 0;
        bool found = false;
        for (Entity *e : world.getPlatforms()) {
            const int tx = static_cast<int>(e->getX() / BLOCK_SIZE);
            const int ty = static_cast<int>(e->getY() / BLOCK_SIZE);
            const Tile t = world.tileAt(tx, ty);
            if (t == Tile::Air || t == Tile::Bedrock || isLiquid(t)) continue;
            btx = tx;
            bty = ty;
            found = true;
            break;
        }
        assert(found);

        // Conjunto R: endereços das entidades exatamente neste tile.
        std::vector<Entity *> area;
        world.query(btx * BLOCK_SIZE - 1.f, bty * BLOCK_SIZE - 1.f,
                    BLOCK_SIZE + 2.f, BLOCK_SIZE + 2.f, area);
        std::vector<Entity *> removed;
        for (Entity *e : area) {
            const int ex = static_cast<int>(e->getX() / BLOCK_SIZE);
            const int ey = static_cast<int>(e->getY() / BLOCK_SIZE);
            if (ex == btx && ey == bty) removed.push_back(e);
        }
        assert(!removed.empty());

        const std::size_t before = world.getPlatforms().size();
        assert(world.breakTile(btx, bty));
        const auto &after = world.getPlatforms();
        assert(after.size() == before - removed.size());
        for (Entity *r : removed) {
            assert(std::find(after.begin(), after.end(), r) == after.end());
        }
    }
    { // BreakAirKeepsLists (falso + listas intactas)
        World world(1337u);
        world.update(0, 26);
        const std::size_t before = world.getPlatforms().size();
        // Céu acima do spawn é ar em qualquer seed razoável; se não for,
        // o assert de !ok falha e o teste avisa em vez de passar à toa.
        bool ok = world.breakTile(0, -500);
        assert(!ok);
        assert(world.getPlatforms().size() == before);
    }

    std::printf("breaktile test OK\n");
    return 0;
}
