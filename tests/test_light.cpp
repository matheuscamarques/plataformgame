/**
 * @file tests/test_light.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava luz por tile em grids sem GL.
 * @details Cobre LightPropagator e Chunk, roda com make test que compila em build/tests/test_light.
 */

#include <cassert>
#include <cstdio>

#include "world/Chunk.h"
#include "world/LightPropagator.h"
#include "world/Tile.h"

// Luz por tile: só grids (sem GL — updateTexture nunca é chamado aqui).
// Convenção support:: (Chunk/Tile moram lá, não em world::).
namespace {
// Chunk não copia (SpatialHash/unique_ptr/Texture): constrói in-place.
// tiles já nascem Air no ctor.
void clearLight(support::Chunk& c) {
    c.skyLight.assign(support::Chunk::W * support::Chunk::H, 0);
    c.blockLight.assign(support::Chunk::W * support::Chunk::H, 0);
}
} // namespace

int main() {
    using namespace support;

    { // SkyFullAboveGround (sem sólido: tudo 15)
        Chunk c;
        LightPropagator::computeSkyLight(c, nullptr, nullptr, nullptr, nullptr, 15);
        for (int y = 0; y < Chunk::H; ++y)
            for (int x = 0; x < Chunk::W; ++x)
                assert(c.skyLight[y * Chunk::W + x] == 15);
    }
    { // SkyWrapsAroundNarrowSolid (bloco 5-wide: luz contorna, não zera)
        // Físico correto (igual Minecraft): sombra total só sob teto
        // largo; bloco estreito vaza luz por baixo (14/13/12...).
        Chunk c;
        for (int x = 6; x <= 10; ++x)
            c.tiles[5 * Chunk::W + x] = Tile::Stone;
        LightPropagator::computeSkyLight(c, nullptr, nullptr, nullptr, nullptr, 15);
        assert(c.skyLight[4 * Chunk::W + 8] == 15);
        assert(c.skyLight[6 * Chunk::W + 8] == 12); // contorna: 15-3
    }
    { // SkyStopsUnderFullRoof (teto total: embaixo zera)
        Chunk c;
        for (int x = 0; x < Chunk::W; ++x)
            c.tiles[5 * Chunk::W + x] = Tile::Stone;
        LightPropagator::computeSkyLight(c, nullptr, nullptr, nullptr, nullptr, 15);
        assert(c.skyLight[4 * Chunk::W + 8] == 15);
        assert(c.skyLight[6 * Chunk::W + 8] == 0);
    }
    { // SkyFromTopEdge (chunk fundo: borda de cima manda, não 15)
        Chunk c;
        Chunk top; // todo ar → borda de baixo = 15
        LightPropagator::computeSkyLight(top, nullptr, nullptr, nullptr, nullptr, 15);
        LightPropagator::computeSkyLight(c, &top, nullptr, nullptr, nullptr, 15);
        assert(c.skyLight[0] == 15);
        // Topo sólido no vizinho de cima: borda 0 → coluna escura.
        for (int x = 0; x < Chunk::W; ++x)
            top.tiles[(Chunk::H - 1) * Chunk::W + x] = Tile::Stone;
        LightPropagator::computeSkyLight(top, nullptr, nullptr, nullptr, nullptr, 15);
        LightPropagator::computeSkyLight(c, &top, nullptr, nullptr, nullptr, 15);
        assert(c.skyLight[0] == 0);
    }
    { // SkySpreadsLaterally (beiral: furo em x=15, decai 1 por tile)
        // Teto sólido na linha 0 com furo na borda direita: embaixo do
        // furo é 15, e cada passo lateral perde 1 (parede lateral pura
        // NÃO escurece — fill vertical já dá 15 em coluna de ar).
        Chunk c;
        for (int x = 0; x < Chunk::W; ++x)
            c.tiles[0 * Chunk::W + x] = Tile::Stone;
        c.tiles[0 * Chunk::W + 15] = Tile::Air;
        LightPropagator::computeSkyLight(c, nullptr, nullptr, nullptr, nullptr, 15);
        assert(c.skyLight[1 * Chunk::W + 15] == 15);
        assert(c.skyLight[1 * Chunk::W + 11] == 11);
        assert(c.skyLight[1 * Chunk::W + 1] == 1);
    }
    { // CaveWithEntrance (teto com buraco: luz entra, longe fica escuro)
        Chunk c;
        for (int x = 0; x < Chunk::W; ++x)
            c.tiles[5 * Chunk::W + x] = Tile::Stone;
        c.tiles[5 * Chunk::W + 8] = Tile::Air;
        LightPropagator::computeSkyLight(c, nullptr, nullptr, nullptr, nullptr, 15);
        assert(c.skyLight[6 * Chunk::W + 8] == 15);
        assert(c.skyLight[6 * Chunk::W + 0] < 8);
    }
    { // BlockSourceDecays (fonte 10: 10, 9, ..., 6)
        Chunk c;
        LightPropagator::addBlockSource(c, 8, 8, 10);
        assert(c.blockLight[8 * Chunk::W + 8] == 10);
        assert(c.blockLight[8 * Chunk::W + 9] == 9);
        assert(c.blockLight[8 * Chunk::W + 12] == 6);
    }
    { // BlockSourceBlockedByWall (parede na col 10: lado longe escuro)
        Chunk c;
        for (int y = 0; y < Chunk::H; ++y)
            c.tiles[y * Chunk::W + 10] = Tile::Stone;
        LightPropagator::addBlockSource(c, 5, 8, 10);
        assert(c.blockLight[8 * Chunk::W + 9] > 0);
        assert(c.blockLight[8 * Chunk::W + 11] == 0);
    }
    { // CombinedIsMax (sky 8 vs block 4 → 8)
        Chunk c;
        LightPropagator::computeSkyLight(c, nullptr, nullptr, nullptr, nullptr, 8);
        c.blockLight.assign(Chunk::W * Chunk::H, 0);
        LightPropagator::addBlockSource(c, 5, 5, 4);
        assert(c.lightAt(5, 5) == 8);
        assert(c.lightAt(6, 5) == 8);
    }
    { // NightSkyLevel (topo com nível 5: penumbra da lua)
        Chunk c;
        LightPropagator::computeSkyLight(c, nullptr, nullptr, nullptr, nullptr, 5);
        assert(c.skyLight[0] == 5);
    }

    { // EdgeSeedSkipsSolid (parede na borda não vira conduto)
        // Vizinho esquerdo todo claro (borda 15); parede total na col 0.
        // A parede (Stone, att 2) recebe a face 15-2=13 mas NÃO entra na
        // fila: sem o no-push, conduzia p/ o ar atrás da parede.
        Chunk left;
        LightPropagator::computeSkyLight(left, nullptr, nullptr, nullptr, nullptr, 15);
        Chunk c;
        for (int y = 0; y < Chunk::H; ++y)
            c.tiles[y * Chunk::W + 0] = Tile::Stone;
        LightPropagator::computeSkyLight(c, nullptr, &left, nullptr, nullptr, 15);
        assert(c.skyLight[6 * Chunk::W + 0] == 13); // face de pedra, sem conduzir
        assert(c.skyLight[6 * Chunk::W + 1] == 15); // ar ao lado: fill vertical
    }
    { // AttenuationByMaterial (pedra 2, terra 2, resto 3)
        using support::LightPropagator;
        assert(LightPropagator::attenuationFor(Tile::Stone) == 2);
        assert(LightPropagator::attenuationFor(Tile::Granite) == 4);
        assert(LightPropagator::attenuationFor(Tile::Bedrock) == 4);
        assert(LightPropagator::attenuationFor(Tile::Dirt) == 2);
        assert(LightPropagator::attenuationFor(Tile::Grass) == 2);
        assert(LightPropagator::attenuationFor(Tile::Sand) == 2);
        assert(LightPropagator::attenuationFor(Tile::OreIron) == 3);
        assert(LightPropagator::attenuationFor(Tile::TreeTrunk) == 3);
    }
    { // EdgeSeedTransmitsZero (parede do vizinho não vaza p/ ar coberto)
        // Vizinho com parede cheia na col 15 (só o topo recebe: 11, o
        // resto apaga em gradiente); chunk atual com teto total.
        // Sem o transmit-zero, a face 11 semeava 10 no ar atrás da parede.
        Chunk left;
        for (int y = 0; y < Chunk::H; ++y)
            left.tiles[y * Chunk::W + 15] = Tile::Stone;
        LightPropagator::computeSkyLight(left, nullptr, nullptr, nullptr, nullptr, 15);
        assert(left.skyLight[0 * Chunk::W + 15] == 13); // face superior acesa
        assert(left.skyLight[6 * Chunk::W + 15] == 13); // face lateral acesa (transmissor)
        Chunk c;
        for (int x = 0; x < Chunk::W; ++x)
            c.tiles[0 * Chunk::W + x] = Tile::Stone; // teto total
        LightPropagator::computeSkyLight(c, nullptr, &left, nullptr, nullptr, 15);
        assert(c.skyLight[0 * Chunk::W + 0] == 13); // face do próprio teto (pedra)
        assert(c.skyLight[6 * Chunk::W + 0] == 0);  // ar coberto: nada vaza
    }


    { // SeamSmoothAcrossBoundary (borda enxerga o vizinho, imagem 32×32)
        // Esquerda escura na col 15, direita clara na col 0.
        // Sem costura: pL.r == 0. Com: pL.r > 0; pR.r < 255.
        Chunk left, right;
        for (int y = 0; y < Chunk::H; ++y) {
            left.skyLight[y * Chunk::W + 15] = 0;
            right.skyLight[y * Chunk::W + 0] = 15;
        }
        sf::Image imgL = LightPropagator::buildLightImage(left, nullptr, &right,
                                                          nullptr, nullptr);
        sf::Image imgR = LightPropagator::buildLightImage(right, &left, nullptr,
                                                          nullptr, nullptr);
        assert(imgL.getSize().x == 16u * LightPropagator::kLightmapScale);
        const unsigned ex = imgL.getSize().x - 1;
        const unsigned mid = imgL.getSize().y / 2;
        const auto pL = imgL.getPixel(ex, mid);
        const auto pR = imgR.getPixel(0, mid);
        assert(pL.r > 0);   // vê luz do vizinho
        assert(pR.r < 255); // foi suavizado
    }

    { // RelightWipesDynamicSource (contrato: fonte dinâmica re-registra)
        // Explosão → breakTile → relightChunk zera o grid. Quem é
        // dinâmico (player) volta por fora (App re-adiciona se dirty);
        // estático futuro (tocha) vai em staticLights. Trava o wipe.
        Chunk c;
        LightPropagator::addBlockSource(c, 8, 8, 8, true);
        assert(c.blockLight[8 * Chunk::W + 8] == 8);
        LightPropagator::relightChunk(c, nullptr, nullptr, nullptr, nullptr, 15);
        for (int i = 0; i < Chunk::W * Chunk::H; ++i)
            assert(c.blockLight[i] == 0);
        // Re-registro restaura (o que o App faz no tick seguinte).
        LightPropagator::addBlockSource(c, 8, 8, 8, true);
        assert(c.blockLight[8 * Chunk::W + 8] == 8);
    }

    std::printf("light test OK\n");
    return 0;
}
