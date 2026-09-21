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
