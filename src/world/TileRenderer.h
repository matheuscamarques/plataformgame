#pragma once
#include <SFML/Graphics/RenderTarget.hpp>

namespace support {

struct Chunk;

// Batch de tiles estáticos: 1 draw call por chunk em vez de 1 por tile.
// Fonte: entidades-tile (kinds colide/água/lava) — mesmos rects e cores
// do draw individual, empacotados em quads. Árvores/deco continuam
// individuais (poucas por chunk, fora do tiles[]).
// rebuild() é CPU puro (VertexArray): headless-safe; upload() precisa
// de GL (RenderTexture) e só roda no render. Camada 5 desenha o sprite
// da camada pré-renderizada em vez de re-enviar vértices por frame.
class TileRenderer {
public:
    // CONTRATO: rebuild() produz vértices em coords LOCAIS do chunk
    // ([0, W*cs) × [0, H*cs)). Entities têm coords WORLD; a conversão
    // é feita dentro do rebuild subtraindo cx*W*cs, cy*H*cs.
    // drawLayer() soma cx*W*cs, cy*H*cs de volta.
    // Reconstrói tileVerts do chunk; limpa tileDirty. Chamar quando dirty.
    static void rebuild(Chunk& c);
    // Sobe tileVerts p/ tileLayer (cria 800×800 sob demanda). Só no render.
    static void upload(Chunk& c);
    // Desenha o sprite da camada (view de mundo ativa, coords de mundo).
    static void drawLayer(const Chunk& c, sf::RenderTarget& target);

    // Kinds cobertos pelo batch (visuais de tile; ver core/EntityKind.h).
    static bool isTileKind(int kind);
};

} // namespace support
