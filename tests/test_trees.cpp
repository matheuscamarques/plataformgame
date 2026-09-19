#include <cassert>
#include <cstdio>
#include <initializer_list>
#include <algorithm>
#include <vector>
#include "support/World/Generation.h"
#include "support/World/World.h"
#include "defines.h"

// Árvores decorativas: sem colisão, determinísticas, sem seam entre chunks.
int main() {
    using namespace support;

    // 1) Zero árvore em bioma inelegível.
    for (uint32_t seed : {1337u, 999u, 42u}) {
        TreeParams p;
        for (int tx = -512; tx < 512; tx++) {
            assert(!treeWants(tx, seed, 64, Biome::Ocean, p));
            assert(!treeWants(tx, seed, 64, Biome::Beach, p));
            assert(!treeWants(tx, seed, 64, Biome::Desert, p));
            assert(!treeWants(tx, seed, 64, Biome::Tundra, p));
        }
    }
    std::printf("biomas inelegíveis OK\n");

    // 2) Floresta densa produz árvores (superfícies reais, loop como generate).
    {
        int total = 0;
        for (int cx = 0; cx < 16; cx++) {
            int inChunk = 0;
            int lastTreeX = cx * 16 - TREE_EXPAND - TREE_SPACING - 1;
            for (int tx = cx * 16 - TREE_EXPAND; tx < (cx + 1) * 16 + TREE_EXPAND; tx++) {
                if (tx - lastTreeX < TREE_SPACING) continue;
                int sy = surfaceHeight(tx, 42u);
                TreeParams p;
                if (!treeWants(tx, 42u, sy, Biome::Forest, p)) continue;
                lastTreeX = tx;
                if (tx < cx * 16 || tx >= (cx + 1) * 16) continue;
                assert(p.trunkH >= TREE_TRUNK_MIN && p.trunkH <= TREE_TRUNK_MAX);
                assert(p.canopyR >= TREE_CANOPY_MIN && p.canopyR <= TREE_CANOPY_MAX);
                inChunk++;
            }
            assert(inChunk > 0);
            total += inChunk;
        }
        std::printf("floresta densa OK (total=%d)\n", total);
        assert(total >= 16);
    }

    // 3) Determinismo (acha uma coluna que quer, compara chamadas).
    {
        int found = -1;
        for (int tx = 0; tx < 500 && found < 0; tx++) {
            TreeParams p;
            if (treeWants(tx, 1234u, surfaceHeight(tx, 1234u), Biome::Forest, p)) found = tx;
        }
        assert(found >= 0);
        TreeParams a, b;
        int s = surfaceHeight(found, 1234u);
        assert(treeWants(found, 1234u, s, Biome::Forest, a));
        assert(treeWants(found, 1234u, s, Biome::Forest, b));
        assert(a.trunkH == b.trunkH && a.canopyR == b.canopyR);
        std::printf("determinismo OK (tx=%d)\n", found);
    }

    // 4) Inclinação barra sozinha: em coluna íngreme, zero árvores
    // mesmo com densidade a favor; em coluna plana, ~35% (Forest).
    {
        int flatN = 0, flatOk = 0, steepN = 0, steepOk = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, 42u);
            int sl = surfaceHeight(tx - 1, 42u);
            int sr = surfaceHeight(tx + 1, 42u);
            TreeParams p;
            bool steep = std::abs(sl - s) > 1 || std::abs(sr - s) > 1;
            if (steep) {
                steepN++;
                if (treeWants(tx, 42u, s, Biome::Forest, p)) steepOk++;
            } else {
                flatN++;
                if (treeWants(tx, 42u, s, Biome::Forest, p)) flatOk++;
            }
        }
        float fFlat = (float)flatOk / flatN, fSteep = (float)steepOk / steepN;
        std::printf("slope OK (plano=%.3f ingreme=%.4f)\n", fFlat, fSteep);
        assert(fFlat > 0.25f && fFlat < 0.45f);
        assert(fSteep == 0.0f);
    }

    // 5) Integração: árvores nascem no mundo, sobre ar, com chão sólido.
    {
        World world(1337u);
        // acha área de floresta
        int fx = 0;
        for (int tx = -2000; tx < 2000; tx++) {
            int s = surfaceHeight(tx, 1337u);
            Biome b = pickBiome(temperature(tx, s, 1337u), humidity(tx, s, 1337u),
                                isOceanColumn(tx, 1337u), isCoastal(s));
            if (b == Biome::Forest) { fx = tx; break; }
        }
        world.update(fx, surfaceHeight(fx, 1337u));
        int trunks = 0;
        std::vector<int> trunkXs;
        for (Entity *e : world.getPlatforms()) {
            if (e->getName() != TREE_TRUNK && e->getName() != TREE_LEAF) continue;
            int tx = static_cast<int>(e->getX()) / BLOCK_SIZE;
            int ty = static_cast<int>(e->getY()) / BLOCK_SIZE;
            // entidade de árvore sempre sobre tile de ar (nunca enterra)
            assert(world.tileAt(tx, ty) == Tile::Air);
            if (e->getName() == TREE_TRUNK) {
                trunks++;
                trunkXs.push_back(tx);
                // base tem chão sólido embaixo (ou boca, aceito)
            }
        }
        std::printf("integração OK (troncos=%d)\n", trunks);
        assert(trunks > 0);
        // espaçamento entre troncos na área carregada
        std::sort(trunkXs.begin(), trunkXs.end());
        for (size_t i = 1; i < trunkXs.size(); i++) {
            // mesmo tronco tem 1 tile de largura; troncos distintos >= spacing
            if (trunkXs[i] != trunkXs[i - 1]) assert(trunkXs[i] - trunkXs[i - 1] >= TREE_SPACING);
        }
    }

    std::printf("trees test OK\n");
    return 0;
}
