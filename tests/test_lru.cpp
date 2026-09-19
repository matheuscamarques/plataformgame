#include <cassert>
#include <cstdio>
#include "support/World/ChunkManager.h"

// BUG1 da review: update() descartava chunk modificado sem checar.
int main() {
    using namespace support;

    // Teleporta para longe: chunk modificado sobrevive, limpo cai.
    {
        ChunkManager m(1337u, 2);
        m.update(0, 0);
        assert(m.loadedCount() == 25);
        Chunk *c = m.find(0, 0);
        assert(c != nullptr && !c->modified());
        c->setTile(0, 0, 1); // modificação de Feature (destruir bloco)
        assert(c->modified() && m.modifiedCount() == 1);

        m.update(5000, 5000);
        assert(m.find(0, 0) != nullptr); // retido por estar modificado
        assert(m.find(1, 1) == nullptr); // limpo descartado
        assert(m.modifiedCount() == 1);
    }

    // Evict com teto baixo: limpos caem até o teto, modificado fica.
    {
        ChunkManager m(1337u, 1, 4);
        m.update(0, 0); // 9 gerados, teto 4
        assert(m.loadedCount() == 4);
        auto loaded = m.loaded();
        assert(!loaded.empty());
        int cx = loaded.front()->cx, cy = loaded.front()->cy;
        loaded.front()->setTile(0, 0, 1);
        m.update(5000, 5000);
        assert(m.find(cx, cy) != nullptr); // modificado retido
        assert(m.modifiedCount() == 1);
    }

    std::printf("lru test OK\n");
    return 0;
}
