/**
 * @file src/world/RaycastLight.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Raycast de luz com oclusão para sombra e visibilidade.
 * @details Declara ChunkProvider, constantes de raios e castRays local e cross-chunk que escreve visibleMask, incluída por LightPropagator e Chunk.
 */

#pragma once
#include <cmath>
#include <functional>

#include <SFML/System/Vector2.hpp>

namespace support {

struct Chunk;

// Provedor de chunks carregados (nullptr se ausente). Evita incluir
// World.h aqui: o cast só precisa achar chunks por coordenada.
using ChunkProvider = std::function<Chunk*(int cx, int cy)>;

// Raycast de luz com oclusão: marca Chunk::visibleMask com os tiles que
// cada raio alcança (parede bloqueia o resto). DDA com passo 0.5 tile.
// Sem vazamento cross-chunk (raio morre na borda; vizinhos fazem o
// próprio cast). Regra de oclusão == BFS (só Air/Água passam).
class RaycastLight {
public:
    static constexpr int   kRaysPerLight = 96;    // 3.75° entre raios
    static constexpr float kStep         = 0.5f;  // avanço por iteração (tiles)
    static constexpr int   kMaxRayDist   = 16;    // alcance em tiles

    // Cast a partir de (ox, oy) em tile coords locais.
    // Sobrescreve visibleMask. Origem sempre visível (mesmo em sólido).
    static void castRays(Chunk& c, float ox, float oy);

    // Cast cross-chunk em coords de mundo (px): uma passada, raios
    // atravessam fronteiras escrevendo a máscara de cada chunk tocado.
    // Mesmas regras do cast local (parede visível-mas-corta, origem
    // sempre visível). maxDist = level + 2 tiles; chunk ausente
    // encerra o raio (não atravessa o vazio descarregado).
    static void castRaysCrossChunk(ChunkProvider findChunk,
                                   sf::Vector2f worldOriginPx, int level);

    // Contagem de tiles visíveis (debug/teste).
    static int visibleCount(const Chunk& c);
};

} // namespace support
