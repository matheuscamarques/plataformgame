/**
 * @file src/world/RaycastLight.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Marca tiles visíveis por raios com oclusão em chunk.
 * @details Implementa castRays com DDA de passo 0.5, 96 raios e alcance 16, mais variante cross-chunk em pixels de mundo, usado pelo LightPropagator para sombra dura.
 */

#include "world/RaycastLight.h"
#include "world/Chunk.h"
#include "world/Tile.h"
#include "core/Config.h"

namespace support {

namespace {
constexpr float kTwoPi = 6.28318530718f;

// Bloqueia raio: mesma regra do BFS (só Ar e Água passam).
inline bool blocksRay(Tile t) {
    return t != Tile::Air && t != Tile::Water;
}
} // namespace

void RaycastLight::castRays(Chunk& c, float ox, float oy) {
    c.clearVisibility();

    const int oxi = static_cast<int>(std::floor(ox));
    const int oyi = static_cast<int>(std::floor(oy));
    if (c.inBounds(oxi, oyi)) {
        c.visibleMask[oyi * Chunk::W + oxi] = 1;
    }

    const float angleStep = kTwoPi / kRaysPerLight;

    for (int r = 0; r < kRaysPerLight; ++r) {
        const float angle = r * angleStep;
        const float dx = std::cos(angle);
        const float dy = std::sin(angle);

        float x = ox;
        float y = oy;

        const int maxSteps = static_cast<int>(kMaxRayDist / kStep) + 2;
        for (int step = 0; step < maxSteps; ++step) {
            x += dx * kStep;
            y += dy * kStep;

            const int tx = static_cast<int>(std::floor(x));
            const int ty = static_cast<int>(std::floor(y));

            if (tx < 0 || ty < 0 || tx >= Chunk::W || ty >= Chunk::H) break;

            const int i = ty * Chunk::W + tx;
            c.visibleMask[i] = 1; // parede é visível, mas corta o resto
            if (blocksRay(c.tiles[i])) break;
        }
    }
}

int RaycastLight::visibleCount(const Chunk& c) {
    int n = 0;
    for (uint8_t v : c.visibleMask) if (v) ++n;
    return n;
}

void RaycastLight::castRaysCrossChunk(ChunkProvider findChunk,
                                      core::Vec2f oPx, int level) {
    if (level <= 0) return;
    const float bs = static_cast<float>(core::kBlockSize);
    const int maxDist = level + 2;        // folga p/ borda do alcance
    const int maxSteps = maxDist * 2 + 2; // passo 0.5 tile
    const int R = maxDist / Chunk::W + 1; // raio em chunks

    const int otx = static_cast<int>(std::floor(oPx.x / bs));
    const int oty = static_cast<int>(std::floor(oPx.y / bs));
    const auto chunkOf = [](int t, int n) {
        return static_cast<int>(std::floor(static_cast<float>(t) /
                                           static_cast<float>(n)));
    };
    const int ocx = chunkOf(otx, Chunk::W);
    const int ocy = chunkOf(oty, Chunk::H);

    // 1. Limpa máscaras da área (chunks existentes).
    for (int cy = ocy - R; cy <= ocy + R; ++cy)
        for (int cx = ocx - R; cx <= ocx + R; ++cx)
            if (Chunk* c = findChunk(cx, cy))
                c->clearVisibility();

    // 2. Origem sempre visível (mesmo em sólido).
    Chunk* src = findChunk(ocx, ocy);
    if (!src) return; // fonte descarregada: nada a iluminar
    const int olx = otx - ocx * Chunk::W;
    const int oly = oty - ocy * Chunk::H;
    if (!src->inBounds(olx, oly)) return;
    src->visibleMask[oly * Chunk::W + olx] = 1;

    // 3. 96 raios cross-chunk (mesma regra do cast local).
    const float angleStep = kTwoPi / kRaysPerLight;
    for (int r = 0; r < kRaysPerLight; ++r) {
        const float angle = r * angleStep;
        const float dx = std::cos(angle);
        const float dy = std::sin(angle);

        float x = static_cast<float>(otx) + 0.5f; // tile coords de mundo
        float y = static_cast<float>(oty) + 0.5f;

        for (int step = 0; step < maxSteps; ++step) {
            x += dx * kStep;
            y += dy * kStep;

            const int tx = static_cast<int>(std::floor(x));
            const int ty = static_cast<int>(std::floor(y));
            Chunk* c = findChunk(chunkOf(tx, Chunk::W),
                                   chunkOf(ty, Chunk::H));
            if (!c) break; // vazio descarregado: encerra, não atravessa
            const int lx = tx - c->cx * Chunk::W;
            const int ly = ty - c->cy * Chunk::H;
            if (!c->inBounds(lx, ly)) break;

            const int i = ly * Chunk::W + lx;
            c->visibleMask[i] = 1; // parede é visível, mas corta o resto
            if (blocksRay(c->tiles[i])) break;
        }
    }
}

} // namespace support
