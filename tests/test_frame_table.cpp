/**
 * @file tests/test_frame_table.cpp
 * @brief Trava a tabela estática do registry (item 2 da faxina).
 * @details Todo id não-player aparece exatamente 1x e frameData()
 * devolve os mesmos campos. Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include "assets/SpriteFrameRegistry.h"
#include "assets/Sprites/SpriteSet.h"

int main() {
    using support::SpriteFrameId;
    int n = 0;
    const assets::StaticFrameEntry* t = assets::staticFrameTable(&n);
    assert(t != nullptr && n > 0);

    const int first = static_cast<int>(SpriteFrameId::SlimeIdle);
    const int last = static_cast<int>(SpriteFrameId::PureElementalIdle);
    assert(n == last - first + 1); // sem falta, sem sobra
    for (int i = first; i <= last; ++i) {
        const auto id = static_cast<SpriteFrameId>(i);
        int hits = 0;
        const assets::StaticFrameEntry* e = nullptr;
        for (int j = 0; j < n; ++j) {
            if (t[j].id == id) {
                ++hits;
                e = &t[j];
            }
        }
        assert(hits == 1);
        const assets::SpriteFrameData f = assets::frameData(id);
        assert(f.rows == e->rows && f.w == e->w && f.h == e->h &&
               f.pal == e->pal && f.palCount == e->palCount);
        assert(f.rows != nullptr && f.w > 0 && f.h > 0);
    }
    assert(assets::frameData(SpriteFrameId::None).rows == nullptr);
    assert(assets::frameData(SpriteFrameId::COUNT).rows == nullptr);
    assert(assets::frameData(SpriteFrameId::PlayerIdle).rows != nullptr);

    { // TargetsCobremTabela (build() monta 1:1 com a tabela, sem falta)
        int m = 0;
        const sprites::EnemyTexTarget* tt = sprites::enemyTexTargets(&m);
        assert(m == n);
        for (int i = 0; i < m; ++i) {
            int hits = 0;
            for (int j = 0; j < n; ++j) {
                if (t[j].id == tt[i].id) ++hits;
            }
            assert(hits == 1);
        }
    }
    { // PackPalsGeradas (cores/chars byte-idênticos às tabelas antigas)
        const char* want = ".KFHERCTBWD";
        auto check = [&](const core::PaletteEntry* pal, std::size_t c,
                         sf::Color torso, sf::Color eye) {
            assert(c == 11);
            for (int i = 0; i < 11; ++i) assert(pal[i].ch == want[i]);
            assert(pal[7].color == torso); // 'T'
            assert(pal[4].color == eye);   // 'E'
            assert(pal[3].color == pal[2].color); // 'H' = pele ('F')
        };
        check(sprites::kHollowPal, sprites::kHollowPalCount,
              sf::Color(120, 70, 50), sf::Color(15, 15, 15));
        check(sprites::kBurstPal, sprites::kBurstPalCount,
              sf::Color(50, 120, 60), sf::Color(10, 10, 10));
        check(sprites::kImpPal, sprites::kImpPalCount,
              sf::Color(140, 50, 50), sf::Color(230, 200, 50));
        check(sprites::kElementalPal, sprites::kElementalPalCount,
              sf::Color(220, 100, 40), sf::Color(255, 255, 220));
        check(sprites::kUndeadPal, sprites::kUndeadPalCount,
              sf::Color(110, 110, 115), sf::Color(10, 10, 15));
    }

    std::printf("frame table test OK\n");
    return 0;
}
