#include <cassert>
#include <cstdio>

#include "core/Inventory.h"
#include "support/GameContext.h"
#include "support/Progression/DropSystem.h"
#include "entities/Player/Player.h"

// Orbes de item (fase 2): spawn, expiração, delay. Headless (sem GL;
// render não é chamado aqui).
int main() {
    using namespace support;

    { // SpawnRecusaDesconhecido (def fora do registry = nullptr)
        DropSystem drops;
        assert(drops.spawnItem("unobtainium", 3, {0.f, 0.f}) == nullptr);
        assert(drops.activeItemCount() == 0u);
        assert(drops.spawnItem("stone", 0, {0.f, 0.f}) == nullptr);
    }
    { // SpawnGuardaPayload (defId + qty sobrevivem ao tick sem player)
        DropSystem drops;
        GameContext ctx{};
        assert(drops.spawnItem("stone", 7, {100.f, 100.f}) != nullptr);
        assert(drops.activeItemCount() == 1u);
        for (int i = 0; i < 10; ++i) drops.tick(1.f / 30.f, ctx);
        assert(drops.activeItemCount() == 1u); // sem player: cai, não some
    }
    { // ExpiraPorLifetime (60s sem coleta some)
        DropSystem drops;
        GameContext ctx{};
        drops.spawnItem("wood", 2, {0.f, 0.f});
        drops.tick(59.f, ctx);
        assert(drops.activeItemCount() == 1u);
        drops.tick(2.f, ctx);
        assert(drops.activeItemCount() == 0u);
    }
    { // DelayBloqueiaColetaPrecoce (0.5s grudado não coleta)
        DropSystem drops;
        Player p; // ctor semeia kit; conta relativo à base
        const int stone0 = p.inventory.count("stone");
        GameContext ctx{};
        ctx.player = &p;
        drops.spawnItem("stone", 1, {p.getCenterX(), p.getCenterY()});
        drops.tick(0.1f, ctx);
        assert(drops.activeItemCount() == 1u); // delay segurou
        assert(p.inventory.count("stone") == stone0);
        for (int i = 0; i < 30; ++i) drops.tick(1.f / 30.f, ctx);
        assert(p.inventory.count("stone") == stone0 + 1); // coletou
        assert(drops.activeItemCount() == 0u);
    }
    { // MagnetAtrai (40px some em 2s, igual ao XP)
        DropSystem drops;
        Player p;
        const int wood0 = p.inventory.count("wood");
        GameContext ctx{};
        ctx.player = &p;
        drops.spawnItem("wood", 3,
                        {p.getCenterX() + 40.f, p.getCenterY()});
        for (int i = 0; i < 60; ++i) drops.tick(1.f / 30.f, ctx);
        assert(p.inventory.count("wood") == wood0 + 3);
        assert(drops.activeItemCount() == 0u);
    }

    std::printf("itemdrop test OK\n");
    return 0;
}
