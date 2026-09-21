#include <cassert>
#include <cstdio>

#include "assets/Sprites/PlayerSprites.h"
#include "assets/Sprites/EnemySprites.h"
#include "assets/Sprites/EquipSprites.h"
#include "assets/Sprites/SpriteSet.h"

// Trava o split de Sprites.h: cada família resolve sozinha e o set
// agregado continua íntegro (counts que os testes legados cobrem).
// build() NÃO é chamado (textura exige GL, sem teste headless).
int main() {
    // Player 12x20 (8 frames), slime 14x12 (2), dwarf 14x18 (5).
    assert(sprites::kPlayerW == 12 && sprites::kPlayerH == 20);
    assert(sprites::kSlimeW == 14 && sprites::kSlimeH == 12);
    assert(sprites::kDwarfW == 14 && sprites::kDwarfH == 18);
    // Equip: espada 8x20 (idle/windup), 16x8 (swing).
    assert(sprites::kSwordW == 8 && sprites::kSwordH == 20);
    assert(sprites::kSwordSwingW == 16 && sprites::kSwordSwingH == 8);
    // Paletas por família.
    assert(sprites::kPlayerPalCount == 12);
    assert(sprites::kSlimePalCount == 5);
    assert(sprites::kDwarfPalCount == 11);
    // Set agregado: 1 textura por (peça × material).
    assert(sprites::SpriteSet::kMats == static_cast<int>(core::MaterialId::COUNT));
    std::puts("test_sprites_split OK");
    return 0;
}
