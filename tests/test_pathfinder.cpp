#include <cassert>
#include <cstdio>
#include <set>

#include "support/Enemies/Pathfinder.h"

// BFS janelado: direção imediata desviando de sólido. Puro (lambda).
int main() {
    using namespace support;

    { // OpenField (sem nada: reto no alvo)
        auto open = [](int, int) { return false; };
        const PathStep s = findStep(open, 0, 0, 5, 0, 20);
        assert(s.found && s.dx == 1 && s.dy == 0);
    }
    { // SameCell (já está: passo zero, found)
        auto open = [](int, int) { return false; };
        const PathStep s = findStep(open, 3, -2, 3, -2, 20);
        assert(s.found && s.dx == 0 && s.dy == 0);
    }
    { // DetourAroundWall (parede com fresta em cima: 1º passo válido)
        // Parede x=2, y em [-3,2]; fresta em y=3.
        auto wall = [](int tx, int ty) {
            return tx == 2 && ty >= -3 && ty <= 2;
        };
        const PathStep s = findStep(wall, 0, 0, 4, 0, 5);
        assert(s.found);
        assert(!wall(0 + s.dx, 0 + s.dy)); // não entra na parede
        assert(s.dx == 1 && s.dy == 0);    // BFS: (1,0) primeiro
    }
    { // EnclosedNoPath (4 vizinhos sólidos: sem rota)
        auto ring = [](int tx, int ty) {
            return (tx == 1 && ty == 0) || (tx == -1 && ty == 0) ||
                   (tx == 0 && ty == 1) || (tx == 0 && ty == -1);
        };
        const PathStep s = findStep(ring, 0, 0, 5, 5, 20);
        assert(!s.found);
    }
    { // OutOfWindow (alvo longe: inalcançável neste passo)
        auto open = [](int, int) { return false; };
        assert(!findStep(open, 0, 0, 100, 0, 20).found);
    }
    { // BlockedTarget (alvo em sólido: entra mesmo assim)
        auto wall = [](int tx, int ty) { return tx == 4 && ty == 0; };
        const PathStep s = findStep(wall, 0, 0, 4, 0, 20);
        assert(s.found);
    }
    { // RadiusClamp (0 vira 1: vizinho alcançável)
        auto open = [](int, int) { return false; };
        const PathStep s = findStep(open, 0, 0, 1, 0, 0);
        assert(s.found && s.dx == 1 && s.dy == 0);
    }
    { // StepIsUnit (passo sempre -1/0/1, nunca pula)
        auto maze = [](int tx, int ty) {
            return (tx + ty) % 7 == 3; // diagonais esparsas
        };
        for (int gx = -4; gx <= 4; ++gx) {
            for (int gy = -4; gy <= 4; ++gy) {
                const PathStep s = findStep(maze, 0, 0, gx, gy, 6);
                if (!s.found) continue;
                assert(s.dx >= -1 && s.dx <= 1);
                assert(s.dy >= -1 && s.dy <= 1);
                assert(!maze(s.dx, s.dy) || (gx == s.dx && gy == s.dy));
            }
        }
    }

    std::printf("pathfinder test OK\n");
    return 0;
}
