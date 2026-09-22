#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "core/Config.h"
#include "world/Chunk.h"
#include "world/LightPropagator.h"
#include "world/RaycastLight.h"
#include "world/Tile.h"
#include "world/World.h"

// Luz cross-chunk do player: cast único global + push outward até
// fixpoint. Headless puro (grids + sf::Image CPU; sem GL).
// World(seed)+update() é headless-safe (precedente: test_culling).
namespace {

constexpr int W = support::Chunk::W;
constexpr int H = support::Chunk::H;

float px(int tx) {
    return tx * core::kBlockSize + core::kBlockSize * 0.5f;
}

void sculptAir(support::World& w, int cx, int cy) {
    support::Chunk* c = w.findChunk(cx, cy);
    assert(c);
    for (auto& t : c->tiles) t = support::Tile::Air;
}

} // namespace

int main() {
    using namespace support;

    { // T1 RayAtravessaFronteira (máscara cruza o chunk; cast local parava)
        World w(1337u);
        w.update(24, 8);
        sculptAir(w, 0, 0);
        sculptAir(w, 1, 0);
        RaycastLight::castRaysCrossChunk([&](int cx, int cy) { return w.findChunk(cx, cy); },
                                                 {px(14), px(8)}, 8);
        Chunk* a = w.findChunk(0, 0);
        Chunk* b = w.findChunk(1, 0);
        assert(a && b);
        assert(a->visibleMask[8 * W + 14] == 1); // fonte visível
        assert(b->visibleMask[8 * W + 0] == 1);  // atravessou a fronteira
    }
    { // T2 DecaimentoAtravessa (8,7,6...2,0 — sem corte na borda)
        World w(1337u);
        w.update(24, 8);
        sculptAir(w, 0, 0);
        sculptAir(w, 1, 0);
        LightPropagator::addBlockSourceAt([&](int cx, int cy) { return w.findChunk(cx, cy); },
                                                 px(14), px(8), 8);
        Chunk* a = w.findChunk(0, 0);
        Chunk* b = w.findChunk(1, 0);
        assert(a && b);
        assert(a->blockLight[8 * W + 14] == 8); // fonte
        assert(a->blockLight[8 * W + 15] == 7); // decai 1
        assert(b->blockLight[8 * W + 0] == 6);  // atravessa decaindo
        assert(b->blockLight[8 * W + 4] == 2);  // decai mais
        assert(b->blockLight[8 * W + 7] == 0);  // alcance acabou (8-9<0)
    }
    { // T3 ParedeNaFronteiraCorta (visível-mas-corta; nada vaza p/ b)
        World w(1337u);
        w.update(24, 8);
        for (int cy : {-1, 0, 1}) {
            sculptAir(w, 0, cy);
            Chunk* c = w.findChunk(0, cy);
            assert(c);
            for (int y = 0; y < H; ++y)
                c->tiles[y * W + 15] = Tile::Stone; // barreira 3 chunks
        }
        sculptAir(w, 1, 0);
        LightPropagator::addBlockSourceAt([&](int cx, int cy) { return w.findChunk(cx, cy); },
                                                 px(8), px(8), 8);
        Chunk* a = w.findChunk(0, 0);
        Chunk* b = w.findChunk(1, 0);
        assert(a && b);
        assert(a->blockLight[8 * W + 8] == 8);  // fonte
        assert(a->visibleMask[8 * W + 15] == 1); // parede visível...
        assert(b->visibleMask[8 * W + 0] == 0);  // ...mas nada passa
        assert(b->blockLight[8 * W + 0] == 0);
    }
    { // T4 StaleLimpa (união old∪new: máscara + grid do cast anterior somem)
        World w(1337u);
        w.update(24, 8);
        for (int cx : {-1, 0, 1, 2, 3}) sculptAir(w, cx, 0);
        LightPropagator::addBlockSourceAt([&](int cx, int cy) { return w.findChunk(cx, cy); },
                                                 px(2), px(8), 5);
        assert(w.findChunk(0, 0)->visibleMask[8 * W + 2] == 1);
        assert(w.findChunk(0, 0)->blockLight[8 * W + 2] == 5);
        // Boxes disjuntas ([-1,1] vs [2,4]): chunk 0 só existe no passado.
        LightPropagator::addBlockSourceAt([&](int cx, int cy) { return w.findChunk(cx, cy); },
                                                 px(48), px(8), 5);
        assert(w.findChunk(0, 0)->visibleMask[8 * W + 2] == 0); // stale limpo
        assert(w.findChunk(0, 0)->blockLight[8 * W + 2] == 0);  // sem fantasma
        assert(w.findChunk(3, 0)->blockLight[8 * W + 0] == 5);  // fonte nova
        assert(w.findChunk(3, 0)->visibleMask[8 * W + 0] == 1);
    }
    { // T5 Fixpoint3Passadas (reentrada em U: 1 passada não basta)
        // Fonte em (1,1)(1,1); alvo em (0,0)(15,15) só via B=(1,0) e
        // D=(1,1)→C=(0,1): ordem (cy,cx) processa C antes de D, então o
        // valor só chega em C na 2ª passada e o retorno fecha na 3ª.
        // Tudo Air (visibilidade total): puro teste de convergência.
        World w(1337u);
        w.update(8, 8);
        sculptAir(w, 0, 0);
        sculptAir(w, 1, 0);
        sculptAir(w, 0, 1);
        sculptAir(w, 1, 1);
        LightPropagator::addBlockSourceAt([&](int cx, int cy) { return w.findChunk(cx, cy); },
                                                 px(17), px(17), 10);
        // Manhattan (17,17)→(15,15) = 4 passos ⇒ 10-4 = 6.
        assert(w.findChunk(0, 0)->blockLight[15 * W + 15] == 6);
    }
    { // T6 Determinismo (duas Worlds idênticas ⇒ grids idênticos)
        auto run = []() {
            World w(1337u);
            w.update(8, 8);
            sculptAir(w, 0, 0);
            sculptAir(w, 1, 0);
            sculptAir(w, 0, 1);
            sculptAir(w, 1, 1);
            Chunk* c = w.findChunk(1, 0);
            assert(c);
            for (int y = 4; y < 12; ++y)
                c->tiles[y * W + 8] = Tile::Stone; // pilar p/ sombra
            LightPropagator::addBlockSourceAt([&](int cx, int cy) { return w.findChunk(cx, cy); },
                                                 px(17), px(17), 10);
            std::vector<uint8_t> out;
            for (auto [cx, cy] : {std::pair{0, 0}, {1, 0}, {0, 1}, {1, 1}}) {
                Chunk* k = w.findChunk(cx, cy);
                assert(k);
                out.insert(out.end(), k->blockLight.begin(),
                           k->blockLight.end());
            }
            return out;
        };
        assert(run() == run());
    }
    { // T7 CosturaVisual (buildLightImage 32×32: sem degrau na borda)
        Chunk a, b;
        for (int y = 0; y < H; ++y) {
            a.blockLight[y * W + 15] = 0;
            b.blockLight[y * W + 0] = 15;
        }
        sf::Image imgA =
            LightPropagator::buildLightImage(a, nullptr, &b, nullptr, nullptr);
        sf::Image imgB =
            LightPropagator::buildLightImage(b, &a, nullptr, nullptr, nullptr);
        assert(imgA.getSize().x == 16u * LightPropagator::kLightmapScale);
        const unsigned ex = imgA.getSize().x - 1;
        const unsigned mid = imgA.getSize().y / 2;
        const int bA = imgA.getPixel(ex, mid).r;
        const int bB = imgB.getPixel(0, mid).r;
        assert(bA > 0);                // vê luz do vizinho
        assert(std::abs(bA - bB) < 30); // suave, sem degrau
    }

    { // T8 CantoDiagonal (blur não mistura 0 onde há chunk diagonal)
        Chunk q, r, s, d;
        for (int i = 0; i < W * H; ++i) {
            q.skyLight[i] = 15;
            r.skyLight[i] = 15;
            s.skyLight[i] = 15;
            d.skyLight[i] = 15;
        }
        // Sem diagonais: o canto afunda (zeros no blur) — regressão travada.
        sf::Image imgNo = LightPropagator::buildLightImage(q, nullptr, &r,
                                                           nullptr, &s);
        const unsigned ex = imgNo.getSize().x - 1;
        const unsigned ey = imgNo.getSize().y - 1;
        assert(imgNo.getPixel(ex, ey).r < 255);
        // Com a diagonal (bottomRight=d): canto cheio, sem artefato.
        sf::Image img = LightPropagator::buildLightImage(q, nullptr, &r,
                                                         nullptr, &s, nullptr,
                                                         nullptr, nullptr, &d);
        assert(img.getPixel(ex, ey).r == 255);
    }

    std::printf("light crosschunk test OK\n");
    return 0;
}
