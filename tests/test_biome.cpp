/**
 * @file tests/test_biome.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava tabela clima para bioma e topo por bioma.
 * @details Cobre Generation em grade sintética, roda com make test que compila em build/tests/test_biome.
 */

#include <cassert>
#include <cstdio>
#include <initializer_list>
#include "world/Generation.h"

// Commit 2 biomas: tabela clima->bioma + topo por bioma.
int main() {
    using namespace support;

    // 1) Regras puras em grade sintética (40k+ checks, sem noise).
    int n = 0;
    for (int ti = 0; ti <= 200; ti++) {
        for (int hi = 0; hi <= 200; hi++) {
            float t = ti / 200.0f, h = hi / 200.0f;
            n++;
            // precedência: oceano > praia > clima
            assert(pickBiome(t, h, true, false) == Biome::Ocean);
            assert(pickBiome(t, h, true, true) == Biome::Ocean);
            assert(pickBiome(t, h, false, true) == Biome::Beach);
            if (t > T_HOT && h < H_DRY) assert(pickBiome(t, h, false, false) == Biome::Desert);
            else if (t > T_HOT && h < H_WET) assert(pickBiome(t, h, false, false) == Biome::Savanna);
            else if (t > T_HOT) assert(pickBiome(t, h, false, false) == Biome::Forest);
            else if (t > T_COLD && h < H_DRY) assert(pickBiome(t, h, false, false) == Biome::Grassland);
            else if (t > T_COLD) assert(pickBiome(t, h, false, false) == Biome::Forest);
            else if (h < H_DRY) assert(pickBiome(t, h, false, false) == Biome::Tundra);
            else assert(pickBiome(t, h, false, false) == Biome::Taiga);
            assert(pickBiome(t, h, false, false) == pickBiome(t, h, false, false));
        }
    }
    std::printf("regras puras OK (%d checks)\n", n);

    // 2) Fronteiras: '>' estrito, == cai na faixa de baixo.
    assert(pickBiome(T_HOT, 0.0f, false, false) == Biome::Grassland);
    assert(pickBiome(T_COLD, 0.0f, false, false) == Biome::Tundra);
    assert(pickBiome(1.0f, H_DRY, false, false) == Biome::Savanna);
    assert(pickBiome(1.0f, H_WET, false, false) == Biome::Forest);
    assert(pickBiome(T_COLD, H_WET, false, false) == Biome::Taiga);

    // 3) Todos os 8 biomas aparecem no mundo real (range largo).
    bool seen[8] = {};
    bool seenGrassTop = false;
    for (uint32_t seed : {1337u, 999u, 42u}) {
        for (int tx = -4000; tx < 4000; tx += 8) {
            int s = surfaceHeight(tx, seed);
            bool ocean = isOceanColumn(tx, seed);
            Biome b = pickBiome(temperature(tx, s, seed), humidity(tx, s, seed),
                                ocean, isCoastal(s));
            int idx = -1;
            switch (b) {
                case Biome::Ocean: idx = 0; break;
                case Biome::Beach: idx = 1; break;
                case Biome::Desert: idx = 2; break;
                case Biome::Savanna: idx = 3; break;
                case Biome::Grassland: idx = 4; break;
                case Biome::Forest: idx = 5; break;
                case Biome::Taiga: idx = 6; break;
                case Biome::Tundra: idx = 7; break;
                default: break;
            }
            if (idx >= 0) seen[idx] = true;
            // topo do mundo == topo do bioma (ou neve nos picos,
            // ou ar onde o verme abre boca).
            Tile expected = wormMouth(tx, s, seed)
                              ? (ocean ? Tile::Water : Tile::Air)
                          : snowcap(s)            ? Tile::Snow
                                                  : biomeTopTile(b);
            assert(tileType(tx, s, seed) == expected);
            if (expected == Tile::Grass) seenGrassTop = true;
        }
    }
    for (int i = 0; i < 8; i++) assert(seen[i]);
    std::printf("biomas presentes OK (8/8)\n");
    assert(seenGrassTop); // grama existe no mundo

    std::printf("biome test OK\n");
    return 0;
}
