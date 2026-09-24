/**
 * @file src/assets/SpriteComposer.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Compõe sprite a partir de partes (Fase B da migração).
 * @details Função pura: mescla Part[] num buffer, sem SFML. Teste compara
 * o composto com o frame monolítico (Fase E remove os monolíticos).
 */

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace assets {

// Pedaço de sprite ASCII: rows é o array JÁ fatiado (h linhas),
// colado em (offX, offY) do buffer. Sem srcY0: ele duplicava o offset
// (rows fatiado + srcY0 do monolítico) e lia fora do array.
struct Part {
    const char* const* rows;
    int w, h;
    int offX, offY;
};

// Mescla parts num buffer totalW×totalH ('.' onde nada cobre).
inline std::vector<std::string> compose(const Part* parts, std::size_t count,
                                        int totalW, int totalH) {
    std::vector<std::string> out(totalH, std::string(totalW, '.'));
    for (std::size_t i = 0; i < count; ++i) {
        const Part& p = parts[i];
        for (int y = 0; y < p.h; ++y) {
            for (int x = 0; x < p.w; ++x) {
                const int dx = p.offX + x;
                const int dy = p.offY + y;
                if (dx < 0 || dx >= totalW || dy < 0 || dy >= totalH)
                    continue; // fora: recorta, nunca crasha
                out[dy][dx] = p.rows[y][x];
            }
        }
    }
    return out;
}

} // namespace assets
