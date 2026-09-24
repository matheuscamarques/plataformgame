/**
 * @file tests/test_equipment.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava larguras de botas e luvas nos sprites.
 * @details Cobre EquipSprites com widths, roda com make test que compila em build/tests/test_equipment.
 */

#include <cassert>
#include <cstdio>
#include <cstring>

#include "assets/Sprites/EquipSprites.h"
#include "core/Equipment.h"
#include "core/Item.h"
#include "core/ItemDef.h"
#include "support/Combat/Body.h"

using namespace support;

int main() {
    { // BootsAndGlovesWidths
        for (int y = 0; y < sprites::kBootsH; ++y) {
            assert(std::strlen(sprites::kIronBootsIdle[y]) ==
                   static_cast<std::size_t>(sprites::kBootsW));
        }
        for (int y = 0; y < sprites::kGloveH; ++y) {
            assert(std::strlen(sprites::kIronGlovesIdle[y]) ==
                   static_cast<std::size_t>(sprites::kGloveW));
        }
    }
    { // AnchorFollowsRebuild (peça anda com a parte: mesmo delta)
        BodySchema s = BodySchema::humanoid(50.f, 50.f);
        Body b;
        b.attach(&s);
        b.rebuild({100.f, 100.f}, 1);
        const PartState *h1 = b.find(BodyPartId::Head);
        assert(h1 != nullptr);
        const float x1 = h1->worldBox.left;
        b.rebuild({200.f, 100.f}, 1);
        const PartState *h2 = b.find(BodyPartId::Head);
        assert(h2 != nullptr);
        assert(h2->worldBox.left - x1 > 99.f &&
               h2->worldBox.left - x1 < 101.f);
    }

    { // GlovesSlot (iron_gloves equipa no slot próprio, pesa, desequipa)
        assert(core::ItemRegistry::instance().has("iron_gloves"));
        const core::ItemDef* d =
            core::ItemRegistry::instance().find("iron_gloves");
        assert(d != nullptr && d->equipSlot == core::EquipSlot::Gloves);
        assert(d->defense == 2);
        assert(std::string(core::equipSlotName(core::EquipSlot::Gloves)) ==
               "Gloves");
        assert(core::equipDisplaySlot(6) == core::EquipSlot::Gloves);
        assert(core::equipDisplaySlot(7) == core::EquipSlot::None);
        core::Equipment eq;
        assert(eq.isEmpty());
        assert(eq.equip(core::Item{"iron_gloves", 1}));
        assert(eq.isOccupied(core::EquipSlot::Gloves));
        assert(eq.weight() == d->weight);
        // Slot errado recusa: luva na cabeça não entra.
        assert(!eq.equipTo(core::EquipSlot::Head,
                           core::Item{"iron_gloves", 1}));
        const core::Item back = eq.unequip(core::EquipSlot::Gloves);
        assert(back.defId == "iron_gloves" && back.quantity == 1);
        assert(eq.isEmpty());
    }

    std::printf("equipment test OK\n");
    return 0;
}
