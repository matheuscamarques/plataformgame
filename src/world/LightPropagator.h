/**
 * @file src/world/LightPropagator.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Propagação de luz por tile com costura entre chunks.
 * @details Declara sky e block light 0 a 15, relightChunk, buildLightImage e updateTexture com vizinhos, incluída por ChunkManager, World e Chunk.
 */

#pragma once
#include <SFML/Graphics/Image.hpp>
#include <cstdint>
#include <vector>

#include "world/RaycastLight.h"
#include "world/Tile.h"

namespace support {

struct Chunk;

// Propagação de luz por tile (0 = escuro, 15 = pleno).
// Sky desce por coluna do topo + flood lateral com -1 por passo.
// Luz nunca atravessa tile não-ar (água atenua -3 na vertical).
//
// Bordas cross-chunk: fill vertical usa a linha de baixo do vizinho
// de cima (ou skyLevel se não há vizinho); flood lateral semeia das
// bordas já computadas. Sem isso, chunk fundo brilharia e toda
// fronteira teria costura escura. Quem gera/muta chama relight dos
// vizinhos existentes (converge no streaming).
class LightPropagator {
public:
    // Atenuação que um sólido impõe à luz que o atinge (item 19):
    // pedra dura 4, terra fofa 2, resto 3. Ar/água não usam (regras
    // próprias). Pura e testável; aplicada nos 5 pontos de pintura.
    static uint8_t attenuationFor(Tile t);
    // Texels por tile no lightmap + kernel do blur (sempre ímpar).
    // Perfis testáveis (muda os dois, rebuild, compara PNGs em build/light/):
    //   A) scale=1, kernel=7   (16×16, spread ~3 tiles — bem suave)
    //   B) scale=2, kernel=5   (32×32, spread ~1 tile — baseline)
    //   C) scale=4, kernel=15  (64×64, spread ~1.75 tiles — meio)
    //   D) scale=2, kernel=7   (32×32, spread ~1.5 tiles — meio-termo)
    //   E) scale=4, kernel=7   (64×64 — TESTADO: costura piora (gap 36 vs
    //      <30 no D); kernel cobre menos tiles em mais resolução. Revertido.)
    // Display usa setSmooth(false): sem clamp bilinear, sem cruz;
    // a suavidade vem do grid (valores) + resolução (px por texel).
    static constexpr int kLightmapScale = 2;
    static constexpr int kBlurKernel = 7;
    // Sky do chunk. Vizinhos nullable (borda do mundo/streaming).
    // skyLevel = valor no topo do mundo (15 dia; noite usa tint, não relight).
    static void computeSkyLight(Chunk& c,
                                const Chunk* top,
                                const Chunk* left,
                                const Chunk* right,
                                const Chunk* bottom,
                                uint8_t skyLevel = 15);

    // Fonte de block light (tochas futuras + player). Re-flood do zero.
    // withRaycast: raycast define a máscara e o BFS só propaga no
    // visível (sombra dura); sem, flood livre (tocha sem oclusão).
    static void addBlockSource(Chunk& c, int lx, int ly, uint8_t level,
                               bool withRaycast = false);

    // Fonte dinâmica cross-chunk (player): cast único global + push
    // outward até fixpoint (teto 4, ordem cy,cx). Limpa a união da área
    // anterior com a nova (máscara + grid + dirty) — luz fantasma não
    // sobrevive ao movimento. Não toca a sobrecarga local (testes/tools).
    // Recebe provedor em vez de World (sem acoplamento com a fachada).
    static void addBlockSourceAt(ChunkProvider findChunk, float worldXpx,
                                 float worldYpx, uint8_t level);

    // Recomputa tudo do chunk (geração/mutação). Sem fontes estáticas
    // hoje: block zera (player é overlay, não grid).
    static void relightChunk(Chunk& c,
                             const Chunk* top,
                             const Chunk* left,
                             const Chunk* right,
                             const Chunk* bottom,
                             uint8_t skyLevel = 15);

    // Imagem S·W×S·H do lightmap (pura CPU, testável): combina (max),
    // lê vizinhos na borda (costura cross-chunk, diagonais inclusas) e
    // aplica blur K×K direto na resolução de saída (single-stage).
    // Diagonais: sem elas o blur mistura 0 nos 4 cantos e repete o
    // artefato em toda interseção de chunks (defaults = sem vizinho).
    static sf::Image buildLightImage(const Chunk& c,
                                     const Chunk* left,
                                     const Chunk* right,
                                     const Chunk* top,
                                     const Chunk* bottom,
                                     const Chunk* topLeft = nullptr,
                                     const Chunk* topRight = nullptr,
                                     const Chunk* bottomLeft = nullptr,
                                     const Chunk* bottomRight = nullptr);

    // Textura 2W×2H do lightmap (GL — só no render; geração/teste não chamam).
    // Vizinhos: costura da borda (sem eles, grade visível de 16 em 16).
    static void updateTexture(Chunk& c,
                              const Chunk* left,
                              const Chunk* right,
                              const Chunk* top,
                              const Chunk* bottom,
                              const Chunk* topLeft = nullptr,
                              const Chunk* topRight = nullptr,
                              const Chunk* bottomLeft = nullptr,
                              const Chunk* bottomRight = nullptr);

private:
    // grids vizinhos: leitura read-only p/ costura (nullptr = sem vizinho).
    // Passar o grid (não o Chunk) evita ler sky alheio num flood de block.
    // Tiles vizinhos: transmissão (sólido transmite 0 — valor da face dele
    // serve p/ render, não p/ atravessar). nullptr ⟺ grid nullptr.
    static void floodGeneric(std::vector<uint8_t>& grid,
                             const Chunk& c,
                             const std::vector<uint8_t>* left,
                             const std::vector<uint8_t>* right,
                             const std::vector<uint8_t>* bottom,
                             const std::vector<Tile>* leftTiles = nullptr,
                             const std::vector<Tile>* rightTiles = nullptr,
                             const std::vector<Tile>* bottomTiles = nullptr);
    // BFS restrito à máscara do raycast (sombra dura).
    static void floodMasked(std::vector<uint8_t>& grid,
                            const Chunk& c,
                            const std::vector<uint8_t>& mask);
};

} // namespace support
