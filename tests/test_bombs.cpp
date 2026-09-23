#include <cassert>
#include <cstdio>

#include "core/ItemDef.h"
#include "entities/Player/Player.h"
#include "support/Effects/ThrowSystem.h"
#include "support/Effects/Throwable.h"

// Escada de bombas: dinamite < TNT < C4 < MOAB (mundo real).
// J joga o slot ativo da hotbar; sem bomba, cai na dinamite (legado).
int main() {
    using namespace support;

    { // TiersOrdenados (dano e raio crescem; kinds certos)
        const core::ItemDef* dyn =
            core::ItemRegistry::instance().find("dynamite");
        const core::ItemDef* tnt = core::ItemRegistry::instance().find("tnt");
        const core::ItemDef* c4  = core::ItemRegistry::instance().find("c4");
        const core::ItemDef* daisy =
            core::ItemRegistry::instance().find("daisy");
        const core::ItemDef* moab =
            core::ItemRegistry::instance().find("moab");
        assert(dyn && dyn->throwable);
        assert(tnt && tnt->throwable && tnt->throwKind == ThrowKind::Tnt);
        assert(c4 && c4->throwable && c4->throwKind == ThrowKind::C4);
        assert(daisy && daisy->throwable &&
               daisy->throwKind == ThrowKind::Daisy);
        assert(moab && moab->throwable && moab->throwKind == ThrowKind::Moab);
        assert(dyn->blastDamage < tnt->blastDamage);
        assert(tnt->blastDamage < c4->blastDamage);
        assert(c4->blastDamage < daisy->blastDamage);
        assert(daisy->blastDamage < moab->blastDamage);
        assert(dyn->blastRadius < tnt->blastRadius);
        assert(tnt->blastRadius < c4->blastRadius);
        assert(c4->blastRadius < daisy->blastRadius);
        assert(daisy->blastRadius < moab->blastRadius);
        assert(isPlayerBomb(ThrowKind::Dynamite));
        assert(isPlayerBomb(ThrowKind::Tnt));
        assert(isPlayerBomb(ThrowKind::C4));
        assert(isPlayerBomb(ThrowKind::Daisy));
        assert(isPlayerBomb(ThrowKind::Moab));
        assert(!isPlayerBomb(ThrowKind::Spit));
        assert(!isPlayerBomb(ThrowKind::Barrel));
    }
    { // ThrowSlotUsaStatsDoDef (TNT do slot 0: fuse/raio/dano/tiles)
        Player p;
        ThrowSystem ts;
        p.inventory = core::Inventory{}; // zera o kit
        p.inventory.add(core::Item{"tnt", 3});
        assert(p.tryThrowSlot(ts, 0));
        assert(p.inventory.count("tnt") == 2); // 1 saiu do slot exato
        assert(ts.activeCount() == 1u);
        bool seen = false;
        ts.forEachActive([&](const Throwable& t) {
            seen = true;
            assert(t.kind == ThrowKind::Tnt);
            assert(t.fuse == 1.0f && t.radius == 70.f);
            assert(t.damage == 45 && t.tilesRadius == 5);
        });
        assert(seen);
    }
    { // ThrowSlotRejeitaNaoBomba (espada no slot: false, sem gasto)
        Player p;
        ThrowSystem ts;
        p.inventory = core::Inventory{};
        p.inventory.add(core::Item{"iron_sword", 1});
        assert(!p.tryThrowSlot(ts, 0));
        assert(p.inventory.count("iron_sword") == 1);
        assert(ts.activeCount() == 0u);
        assert(!p.tryThrowSlot(ts, -1)); // slot inválido
        assert(!p.tryThrowSlot(ts, 80));
    }
    { // ThrowSlotRespeitaCooldown (2º imediato falha sem gastar)
        Player p;
        ThrowSystem ts;
        p.inventory = core::Inventory{};
        p.inventory.add(core::Item{"c4", 5});
        assert(p.tryThrowSlot(ts, 0));
        assert(!p.tryThrowSlot(ts, 0));
        assert(p.inventory.count("c4") == 4);
    }
    { // MoabApagaChunk (raio cobre 16x16 tiles = 1 chunk)
        Player p;
        ThrowSystem ts;
        p.inventory = core::Inventory{};
        p.inventory.add(core::Item{"moab", 1});
        assert(p.tryThrowSlot(ts, 0));
        assert(p.inventory.count("moab") == 0); // última unidade limpa
        ts.forEachActive([&](const Throwable& t) {
            assert(t.kind == ThrowKind::Moab);
            assert(t.radius == 800.f && t.damage == 140);
            assert(t.tilesRadius == 16);
        });
    }

    std::printf("bombs test OK\n");
    return 0;
}
