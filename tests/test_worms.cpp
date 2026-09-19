#include <cassert>
#include <cstdio>
#include <initializer_list>
#include <vector>
#include "support/World/Generation.h"
#include "support/World/Block.h"

// Worms: túneis conectados (iso-linha), bocas na superfície, independentes.
int main() {
    using namespace support;

    for (uint32_t seed : {1337u, 999u, 42u}) {
        // 1) Conectividade: maior componente >= 50% numa grade 128x128.
        {
            const int W = 128, H = 128;
            std::vector<uint8_t> grid(W * H, 0);
            for (int y = 0; y < H; y++)
                for (int x = 0; x < W; x++)
                    grid[y * W + x] = wormCave(x, y + 100, seed) ? 1 : 0;
            std::vector<int> comp(W * H, -1);
            int largest = 0, cid = 0, total = 0;
            for (int i = 0; i < W * H; i++) {
                if (grid[i] && comp[i] < 0) {
                    std::vector<int> stack{i};
                    comp[i] = cid;
                    int size = 0;
                    while (!stack.empty()) {
                        int p = stack.back(); stack.pop_back(); ++size;
                        int px = p % W, py = p / W;
                        const int dx[] = {1,-1,0,0}, dy[] = {0,0,1,-1};
                        for (int d = 0; d < 4; d++) {
                            int nx = px + dx[d], ny = py + dy[d];
                            if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;
                            int ni = ny * W + nx;
                            if (grid[ni] && comp[ni] < 0) { comp[ni] = cid; stack.push_back(ni); }
                        }
                    }
                    if (size > largest) largest = size;
                    ++cid;
                }
            }
            for (auto v : grid) total += v;
            assert(total > 0);
            float frac = (float)largest / total;
            std::printf("seed=%u worm total=%d maior=%.2f\n", seed, total, frac);
            assert(frac > 0.50f);
        }

        // 2) Bocas: >=1% das colunas tem abertura (ty em [s, s+2]).
        {
            int mouths = 0, n = 0;
            for (int tx = -2000; tx < 2000; tx++) {
                int s = surfaceHeight(tx, seed);
                n++;
                if (wormMouth(tx, s, seed) || wormMouth(tx, s + 1, seed) ||
                    wormMouth(tx, s + 2, seed)) mouths++;
            }
            float f = (float)mouths / n;
            std::printf("seed=%u bocas=%.4f\n", seed, f);
            assert(f > 0.01f && f < 0.25f);
        }

        // 3) Longe do centro: guard protege, faixa vira sólido.
        {
            int checked = 0;
            for (int tx = -2000; tx < 2000 && checked < 200; tx++) {
                int s = surfaceHeight(tx, seed);
                bool near = wormMouth(tx, s, seed) || wormMouth(tx, s + 1, seed) ||
                            wormMouth(tx, s + 2, seed);
                if (near) continue;
                for (int ty = s; ty <= s + 2; ty++) {
                    assert(isSolid(tileType(tx, ty, seed)));
                }
                checked++;
            }
            assert(checked == 200);
        }

        // 4) Determinismo.
        assert(wormCave(100, 200, seed) == wormCave(100, 200, seed));
        assert(wormMouth(100, 200, seed) == wormMouth(100, 200, seed));
    }

    // 5) Seeds distintas, mundos distintos (independência do blob).
    {
        int diff = 0, n = 0;
        for (int y = 0; y < 100; y++)
            for (int x = 0; x < 100; x++) {
                bool a = wormCave(x, y + 50, 1111u);
                bool b = wormCave(x, y + 50, 2222u);
                if (a != b) diff++;
                n++;
            }
        assert((float)diff / n > 0.10f);
    }

    std::printf("worms test OK\n");
    return 0;
}
