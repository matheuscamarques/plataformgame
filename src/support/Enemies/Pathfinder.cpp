#include "Pathfinder.h"

#include <queue>
#include <vector>

namespace support {

PathStep findStep(std::function<bool(int tx, int ty)> isBlocked,
                  int fromTx, int fromTy, int toTx, int toTy, int radius) {
    if (radius < 1) radius = 1;
    const int dtx = toTx - fromTx;
    const int dty = toTy - fromTy;
    if (dtx == 0 && dty == 0) return {0, 0, true};
    // Fora da janela: inalcançável neste passo (IA usa fallback guloso).
    if (dtx < -radius || dtx > radius || dty < -radius || dty > radius)
        return {};

    const int w = 2 * radius + 1;
    const auto idx = [&](int tx, int ty) {
        return (ty - (fromTy - radius)) * w + (tx - (fromTx - radius));
    };
    std::vector<int> parent(w * w, -2); // -2 = não visitado
    std::queue<int> q;
    const int start = idx(fromTx, fromTy);
    parent[start] = -1;
    q.push(start);

    static const int kDirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    int goal = -1;
    while (!q.empty()) {
        const int cur = q.front();
        q.pop();
        const int ctx_ = (fromTx - radius) + (cur % w);
        const int cty = (fromTy - radius) + (cur / w);
        if (ctx_ == toTx && cty == toTy) {
            goal = cur;
            break;
        }
        for (const auto& d : kDirs) {
            const int nx = ctx_ + d[0];
            const int ny = cty + d[1];
            if (nx < fromTx - radius || nx > fromTx + radius ||
                ny < fromTy - radius || ny > fromTy + radius)
                continue;
            const int ni = idx(nx, ny);
            if (parent[ni] != -2) continue;
            // Alvo entra mesmo bloqueado; resto só por ar.
            const bool isGoal = (nx == toTx && ny == toTy);
            if (!isGoal && isBlocked(nx, ny)) continue;
            parent[ni] = cur;
            q.push(ni);
        }
    }
    if (goal < 0) return {}; // sem rota na janela

    // Volta até o filho do início: esse é o 1º passo.
    int cur = goal;
    while (parent[cur] != start) cur = parent[cur];
    const int sx = (fromTx - radius) + (cur % w) - fromTx;
    const int sy = (fromTy - radius) + (cur / w) - fromTy;
    const int dx = (sx > 0) ? 1 : (sx < 0 ? -1 : 0);
    const int dy = (sy > 0) ? 1 : (sy < 0 ? -1 : 0);
    return {dx, dy, true};
}

} // namespace support
