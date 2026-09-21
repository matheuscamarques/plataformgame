#include "BodyDump.h"

#include <iomanip>
#include <ostream>

#include "support/Combat/Body.h"

namespace support {

void dumpBody(std::ostream &os,
              const Body &body,
              const char *const *rows,
              int spriteW, int spriteH,
              const core::PaletteEntry *pal, std::size_t palCount) {
    os << "Body dump:\n";
    if (!body.schema) {
        os << "  (no schema)\n";
        return;
    }
    for (const auto &part : body.parts) {
        int px = 0;
        if (rows) {
            for (int y = 0; y < spriteH; ++y) {
                for (int x = 0; x < spriteW; ++x) {
                    core::BodyPartId cp = core::BodyPartId::None;
                    for (std::size_t k = 0; k < palCount; ++k) {
                        if (pal[k].ch == rows[y][x]) {
                            cp = pal[k].part;
                            break;
                        }
                    }
                    if (cp == part.id) ++px;
                }
            }
        }
        const bool fallback = (px == 0 && rows != nullptr);
        os << "  " << std::setw(8) << static_cast<int>(part.id)
           << " bbox=(" << part.worldBox.left << ","
                        << part.worldBox.top << ","
                        << part.worldBox.width << ","
                        << part.worldBox.height << ")"
           << " px=" << px
           << " fallback=" << (fallback ? "SIM" : "NAO") << "\n";
    }
}

} // namespace support
