// Visualizador de noise standalone (P1 da camada de cavernas).
// Uso: noise_view seed x0 x1 y0 y1 camada saida.png
//   camadas: relief | surface | tiles
// Escreve PNG em escala de cinza (tiles: tons por tipo).
// Não faz parte do jogo: só linka Generation + core (sem Entity/SFML).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Color.hpp>
#include "support/World/Generation.h"
#include "core/Noise.h"

namespace {

sf::Uint8 gray(float v) {
    if (v < 0.f) v = 0.f;
    if (v > 1.f) v = 1.f;
    return static_cast<sf::Uint8>(v * 255.f);
}

sf::Color tileColor(int t) {
    switch (t) {
        case 0:  return sf::Color(10, 10, 10);
        case 1:  return sf::Color(90, 90, 90);
        case 2:  return sf::Color(146, 90, 43);
        case 3:  return sf::Color(120, 60, 0);
        case 4:  return sf::Color(159, 89, 30);
        case 5:  return sf::Color(200, 120, 40);
        case 6:  return sf::Color(194, 178, 128); // areia
        case 8:  return sf::Color(235, 235, 245); // neve
        case 9:  return sf::Color(106, 190, 48);  // grama
        default: return sf::Color::Magenta;
    }
}

// Azul de oceano: tile vazio entre o nível do mar e a superfície.
bool isOceanWater(int tx, int ty, uint32_t seed) {
    return support::isOceanColumn(tx, seed) &&
           ty >= support::SEA_LEVEL &&
           ty < support::surfaceHeight(tx, seed);
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 8) {
        std::printf("uso: %s seed x0 x1 y0 y1 camada saida.png\n", argv[0]);
        std::printf("camadas: relief | mask | cave | overlay | temp | humid | biome_overlay | biomes | surface | tiles\n");
        return 2;
    }
    uint32_t seed = static_cast<uint32_t>(std::strtoul(argv[1], nullptr, 10));
    int x0 = std::atoi(argv[2]), x1 = std::atoi(argv[3]);
    int y0 = std::atoi(argv[4]), y1 = std::atoi(argv[5]);
    std::string layer = argv[6];
    std::string out = argv[7];
    if (x1 <= x0 || y1 <= y0) {
        std::printf("range inválido\n");
        return 2;
    }

    sf::Image img;
    img.create(static_cast<unsigned>(x1 - x0), static_cast<unsigned>(y1 - y0));

    float lo = 1.f, hi = 0.f, sum = 0.f;
    for (int ty = y0; ty < y1; ++ty) {
        for (int tx = x0; tx < x1; ++tx) {
            sf::Color px;
            if (layer == "relief") {
                float v = core::valueNoise2D(tx * 0.02f, 3.7f, seed);
                px = sf::Color(gray(v), gray(v), gray(v));
                if (v < lo) lo = v;
                if (v > hi) hi = v;
                sum += v;
            } else if (layer == "mask") {
                float v = support::mountainMask(tx, ty, seed);
                px = sf::Color(gray(v), gray(v), gray(v));
                if (v > support::MOUNTAIN_THRESHOLD) sum += 1.0f;
            } else if (layer == "cave") {
                float v = support::caveNoise(tx, ty, seed);
                px = sf::Color(gray(v), gray(v), gray(v));
                if (v < lo) lo = v;
                if (v > hi) hi = v;
                sum += v;
            } else if (layer == "overlay") {
                // R = mountainMask, G = caveNoise: onde bate, modulação atua.
                float m = support::mountainMask(tx, ty, seed);
                float c = support::caveNoise(tx, ty, seed);
                px = sf::Color(gray(m), gray(c), 0);
            } else if (layer == "temp" || layer == "humid") {
                float v = (layer == "temp")
                    ? support::temperature(tx, ty, seed)
                    : support::humidity(tx, ty, seed);
                px = sf::Color(gray(v), gray(v), gray(v));
                if (v < lo) lo = v;
                if (v > hi) hi = v;
                sum += v;
            } else if (layer == "biome_overlay") {
                // R = temperatura, G = umidade: variedade = biomas.
                float t = support::temperature(tx, ty, seed);
                float h = support::humidity(tx, ty, seed);
                px = sf::Color(gray(t), gray(h), 0);
            } else if (layer == "biomes") {
                // Clima cru (sem oceano): mostra a tabela funcionando.
                float t = support::temperature(tx, ty, seed);
                float h = support::humidity(tx, ty, seed);
                switch (support::pickBiome(t, h, false, false)) {
                    case support::Biome::Desert:    px = sf::Color(230, 200, 120); break;
                    case support::Biome::Savanna:   px = sf::Color(200, 140, 60); break;
                    case support::Biome::Grassland: px = sf::Color(90, 160, 70); break;
                    case support::Biome::Forest:    px = sf::Color(40, 110, 50); break;
                    case support::Biome::Taiga:     px = sf::Color(60, 120, 110); break;
                    case support::Biome::Tundra:    px = sf::Color(150, 150, 150); break;
                    default:                        px = sf::Color::Magenta; break;
                }
            } else if (layer == "surface") {
                // Banda clara = linha da superfície.
                int s = support::surfaceHeight(tx, seed);
                float d = 1.0f - (std::abs(ty - s) / 30.0f);
                if (d < 0.f) d = 0.f;
                px = sf::Color(gray(d), gray(d), gray(d));
            } else if (layer == "tiles") {
                int t = support::tileType(tx, ty, seed);
                if (t == 0 && isOceanWater(tx, ty, seed))
                    px = sf::Color(20, 80, 200);
                else
                    px = tileColor(t);
            } else {
                std::printf("camada desconhecida: %s\n", layer.c_str());
                return 2;
            }
            img.setPixel(static_cast<unsigned>(tx - x0), static_cast<unsigned>(ty - y0), px);
        }
    }

    if (!img.saveToFile(out)) {
        std::printf("falha ao salvar %s\n", out.c_str());
        return 1;
    }
    if (layer == "relief" || layer == "cave" || layer == "temp" || layer == "humid") {
        int n = (x1 - x0) * (y1 - y0);
        std::printf("salvo %s (lo=%.3f hi=%.3f media=%.3f)\n", out.c_str(), lo, hi, sum / n);
    } else if (layer == "mask") {
        int n = (x1 - x0) * (y1 - y0);
        std::printf("salvo %s (fração montanhosa=%.3f, alvo 0.30-0.40)\n", out.c_str(), sum / n);
    } else {
        std::printf("salvo %s\n", out.c_str());
    }
    return 0;
}
