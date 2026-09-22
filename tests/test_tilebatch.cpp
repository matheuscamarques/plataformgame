#include <cassert>
#include <cstdio>
#include <memory>

#include "core/EntityKind.h"
#include "world/Chunk.h"
#include "world/Tile.h"
#include "world/TileRenderer.h"

// Batch de tiles: 1 draw/chunk a partir das entidades-tile (CPU puro).
// Deco/árvores ficam fora; dirty dirige o rebuild.
int main() {
    using namespace support;

    { // IsTileKind (só visuais de tile entram no batch)
        assert(TileRenderer::isTileKind(core::kIdColide));
        assert(TileRenderer::isTileKind(core::kIdWater));
        assert(TileRenderer::isTileKind(core::kIdLava));
        assert(!TileRenderer::isTileKind(core::kIdTreeTrunk));
        assert(!TileRenderer::isTileKind(core::kIdTreeLeaf));
        assert(!TileRenderer::isTileKind(0));
    }
    { // RebuildEmpacotaTiles (3 tiles -> 12 vértices; árvore fora)
        Chunk c;
        auto add = [&](int kind, float x, float y, sf::Color col) {
            auto e = std::make_unique<Entity>(kind, x, y, 50.f, 50.f);
            e->setFillColor(col);
            c.entities.push_back(std::move(e));
        };
        add(core::kIdColide, 100.f, 200.f, sf::Color(128, 128, 128));
        add(core::kIdWater, 150.f, 200.f, sf::Color(0, 200, 255));
        add(core::kIdLava, 200.f, 200.f, sf::Color(255, 100, 20));
        add(core::kIdTreeTrunk, 250.f, 200.f, sf::Color(101, 67, 33));
        assert(c.tileDirty); // nasce dirty (geração constrói antes do draw)
        TileRenderer::rebuild(c);
        assert(!c.tileDirty);
        assert(c.tileVerts.getVertexCount() == 12);
        assert(c.tileVerts.getPrimitiveType() == sf::Quads);
        // Primeiro quad == primeiro tile (posição + cor idênticas ao draw).
        const sf::Vertex& v0 = c.tileVerts[0];
        const sf::Vertex& v2 = c.tileVerts[2];
        assert(v0.position == sf::Vector2f(100.f, 200.f));
        assert(v2.position == sf::Vector2f(150.f, 250.f));
        assert(v0.color == sf::Color(128, 128, 128));
        // Rebuild é determinístico (2× => mesmos bytes).
        const std::size_t n = c.tileVerts.getVertexCount();
        TileRenderer::rebuild(c);
        assert(c.tileVerts.getVertexCount() == n);
        assert(c.tileVerts[0].position == v0.position);
    }
    { // DirtyProtocol (setTile/remove marcam; rebuild limpa)
        Chunk c;
        TileRenderer::rebuild(c); // vazio: 0 vértices, limpa
        assert(c.tileVerts.getVertexCount() == 0);
        assert(!c.tileDirty);
        c.setTile(3, 4, Tile::Stone);
        assert(c.tileDirty);
        auto e = std::make_unique<Entity>(core::kIdColide, 0.f, 0.f, 50.f, 50.f);
        c.entities.push_back(std::move(e));
        TileRenderer::rebuild(c);
        assert(!c.tileDirty);
        assert(c.tileVerts.getVertexCount() == 4);
        assert(c.removeEntitiesAt(0, 0) == 1);
        assert(c.tileDirty); // sumiu do batch: rebuild pendente
        TileRenderer::rebuild(c);
        assert(c.tileVerts.getVertexCount() == 0);
    }

    { // ChunkNaoOrigem (world→local: entidade em (850,50) no chunk
        // (1,0) vira quad (50,50) — sem isso clippava no tileLayer 800
        // e só o chunk (0,0) desenhava; resto ficava transparente/preto)
        Chunk c;
        c.cx = 1;
        c.cy = 0;
        auto e = std::make_unique<Entity>(core::kIdColide, 850.f, 50.f,
                                          50.f, 50.f);
        e->setFillColor(sf::Color(128, 128, 128));
        c.entities.push_back(std::move(e));
        TileRenderer::rebuild(c);
        assert(c.tileVerts.getVertexCount() == 4);
        assert(c.tileVerts[0].position == sf::Vector2f(50.f, 50.f));
        assert(c.tileVerts[2].position == sf::Vector2f(100.f, 100.f));
        // Chunk negativo: (-50,-50) no chunk (-1,-1) vira (750,750).
        Chunk n;
        n.cx = -1;
        n.cy = -1;
        auto f = std::make_unique<Entity>(core::kIdColide, -50.f, -50.f,
                                          50.f, 50.f);
        n.entities.push_back(std::move(f));
        TileRenderer::rebuild(n);
        assert(n.tileVerts[0].position == sf::Vector2f(750.f, 750.f));
    }

    std::printf("tilebatch test OK\n");
    return 0;
}
