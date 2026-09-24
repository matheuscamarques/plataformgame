#pragma once
#include <functional>

namespace support {

// Pathfinder local (BFS 4-dir em janela): direção imediata de from→to
// desviando de sólido. Sem A* completo de propósito: janela pequena,
// recalculado a cada N ticks pela IA, com fallback guloso.
// Headless-safe: solidity vem por callback (teste usa lambda).
struct PathStep {
    int dx = 0;   // -1, 0 ou 1 (primeiro passo)
    int dy = 0;   // -1, 0 ou 1 (<0 = precisa subir)
    bool found = false;
};

// BFS de from até to dentro de [from±radius]. Alvo vale como destino
// mesmo se bloqueado. Mesmo tile = {0,0,true}. Sem rota = !found.
PathStep findStep(std::function<bool(int tx, int ty)> isBlocked,
                  int fromTx, int fromTy, int toTx, int toTy, int radius);

} // namespace support
