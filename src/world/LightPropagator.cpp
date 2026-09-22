#include "world/LightPropagator.h"
#include "world/Chunk.h"
#include "world/RaycastLight.h"
#include "world/Tile.h"
#include "core/Config.h"

#include <cmath>
#include <queue>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>

namespace support {

uint8_t LightPropagator::attenuationFor(Tile t) {
    switch (t) {
        case Tile::Stone:
            return 2; // pedra comum: gradiente noturno legível (8 tiles)
        // Rocha dura: segura mais luz.
        case Tile::Granite:
        case Tile::Basalt:
        case Tile::Marble:
        case Tile::Obsidian:
        case Tile::Bedrock:
        case Tile::Cobblestone:
        case Tile::Schist:
        case Tile::PebbledStone:
        case Tile::CrackedStone:
        case Tile::CharredStone:
        case Tile::TwistedStone:
        case Tile::NullStone:
            return 4;
        // Terra fofa: deixa passar mais.
        case Tile::Dirt:
        case Tile::Grass:
        case Tile::Sand:
        case Tile::Mud:
        case Tile::Clay:
        case Tile::Gravel:
        case Tile::Snow:
        case Tile::TundraTop:
        case Tile::TaigaTop:
        case Tile::FallbackTop:
        case Tile::SavannaTop:
        case Tile::Sandstone:
        case Tile::Permafrost:
        case Tile::RootedDirt:
        case Tile::MossStone:
        case Tile::Mycelium:
        case Tile::CoalDust:
        case Tile::ScorchedEarth:
            return 2;
        default:
            return 3; // minérios, cristais, madeiras, resto
    }
}

namespace {

struct Cell { int x, y; };

// BFS 4-dir com -1 por passo. Semeia de todas as células > 1 (fill
// vertical / fonte) + bordas dos vizinhos já computados (costura
// cross-chunk sem escrita fora do próprio chunk).
void bfs(std::vector<uint8_t>& grid,
         const Chunk& c,
         const std::vector<uint8_t>* left,
         const std::vector<uint8_t>* right,
         const std::vector<uint8_t>* bottom,
         const std::vector<Tile>* leftTiles = nullptr,
         const std::vector<Tile>* rightTiles = nullptr,
         const std::vector<Tile>* bottomTiles = nullptr)
{
    std::queue<Cell> q;
    auto push = [&](int x, int y) {
        // Só ar/água entram na fila: sólido pintado (face iluminada)
        // absorve, nunca conduz (senão a parede vira conduto).
        const Tile t = c.tiles[y * Chunk::W + x];
        if (t != Tile::Air && t != Tile::Water) return;
        if (grid[y * Chunk::W + x] > 1) q.push({x, y});
    };
    for (int y = 0; y < Chunk::H; ++y)
        for (int x = 0; x < Chunk::W; ++x)
            push(x, y);

    // Sementes de borda: luz que entra do vizinho.
    // Transmissão, não face: sólido vizinho transmite 0 (o valor da face
    // dele serve p/ render, não p/ atravessar — senão vaza 12 p/ o ar
    // atrás de parede de 1 tile). Sólido local recebe -2 sem fila.
    auto transmit = [](uint8_t v, Tile t) -> uint8_t {
        return (t == Tile::Air || t == Tile::Water) ? v : 0;
    };
    auto seedEdge = [&](int x, int y, uint8_t v) {
        const int i = y * Chunk::W + x;
        const Tile t = c.tiles[i];
        if (t != Tile::Air && t != Tile::Water) {
            const uint8_t att = LightPropagator::attenuationFor(t);
            if (v > grid[i] + att)
                grid[i] = static_cast<uint8_t>(v - att);
            return;
        }
        if (v > grid[i] + 1) {
            grid[i] = static_cast<uint8_t>(v - 1);
            q.push({x, y});
        }
    };
    if (left)
        for (int y = 0; y < Chunk::H; ++y)
            seedEdge(0, y, transmit((*left)[y * Chunk::W + Chunk::W - 1],
                                    (*leftTiles)[y * Chunk::W + Chunk::W - 1]));
    if (right)
        for (int y = 0; y < Chunk::H; ++y)
            seedEdge(Chunk::W - 1, y, transmit((*right)[y * Chunk::W],
                                               (*rightTiles)[y * Chunk::W]));
    if (bottom)
        for (int x = 0; x < Chunk::W; ++x)
            seedEdge(x, Chunk::H - 1,
                     transmit((*bottom)[x], (*bottomTiles)[x]));

    constexpr int dx[] = {1, -1, 0, 0};
    constexpr int dy[] = {0, 0, 1, -1};

    while (!q.empty()) {
        const Cell cell = q.front();
        q.pop();
        const int i = cell.y * Chunk::W + cell.x;
        const uint8_t cur = grid[i];
        if (cur <= 1) continue;

        for (int d = 0; d < 4; ++d) {
            const int nx = cell.x + dx[d];
            const int ny = cell.y + dy[d];
            if (nx < 0 || nx >= Chunk::W || ny < 0 || ny >= Chunk::H) continue;
            const int ni = ny * Chunk::W + nx;
            // Sólido: pinta a face com -att (material), mas não empurra
            // p/ fila (absorve, não conduz). Ar/água: -1 com fila, como antes.
            if (c.tiles[ni] != Tile::Air && c.tiles[ni] != Tile::Water) {
                const uint8_t att = LightPropagator::attenuationFor(c.tiles[ni]);
                if (cur > grid[ni] + att)
                    grid[ni] = static_cast<uint8_t>(cur - att);
                continue;
            }
            const uint8_t next = static_cast<uint8_t>(cur - 1);
            if (next > grid[ni]) {
                grid[ni] = next;
                q.push({nx, ny});
            }
        }
    }
}

} // namespace

void LightPropagator::computeSkyLight(Chunk& c,
                                      const Chunk* top,
                                      const Chunk* left,
                                      const Chunk* right,
                                      const Chunk* bottom,
                                      uint8_t skyLevel)
{
    c.skyLight.assign(Chunk::W * Chunk::H, 0);

    // Passo 1: fill vertical por coluna. Nível inicial = borda de baixo
    // do vizinho de cima (luz real que desce); sem vizinho = skyLevel
    // (correto na superfície; chunk fundo sem vizinho relighta quando
    // o de cima gerar).
    for (int x = 0; x < Chunk::W; ++x) {
        uint8_t level = skyLevel;
        if (top) {
            // Transmissão, não face: sólido não deixa passar (o valor da
            // face dele serve p/ render, não p/ iluminar quem está embaixo).
            const int bi = (Chunk::H - 1) * Chunk::W + x;
            const Tile bt = top->tiles[bi];
            level = (bt == Tile::Air || bt == Tile::Water)
                        ? top->skyLight[bi]
                        : 0;
        }
        for (int y = 0; y < Chunk::H; ++y) {
            const int i = y * Chunk::W + x;
            const Tile t = c.tiles[i];
            if (t == Tile::Air) {
                c.skyLight[i] = level;
            } else if (t == Tile::Water) {
                // Água atenua e continua descendo.
                level = (level > 3) ? static_cast<uint8_t>(level - 3) : 0;
                c.skyLight[i] = level;
            } else {
                // Sólido: face com -att (material), sem propagar (bloqueia).
                const uint8_t att = LightPropagator::attenuationFor(t);
                c.skyLight[i] = (level > att) ? static_cast<uint8_t>(level - att) : 0;
                break;
            }
        }
    }

    // Passo 2: flood lateral (+ costura de borda).
    floodGeneric(c.skyLight, c,
                 left ? &left->skyLight : nullptr,
                 right ? &right->skyLight : nullptr,
                 bottom ? &bottom->skyLight : nullptr,
                 left ? &left->tiles : nullptr,
                 right ? &right->tiles : nullptr,
                 bottom ? &bottom->tiles : nullptr);
}

void LightPropagator::addBlockSource(Chunk& c, int lx, int ly, uint8_t level,
                                      bool withRaycast) {
    if (!c.inBounds(lx, ly)) return;
    const int i = ly * Chunk::W + lx;

    if (withRaycast) {
        // 1. Raycast define o visível; 2. zera (luz velha em tile agora
        // invisível vira lixo); 3. BFS só no visível. Assume 1 fonte
        // dinâmica por chunk (player); tochas estáticas re-aplicam junto
        // na fase 3.
        RaycastLight::castRays(c, lx + 0.5f, ly + 0.5f);
        c.blockLight.assign(Chunk::W * Chunk::H, 0);
        c.blockLight[i] = level;
        floodMasked(c.blockLight, c, c.visibleMask);
        c.lightDirty = true;
        return;
    }

    if (c.blockLight[i] >= level) return;
    c.blockLight[i] = level;
    floodGeneric(c.blockLight, c, nullptr, nullptr, nullptr);
    c.lightDirty = true;
}

namespace {

// floor-div p/ tiles possivelmente negativos (mundo pode ser < 0).
inline int chunkOfTile(int t, int n) {
    return static_cast<int>(std::floor(static_cast<float>(t) /
                                       static_cast<float>(n)));
}

// Semeia a borda de `dst` a partir dos grids vizinhos já computados.
// Mesma regra do seedEdge do bfs + respeito à máscara do próprio chunk
// (luz só entra onde o raio alcançou). Retorna true se algum valor subiu.
bool seedBorderFromNeighbors(std::vector<uint8_t>& grid, const Chunk& dst,
                             const std::vector<uint8_t>& mask,
                             ChunkProvider findChunk) {
    bool changed = false;
    // Transmissão, não face: sólido vizinho transmite 0.
    auto transmit = [](uint8_t v, Tile t) -> uint8_t {
        return (t == Tile::Air || t == Tile::Water) ? v : 0;
    };
    auto seed = [&](int x, int y, uint8_t v) {
        const int i = y * Chunk::W + x;
        const Tile t = dst.tiles[i];
        if (t != Tile::Air && t != Tile::Water) {
            // Sólido de borda: face iluminada (-att material), sem fila.
            if (mask[i] == 0) return; // sem raycast aqui: sombra, não costura
            const uint8_t att = LightPropagator::attenuationFor(t);
            if (v > grid[i] + att) {
                grid[i] = static_cast<uint8_t>(v - att);
                changed = true;
            }
            return;
        }
        if (mask[i] == 0) return;   // respeita visibilidade
        if (v > grid[i] + 1) {
            grid[i] = static_cast<uint8_t>(v - 1);
            changed = true;
        }
    };
    if (const Chunk* L = findChunk(dst.cx - 1, dst.cy))
        for (int y = 0; y < Chunk::H; ++y) {
            const int bi = y * Chunk::W + Chunk::W - 1;
            seed(0, y, transmit(L->blockLight[bi], L->tiles[bi]));
        }
    if (const Chunk* R = findChunk(dst.cx + 1, dst.cy))
        for (int y = 0; y < Chunk::H; ++y) {
            const int bi = y * Chunk::W;
            seed(Chunk::W - 1, y, transmit(R->blockLight[bi], R->tiles[bi]));
        }
    if (const Chunk* T = findChunk(dst.cx, dst.cy - 1))
        for (int x = 0; x < Chunk::W; ++x) {
            const int bi = (Chunk::H - 1) * Chunk::W + x;
            seed(x, 0, transmit(T->blockLight[bi], T->tiles[bi]));
        }
    if (const Chunk* B = findChunk(dst.cx, dst.cy + 1))
        for (int x = 0; x < Chunk::W; ++x)
            seed(x, Chunk::H - 1, transmit(B->blockLight[x], B->tiles[x]));
    return changed;
}

// Última área varrida: limpa a união(old,new) p/ máscara stale e grid
// fantasma não sobreviverem ao movimento. Estado file-local (header
// segue stateless); primeira chamada não tem anterior (valid=false).
struct LastSweep {
    bool valid = false;
    float x = 0.f, y = 0.f;
    int level = 0;
};
LastSweep g_lastSweep;

} // namespace

void LightPropagator::addBlockSourceAt(ChunkProvider findChunk, float wxPx,
                                       float wyPx, uint8_t level) {
    const float bs = static_cast<float>(core::kBlockSize);
    const auto boxOf = [&](float px, float py, int lv) {
        struct Box {
            int x0, x1, y0, y1;
        };
        const int R = (lv + 2) / Chunk::W + 1;
        const int tx = static_cast<int>(std::floor(px / bs));
        const int ty = static_cast<int>(std::floor(py / bs));
        const int cx = chunkOfTile(tx, Chunk::W);
        const int cy = chunkOfTile(ty, Chunk::H);
        return Box{cx - R, cx + R, cy - R, cy + R};
    };
    auto clearBox = [&](int x0, int x1, int y0, int y1) {
        for (int cy = y0; cy <= y1; ++cy)
            for (int cx = x0; cx <= x1; ++cx)
                if (Chunk* c = findChunk(cx, cy)) {
                    c->clearVisibility();
                    c->blockLight.assign(Chunk::W * Chunk::H, 0);
                    c->lightDirty = true;
                }
    };

    // 1. Limpa união(old,new): stale some aqui, não no playtest.
    if (g_lastSweep.valid) {
        const auto ob = boxOf(g_lastSweep.x, g_lastSweep.y, g_lastSweep.level);
        clearBox(ob.x0, ob.x1, ob.y0, ob.y1);
    }
    const auto nb = boxOf(wxPx, wyPx, level);
    clearBox(nb.x0, nb.x1, nb.y0, nb.y1);
    // Anel dirty ±1 (só dirty, sem zerar): o blur 3×3 de cada chunk lê
    // 1 tile do vizinho — se o vizinho não re-subir a textura, a borda
    // dele congela no valor antigo e vira "sombra" na fronteira.
    // O Renderer só re-uploada o visível, então o custo é ~zero.
    for (int cy = nb.y0 - 1; cy <= nb.y1 + 1; ++cy)
        for (int cx = nb.x0 - 1; cx <= nb.x1 + 1; ++cx)
            if (Chunk* c = findChunk(cx, cy))
                c->lightDirty = true;
    g_lastSweep.valid = true;
    g_lastSweep.x = wxPx;
    g_lastSweep.y = wyPx;
    g_lastSweep.level = level;
    if (level == 0) return;

    // 2. Cast único cross-chunk (máscara global, 1 passada).
    RaycastLight::castRaysCrossChunk(findChunk, {wxPx, wyPx}, level);

    // 3. Fonte no chunk dono.
    const int stx = static_cast<int>(std::floor(wxPx / bs));
    const int sty = static_cast<int>(std::floor(wyPx / bs));
    Chunk* src = findChunk(chunkOfTile(stx, Chunk::W),
                           chunkOfTile(sty, Chunk::H));
    if (!src) return; // fonte descarregada: área já foi limpa acima
    const int slx = stx - src->cx * Chunk::W;
    const int sly = sty - src->cy * Chunk::H;
    if (!src->inBounds(slx, sly)) return;
    src->blockLight[sly * Chunk::W + slx] = level;

    // 4. Push outward até fixpoint: ordem fixa (cy,cx), teto 4.
    // Anel ordenado + valores monótonos ⇒ converge; reentradas em U
    // precisam de 2-3 passadas (1 não basta — ver test_light_crosschunk).
    // Mudança = snapshot do grid: seed E flood escrevem nele; observar
    // só o seed sairia após 1 passada (vizinhos ainda zerados).
    struct Node {
        int cx, cy;
        Chunk* c;
    };
    std::vector<Node> ring;
    for (int cy = nb.y0; cy <= nb.y1; ++cy)
        for (int cx = nb.x0; cx <= nb.x1; ++cx)
            if (Chunk* c = findChunk(cx, cy))
                ring.push_back({cx, cy, c});
    for (int iter = 0; iter < 4; ++iter) {
        bool changed = false;
        for (auto& n : ring) {
            auto& grid = n.c->blockLight;
            const auto before = grid; // 256B: barato, decide o fixpoint
            seedBorderFromNeighbors(grid, *n.c, n.c->visibleMask, findChunk);
            floodMasked(grid, *n.c, n.c->visibleMask);
            changed |= (grid != before);
        }
        if (!changed) break;
    }
}

void LightPropagator::relightChunk(Chunk& c,
                                   const Chunk* top,
                                   const Chunk* left,
                                   const Chunk* right,
                                   const Chunk* bottom,
                                   uint8_t skyLevel)
{
    computeSkyLight(c, top, left, right, bottom, skyLevel);
    // Sem fontes estáticas hoje (player é overlay radial, não grid).
    c.blockLight.assign(Chunk::W * Chunk::H, 0);
    c.lightDirty = true;
}

sf::Image LightPropagator::buildLightImage(const Chunk& c,
                                           const Chunk* left,
                                           const Chunk* right,
                                           const Chunk* top,
                                           const Chunk* bottom,
                                           const Chunk* topLeft,
                                           const Chunk* topRight,
                                           const Chunk* bottomLeft,
                                           const Chunk* bottomRight)
{
    // Single-stage: blur K×K direto na resolução de saída (S texels/tile).
    // Fantasma P=H texels (cobre o kernel exato); cantos usam o chunk
    // diagonal (sem ele: 0). Display é nearest: sem clamp de GPU.
    // Perfis (header): A=(1,7) B=(2,5) C=(4,15).
    constexpr int S = kLightmapScale;
    constexpr int K = kBlurKernel;
    static_assert(K % 2 == 1, "kernel do blur precisa ser impar");
    constexpr int H = K / 2;
    constexpr int P = H;
    constexpr int OW = Chunk::W * S;
    constexpr int OH = Chunk::H * S;
    constexpr int BW = OW + 2 * P;
    constexpr int BH = OH + 2 * P;
    auto floordiv = [](int a, int n) {
        return a >= 0 ? a / n : -((-a + n - 1) / n);
    };
    std::vector<uint8_t> buf(BW * BH, 0);
    for (int by = 0; by < BH; ++by)
        for (int bx = 0; bx < BW; ++bx) {
            // Texel -> tile (piso p/ negativos), quadrante -> chunk.
            // |excursão| <= H < 16: cai sempre no vizinho imediato.
            const int tx = floordiv(bx - P, S);
            const int ty = floordiv(by - P, S);
            const Chunk* q = &c;
            if (tx < 0 && ty >= 0 && ty < Chunk::H) q = left;
            else if (tx >= Chunk::W && ty >= 0 && ty < Chunk::H) q = right;
            else if (ty < 0 && tx >= 0 && tx < Chunk::W) q = top;
            else if (ty >= Chunk::H && tx >= 0 && tx < Chunk::W) q = bottom;
            else if (tx < 0 && ty < 0) q = topLeft;
            else if (tx >= Chunk::W && ty < 0) q = topRight;
            else if (tx < 0 && ty >= Chunk::H) q = bottomLeft;
            else if (tx >= Chunk::W && ty >= Chunk::H) q = bottomRight;
            if (!q) continue; // 0: sem vizinho (borda do mundo/streaming)
            const int lx = (tx % Chunk::W + Chunk::W) % Chunk::W;
            const int ly = (ty % Chunk::H + Chunk::H) % Chunk::H;
            buf[by * BW + bx] = q->lightByte(lx, ly);
        }

    // Box-blur K×K; saída OW×OH (degrau = 50/S px).
    sf::Image img;
    img.create(OW, OH);
    for (int oy = 0; oy < OH; ++oy)
        for (int ox = 0; ox < OW; ++ox) {
            int sum = 0;
            for (int dy = -H; dy <= H; ++dy)
                for (int dx = -H; dx <= H; ++dx)
                    sum += buf[(oy + P + dy) * BW + (ox + P + dx)];
            const auto v = static_cast<uint8_t>(sum / (K * K));
            img.setPixel(static_cast<unsigned>(ox), static_cast<unsigned>(oy),
                         sf::Color(v, v, v, 255));
        }
    return img;
}

void LightPropagator::updateTexture(Chunk& c,
                                    const Chunk* left,
                                    const Chunk* right,
                                    const Chunk* top,
                                    const Chunk* bottom,
                                    const Chunk* topLeft,
                                    const Chunk* topRight,
                                    const Chunk* bottomLeft,
                                    const Chunk* bottomRight) {
    if (c.lightmap.getSize().x != static_cast<unsigned>(Chunk::W * kLightmapScale) ||
        c.lightmap.getSize().y != static_cast<unsigned>(Chunk::H * kLightmapScale)) {
        c.lightmap.create(Chunk::W * kLightmapScale, Chunk::H * kLightmapScale);
        c.lightmap.setSmooth(false); // nearest: sem clamp, sem cruz
    }

    c.lightmap.update(buildLightImage(c, left, right, top, bottom, topLeft,
                                      topRight, bottomLeft, bottomRight));
    c.lightDirty = false;
}

void LightPropagator::floodGeneric(std::vector<uint8_t>& grid,
                                   const Chunk& c,
                                   const std::vector<uint8_t>* left,
                                   const std::vector<uint8_t>* right,
                                   const std::vector<uint8_t>* bottom,
                                   const std::vector<Tile>* leftTiles,
                                   const std::vector<Tile>* rightTiles,
                                   const std::vector<Tile>* bottomTiles)
{
    bfs(grid, c, left, right, bottom, leftTiles, rightTiles, bottomTiles);
}

void LightPropagator::floodMasked(std::vector<uint8_t>& grid,
                                  const Chunk& c,
                                  const std::vector<uint8_t>& mask)
{
    std::queue<int> q;
    for (int i = 0; i < Chunk::W * Chunk::H; ++i) {
        // Ar/água entram sempre; sólido visível entra p/ emitir a partir
        // da face (ex.: fonte dentro de parede) — a propagação abaixo
        // decide o que cada vizinho recebe. Sólido invisível nunca conduz.
        if (grid[i] <= 1) continue;
        const Tile t = c.tiles[i];
        if ((t == Tile::Air || t == Tile::Water) || mask[i] != 0) q.push(i);
    }

    constexpr int dx[] = {1, -1, 0, 0};
    constexpr int dy[] = {0, 0, 1, -1};

    while (!q.empty()) {
        const int i = q.front();
        q.pop();
        const int x = i % Chunk::W;
        const int y = i / Chunk::W;
        const uint8_t cur = grid[i];
        if (cur <= 1) continue;

        for (int d = 0; d < 4; ++d) {
            const int nx = x + dx[d];
            const int ny = y + dy[d];
            if (nx < 0 || nx >= Chunk::W || ny < 0 || ny >= Chunk::H) continue;
            const int ni = ny * Chunk::W + nx;
            if (mask[ni] == 0) continue; // fora da visibilidade: sombra
            // Sólido visível: pinta a face com -2, sem fila (absorve).
            // (Espalhar A PARTIR de sólido visível continua permitido via
            // fila inicial: a máscara já limitou a região — o raio viu.)
            if (c.tiles[ni] != Tile::Air && c.tiles[ni] != Tile::Water) {
                const uint8_t att = LightPropagator::attenuationFor(c.tiles[ni]);
                if (cur > grid[ni] + att)
                    grid[ni] = static_cast<uint8_t>(cur - att);
                continue;
            }
            const uint8_t next = static_cast<uint8_t>(cur - 1);
            if (next > grid[ni]) {
                grid[ni] = next;
                q.push(ni);
            }
        }
    }
}

} // namespace support
