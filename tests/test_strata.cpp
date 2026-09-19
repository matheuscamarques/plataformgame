#include <cassert>
#include <cstdio>
#include "entities/player/player.h"
#include "support/GameContext.h"
#include "support/StratumManager.h"
#include "support/World/Generation.h"
#include "support/World/Stratum.h"
#include "support/World/World.h"

// Estratos: cobertura contígua até 12000, checkpoints desvendam e
// respawn volta ao mais fundo. Geração funda funciona.
int main() {
    using namespace support;

    { // BoundariesAreContiguous (11 tetos, 200..12000, estritos)
        assert(STRATUM_COUNT == 11);
        assert(STRATUM_TOP[0] == 200 && STRATUM_TOP[10] == 12000);
        for (int i = 1; i < STRATUM_COUNT; ++i)
            assert(STRATUM_TOP[i] > STRATUM_TOP[i - 1]);
    }
    { // StratumAtMapping (fronteiras exatas, clamp nos extremos)
        assert(stratumAt(-500) == 0 && stratumAt(0) == 0);
        assert(stratumAt(199) == 0 && stratumAt(200) == 1);
        assert(stratumAt(1399) == 1 && stratumAt(1400) == 2);
        assert(stratumAt(6199) == 5 && stratumAt(6200) == 6);
        assert(stratumAt(10999) == 9 && stratumAt(11000) == 10);
        assert(stratumAt(11999) == 10 && stratumAt(50000) == 10);
        for (int s = 0; s < STRATUM_COUNT; ++s) assert(stratumName(s)[0] != '?');
        assert(checkpointTy(0) == 0 && checkpointTy(1) == 200);
        assert(checkpointTy(5) == 5000 && checkpointTy(6) == 6200);
        assert(checkpointTy(10) == 11000);
    }
    { // CheckpointUnlocksOnEntry + RespawnAtDeepestDiscovered
        Player p;
        StratumManager sm;
        GameContext ctx{};
        ctx.player = &p;
        assert(sm.deepest() == 0 && sm.unlocked(0) && !sm.unlocked(1));

        p.setY(300.f * 50.f); // ty=300: estrato 1
        sm.tick(1.f / 30.f, ctx);
        assert(sm.deepest() == 1 && sm.unlocked(1) && !sm.unlocked(2));

        p.setY(5000.f * 50.f); // ty=5000: estrato 5 (pula 2..4: desbloqueia)
        sm.tick(1.f / 30.f, ctx);
        assert(sm.deepest() == 5);
        for (int i = 0; i <= 5; ++i) assert(sm.unlocked(i));
        assert(!sm.unlocked(6));

        sf::Vector2f r = sm.respawnPoint(123.f);
        assert(r.x == 123.f && r.y == 5000.f * 50.f); // entrada do 5
    }
    { // DeepestStratumGenerates (determinístico, sem bedrock antes)
        for (int tx : {-500, 0, 500}) {
            Tile t = tileType(tx, 11999, 1337u);
            assert(t == tileType(tx, 11999, 1337u));
            assert(t != Tile::Bedrock);
            assert(tileType(tx, WORLD_BOTTOM, 1337u) == Tile::Bedrock);
        }
        // Bedrock não quebra (fundo é piso final).
        World world(1337u);
        world.update(0, WORLD_BOTTOM / 16);
        assert(!world.breakTile(0, WORLD_BOTTOM));
    }
    { // LavaExistsDeep (banda 6200+ tem lava em caverna)
        int found = 0;
        for (int tx = -100; tx < 100 && !found; tx++) {
            for (int ty = 6200; ty < 6300; ty++) {
                if (tileType(tx, ty, 1337u) == Tile::Lava) { found++; break; }
            }
        }
        assert(found > 0);
    }

    { // BgContrastBetweenNeighbors (vizinhos distinguíveis a olho)
        for (int s = 0; s + 1 < STRATUM_COUNT; ++s) {
            StratumBg a = stratumBg(s), b = stratumBg(s + 1);
            const int d = abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b);
            assert(d >= 25);
        }
        StratumBg sky = stratumBg(0);
        assert(sky.r == 135 && sky.g == 206 && sky.b == 235); // céu intacto
    }

    std::printf("strata test OK\n");
    return 0;
}
