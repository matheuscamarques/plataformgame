/**
 * @file tests/test_tarot_sprites.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless dos sprites das 78 cartas (38 únicos, 12×20).
 * @details Cobre spriteFor p/ os 78 arcanos (non-null, 12×20, charset na
 * paleta, toSpriteData sem crash), tiers 40/16/10/8/4 e registry com 78,
 * roda com make test que compila em build/tests/test_tarot_sprites.
 */

#include <cassert>
#include <cstdio>

#include "core/SpriteData.h"
#include "core/TarotCard.h"
#include "core/TarotSprites.h"

int main() {
    using core::TarotArcana;
    using core::tarot_sprites::spriteFor;
    constexpr int kCount = static_cast<int>(TarotArcana::COUNT);

    { // AllMapped (78 → sprite 12×20, charset na paleta, dado válido)
        for (int i = 0; i < kCount; ++i) {
            const auto a = static_cast<TarotArcana>(i);
            const auto ref = spriteFor(a);
            assert(ref.rows != nullptr);
            assert(ref.w == 12 && ref.h == 20);
            assert(ref.pal != nullptr && ref.palCount > 0);
            for (int y = 0; y < ref.h; ++y) {
                for (int x = 0; x < ref.w; ++x) {
                    const char ch = ref.rows[y][x];
                    bool found = false;
                    for (std::size_t p = 0; p < ref.palCount; ++p) {
                        if (ref.pal[p].ch == ch) {
                            found = true;
                            break;
                        }
                    }
                    assert(found); // char fora da paleta
                }
            }
            // Moldura: borda superior/inferior fechada em K.
            for (int x = 0; x < ref.w; ++x) {
                assert(ref.rows[0][x] == 'K');
                assert(ref.rows[ref.h - 1][x] == 'K');
            }
            // toSpriteData não crasha e tem o tamanho certo.
            const core::SpriteData d = core::toSpriteData(
                ref.rows, ref.w, ref.h, ref.pal, ref.palCount);
            assert(d.width == 12 && d.height == 20);
            assert(d.indices.size() == 12u * 20u);
        }
    }
    { // RanksShareSprites (56 Menores em 16 sprites: Ás/Núm/Pajem/Corte)
        const auto ace =
            spriteFor(TarotArcana::WandsAce).rows;
        const auto num =
            spriteFor(TarotArcana::WandsFive).rows;
        const auto page =
            spriteFor(TarotArcana::WandsPage).rows;
        const auto court =
            spriteFor(TarotArcana::WandsKing).rows;
        assert(ace != num && num != page && page != court);
        // Mesmo rank, cartas diferentes: mesmo sprite.
        assert(spriteFor(TarotArcana::WandsTwo).rows == num);
        assert(spriteFor(TarotArcana::WandsTen).rows == num);
        assert(spriteFor(TarotArcana::WandsKnight).rows == court);
        assert(spriteFor(TarotArcana::WandsQueen).rows == court);
        // Naipes diferentes: sprites diferentes.
        assert(spriteFor(TarotArcana::CupsAce).rows != ace);
        assert(spriteFor(TarotArcana::SwordsAce).rows != ace);
        assert(spriteFor(TarotArcana::PentaclesAce).rows != ace);
        // Maiores: cada uma tem a sua (22 únicas).
        for (int i = 0;
             i <= static_cast<int>(TarotArcana::World); ++i) {
            for (int j = i + 1;
                 j <= static_cast<int>(TarotArcana::World); ++j) {
                assert(spriteFor(static_cast<TarotArcana>(i)).rows !=
                       spriteFor(static_cast<TarotArcana>(j)).rows);
            }
        }
    }
    { // TierDistribution (40/16/10/8/4, Page Comum por decisão)
        int counts[5] = {0};
        for (int i = 0; i < kCount; ++i) {
            ++counts[static_cast<int>(
                core::tierOf(static_cast<TarotArcana>(i)))];
        }
        assert(counts[0] == 40); // Common
        assert(counts[1] == 16); // Uncommon (Ás + Knight/Queen/King)
        assert(counts[2] == 10); // Rare
        assert(counts[3] == 8);  // Epic
        assert(counts[4] == 4);  // Legendary
        assert(core::tierOf(TarotArcana::WandsPage) ==
               core::TarotTier::Common);
    }
    { // RegistryComplete (78 defs com flavor)
        assert(core::TarotRegistry::instance().keys().size() == 78u);
    }
    { // UniqueIndex (78 → 38: Maiores 1:1, Menores por rank)
        using core::tarot_sprites::uniqueIndexOf;
        using core::tarot_sprites::uniqueSprite;
        using core::tarot_sprites::kUniqueSpriteCount;
        for (int i = 0; i < kCount; ++i) {
            const auto u =
                uniqueIndexOf(static_cast<TarotArcana>(i));
            assert(u < kUniqueSpriteCount);
        }
        assert(uniqueIndexOf(TarotArcana::Fool) == 0);
        assert(uniqueIndexOf(TarotArcana::World) == 21);
        assert(uniqueIndexOf(TarotArcana::WandsAce) == 22);
        assert(uniqueIndexOf(TarotArcana::WandsTwo) == 23);
        assert(uniqueIndexOf(TarotArcana::WandsTen) == 23);
        assert(uniqueIndexOf(TarotArcana::WandsPage) == 24);
        assert(uniqueIndexOf(TarotArcana::WandsKnight) == 25);
        assert(uniqueIndexOf(TarotArcana::WandsQueen) == 25);
        assert(uniqueIndexOf(TarotArcana::WandsKing) == 25);
        assert(uniqueIndexOf(TarotArcana::PentaclesKing) == 37);
        // spriteFor concorda com uniqueSprite via índice.
        for (int i = 0; i < kCount; ++i) {
            const auto a = static_cast<TarotArcana>(i);
            assert(spriteFor(a).rows ==
                   uniqueSprite(uniqueIndexOf(a)).rows);
        }
        // uniqueSprite cobre os 38 e rejeita fora da faixa.
        for (int u = 0; u < kUniqueSpriteCount; ++u) {
            const auto ref = uniqueSprite(u);
            assert(ref.rows != nullptr);
            assert(ref.w == 12 && ref.h == 20);
        }
        assert(uniqueSprite(-1).rows == nullptr);
        assert(uniqueSprite(38).rows == nullptr);
    }

    std::printf("tarot sprites test OK\n");
    return 0;
}
