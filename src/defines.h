/**
 * @file src/defines.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Shim legado que reexporta configuração e tipos de entidade.
 * @details Inclui Config.h e EntityKind.h para compatibilidade, mantido para os includes antigos sem nada novo.
 */

#pragma once
// Shim: defines.h morreu. Use core/Config.h (kBlockSize, kWorldSeed) e
// core/EntityKind.h (EntityKind + kId*). Mantido para não quebrar os
// ~25 includes existentes; não adicionar nada aqui.
#include "core/Config.h"
#include "core/EntityKind.h"
