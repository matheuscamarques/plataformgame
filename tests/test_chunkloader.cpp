#include <cassert>
#include <chrono>
#include <cstdio>
#include <thread>

#include "world/ChunkLoader.h"
#include "world/World.h"

// ChunkLoader async (camada 6, opt-in): worker gera nu, main adota.
// Resultado idêntico ao sync (geração é pura em seed+coords). Headless:
// threads + CPU, sem GL (textura só no render).
// NOTA: worker leva dezenas de ms p/ 25 chunks; main faz updates em
// microssegundos — o teste espera com poll+timeout (sem isso, take=null
// é correto, não bug: nada pronto ainda).
int main() {
    using namespace support;
    using namespace std::chrono_literals;

    { // AsyncCarregaAreaNova (pede longe, chega em updates seguintes)
        World w(1337u);
        ChunkLoader loader(
            [&](int cx, int cy) { return w.buildBareChunk(cx, cy); });
        assert(!loader.running());
        loader.start();
        assert(loader.running());
        w.setChunkLoader(&loader);
        assert(w.findChunk(31, 0) == nullptr); // ainda não existe
        bool arrived = false;
        for (int i = 0; i < 200 && !arrived; ++i) {
            w.update(500, 0); // chunk (31,0), raio 2
            arrived = w.findChunk(31, 0) != nullptr;
            if (!arrived) std::this_thread::sleep_for(25ms);
        }
        assert(arrived); // worker entregou via budget
        w.setChunkLoader(nullptr); // volta ao sync; loader morre no escopo
    }
    { // AsyncIgualSync (mesmos tiles que geração síncrona do mesmo seed)
        World async(1337u), sync(1337u);
        ChunkLoader loader(
            [&](int cx, int cy) { return async.buildBareChunk(cx, cy); });
        loader.start();
        async.setChunkLoader(&loader);
        for (int i = 0; i < 200; ++i) {
            async.update(500, 0);
            if (async.findChunk(31, 0) && async.findChunk(33, 2)) break;
            std::this_thread::sleep_for(25ms);
        }
        sync.update(500, 0); // tudo sync de uma vez
        for (auto [cx, cy] : {std::pair{31, 0}, {33, 2}, {29, -2}}) {
            Chunk* a = async.findChunk(cx, cy);
            Chunk* s = sync.findChunk(cx, cy);
            assert(a && s);
            assert(a->tiles == s->tiles); // byte-idêntico (puro em seed)
        }
    }

    std::printf("chunkloader test OK\n");
    return 0;
}
