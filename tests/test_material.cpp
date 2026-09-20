#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "assets/Sprites.h"
#include "core/Material.h"

// Material: 4 cores distintas; ASCII de equipamento com widths exatas.
// build() NÃO é chamado (textura exige GL, sem teste headless).
int main() {
    using namespace sprites;
    auto colorDist = [](sf::Color a, sf::Color b) {
        return std::abs((int)a.r - (int)b.r) + std::abs((int)a.g - (int)b.g) +
               std::abs((int)a.b - (int)b.b);
    };

    { // AllColorsDistinct (Δ>40 entre materiais: legível no jogo)
        for (int i = 0; i < static_cast<int>(core::MaterialId::COUNT); ++i) {
            for (int j = i + 1; j < static_cast<int>(core::MaterialId::COUNT); ++j) {
                const auto &a = core::materialColors(static_cast<core::MaterialId>(i));
                const auto &b = core::materialColors(static_cast<core::MaterialId>(j));
                assert(colorDist(a.main, b.main) > 40);
            }
        }
    }
    { // AccentDiffersFromMain (detalhe visível: Δ>30)
        for (int i = 0; i < static_cast<int>(core::MaterialId::COUNT); ++i) {
            const auto &c = core::materialColors(static_cast<core::MaterialId>(i));
            assert(colorDist(c.accent, c.main) > 30);
        }
    }
    { // MaterialNamesNonEmpty
        for (int i = 0; i < static_cast<int>(core::MaterialId::COUNT); ++i) {
            assert(std::strlen(core::materialName(static_cast<core::MaterialId>(i))) > 0);
        }
    }
    { // EquipWidths (sword 8x20, swing 16x8, helm 12x5, chest 12x8, legs 12x6)
        for (int y = 0; y < kSwordH; ++y) {
            assert(std::strlen(kIronSwordIdle[y]) == static_cast<std::size_t>(kSwordW));
            assert(std::strlen(kIronSwordWindup[y]) == static_cast<std::size_t>(kSwordW));
        }
        for (int y = 0; y < kSwordSwingH; ++y) {
            assert(std::strlen(kIronSwordSwing[y]) == static_cast<std::size_t>(kSwordSwingW));
        }
        for (int y = 0; y < kHelmH; ++y) {
            assert(std::strlen(kIronHelmIdle[y]) == static_cast<std::size_t>(kHelmW));
        }
        for (int y = 0; y < kChestH; ++y) {
            assert(std::strlen(kIronChestIdle[y]) == static_cast<std::size_t>(kChestW));
        }
        for (int y = 0; y < kLegsH; ++y) {
            assert(std::strlen(kIronLegsIdle[y]) == static_cast<std::size_t>(kLegsW));
        }
    }
    { // EquipHasInk (nenhuma peça vazia)
        auto hasInk = [](const char *const *f, int h) {
            for (int y = 0; y < h; ++y) {
                for (const char *c = f[y]; *c; ++c) {
                    if (*c != '.') return true;
                }
            }
            return false;
        };
        assert(hasInk(kIronSwordIdle, kSwordH));
        assert(hasInk(kIronSwordSwing, kSwordSwingH));
        assert(hasInk(kIronHelmIdle, kHelmH));
        assert(hasInk(kIronChestIdle, kChestH));
        assert(hasInk(kIronLegsIdle, kLegsH));
    }

    std::printf("material test OK\n");
    return 0;
}
