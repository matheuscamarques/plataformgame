/**
 * @file src/support/Debug/BodyDump.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Declara função de despejo textual do estado do Body.
 * @details Declara dumpBody com stream, Body, matriz ASCII e paleta para diagnóstico, incluída pelo Renderer e testes sem depender de GL.
 */

#pragma once
#include <iosfwd>

#include "core/sprite_from_ascii.h"

namespace support {

struct Body;

// Imprime estado do Body: cada parte com bbox, contagem de pixels no
// sprite de origem, e flag fallback (SIM = usa schema estático).
// pixelCount só é preenchido se rows/pal forem fornecidos.
void dumpBody(std::ostream &os,
              const Body &body,
              const char *const *rows,
              int spriteW, int spriteH,
              const core::PaletteEntry *pal, std::size_t palCount);

} // namespace support
