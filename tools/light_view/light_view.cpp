// light_view: golden images de luz (CPU puro, sem GL na lógica).
// Gera PNGs + dump de grid em build/light/ p/ inspeção humana.
// Uso: make light-view && ./build/tools/light_view (a partir da raiz).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>

#include "core/DayNightCycle.h"
#include "core/RadialTexture.h"
#include "world/Chunk.h"
#include "world/LightPropagator.h"
#include "world/RaycastLight.h"
#include "world/Tile.h"

using namespace support;

namespace {
void dumpGrid(const Chunk& c, const std::vector<uint8_t>& grid,
              const char* path) {
    std::ofstream txt(path);
    for (int y = 0; y < Chunk::H; ++y) {
        for (int x = 0; x < Chunk::W; ++x)
            txt << std::setw(3) << int(grid[y * Chunk::W + x]);
        txt << "\n";
    }
}
} // namespace

int main() {
    namespace fs = std::filesystem;
    fs::create_directories("build/light");

    // 1. Superfície aberta (tudo ar, sol 15).
    {
        Chunk surface;
        LightPropagator::computeSkyLight(surface, nullptr, nullptr,
                                         nullptr, nullptr, 15);
        LightPropagator::buildLightImage(surface, nullptr, nullptr,
                                         nullptr, nullptr)
            .saveToFile("build/light/scene_surface.png");
        dumpGrid(surface, surface.skyLight, "build/light/scene_surface.txt");
    }

    // 2. Caverna: teto em y=5 com buraco em x=8 + fonte com raycast.
    {
        Chunk cave;
        for (int x = 0; x < Chunk::W; ++x)
            cave.tiles[5 * Chunk::W + x] = Tile::Stone;
        cave.tiles[5 * Chunk::W + 8] = Tile::Air;
        LightPropagator::computeSkyLight(cave, nullptr, nullptr,
                                         nullptr, nullptr, 15);
        LightPropagator::addBlockSource(cave, 12, 10, 8, true);
        LightPropagator::buildLightImage(cave, nullptr, nullptr,
                                         nullptr, nullptr)
            .saveToFile("build/light/scene_cave.png");
        dumpGrid(cave, cave.blockLight, "build/light/cave_block.txt");
        dumpGrid(cave, cave.skyLight, "build/light/cave_sky.txt");
    }

    // 3. Glow radial do player/TNT.
    core::makeRadialImage(128, 3.0f).saveToFile("build/light/glow.png");

    // 4. Tint do ciclo meio-dia vs meia-noite (brilho médio).
    {
        const auto noon = core::DayNightCycle::sampleAt(12.f);
        const auto night = core::DayNightCycle::sampleAt(0.f);
        std::printf("noon tint=(%d,%d,%d) sun=%.2f\n", noon.tintR,
                    noon.tintG, noon.tintB, noon.sunIntensity);
        std::printf("night tint=(%d,%d,%d) moon=%.2f\n", night.tintR,
                    night.tintG, night.tintB, night.moonIntensity);
    }

    std::puts("light_view OK: build/light/{scene_surface,scene_cave,glow}.png");
    return 0;
}
