#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
#include "support/World/ChunkManager.h"

// Snapshot textual de um chunk: tiles + posição/tipo das entidades.
static std::string snapshot(support::Chunk *c) {
    std::string s;
    s += std::to_string(c->cx) + "," + std::to_string(c->cy) + ";";
    for (int t : c->tiles) { s += std::to_string(t) + ","; }
    s += ";";
    for (auto &e : c->entities) {
        s += std::to_string((int)e->getX()) + "," + std::to_string((int)e->getY()) +
             "," + std::to_string(e->getName()) + ";";
    }
    return s;
}

static support::Chunk *find(std::vector<support::Chunk*> v, int cx, int cy) {
    for (support::Chunk *c : v)
        if (c->cx == cx && c->cy == cy) return c;
    return nullptr;
}

int main() {
    support::ChunkManager m(1337u, 2);
    m.update(0, 0);
    // raio 2 -> 5x5 = 25 chunks
    assert(m.loadedCount() == 25);

    std::string before = snapshot(find(m.loaded(), 0, 0));
    assert(!before.empty());
    std::string negBefore = snapshot(find(m.loaded(), -2, -2));
    assert(!negBefore.empty());

    // Teleporta para longe: descarrega tudo do spawn e carrega outra área.
    m.update(5000, -3000);
    assert(m.loadedCount() == 25);
    assert(find(m.loaded(), 0, 0) == nullptr);

    // Volta: conteúdo tem que ser idêntico.
    m.update(0, 0);
    assert(m.loadedCount() == 25);
    assert(snapshot(find(m.loaded(), 0, 0)) == before);
    assert(snapshot(find(m.loaded(), -2, -2)) == negBefore);

    // Seed diferente gera mundo diferente (compara maciço, não céu:
    // céu pode ser identicamente vazio nas duas seeds).
    support::ChunkManager other(999u, 0);
    other.update(0, 40); // chunk (0,2): maciço
    assert(other.loadedCount() == 1);
    assert(snapshot(find(other.loaded(), 0, 2)) != snapshot(find(m.loaded(), 0, 2)));

    // Chunks têm 256 tiles; algum chunk carregado tem entidades
    // (o (0,0) pode ser céu vazio sobre o oceano — sem plataformas lá).
    support::Chunk *c0 = find(m.loaded(), 0, 0);
    assert((int)c0->tiles.size() == 256);
    bool anyEntities = false;
    for (support::Chunk *c : m.loaded())
        if (!c->entities.empty()) { anyEntities = true; break; }
    assert(anyEntities);

    std::printf("chunk reload determinism OK (loaded=%zu)\n", m.loadedCount());
    return 0;
}
