/**
 * @file src/core/Config.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Configuração central do mundo com tamanho de bloco e seed padrão.
 * @details Centraliza kBlockSize e kWorldSeed para uso em todo o jogo, incluído por defines.h e sistemas de mundo e física.
 */

#pragma once

#include <cstdint>

namespace core {

// Configuração central do mundo (era defines.h + constexpr locais).
// Um lugar só: mudar aqui muda em todo lugar, sem caçar número mágico.

// Bloco: unidade do mundo em px (era BLOCK_SIZE). Tile 1x1 = 50x50px;
// AABB do player 30x50, célula do hash = 2 tiles.
inline constexpr int kBlockSize = 50;

// Seed padrão do mundo (era WORLD_SEED local em game.cpp).
inline constexpr uint32_t kWorldSeed = 1337u;

} // namespace core
