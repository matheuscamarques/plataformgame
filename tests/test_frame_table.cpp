/**
 * @file tests/test_frame_table.cpp
 * @brief Trava a tabela estática do registry (item 2 da faxina).
 * @details Todo id não-player aparece exatamente 1x e frameData()
 * devolve os mesmos campos. Roda com make test.
 */
#include <cassert>
#include <cstdio>
#include "assets/SpriteFrameRegistry.h"

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

    std::printf("frame table test OK\n");
    return 0;
}
