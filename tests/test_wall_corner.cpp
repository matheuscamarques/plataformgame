// Regressão: túnel lateral em canto de parede (pior na esquerda).
// Tiles 50px, player 30x50. Cobre: correção esquerda com largura do
// player (+30 em vez de +50, embed de 20px dentro da parede),
// gate do teto (bloco na altura do peito ia p/ correção vertical e
// teleportava p/ baixo) e sensores laterais cegos na faixa da cabeça
// ([0.3h,0.7h] não via quina alta). Em blocos 50px a correção do teto
// antiga era numericamente exata — o gate muda o ROTEAMENTO, não o valor.
#include <cmath>
#include <cstdio>
#include <memory>

#include "entities/Entity.hpp"
#include "entities/Player/Player.h"
#include "defines.h"

namespace {
bool near(float a, float b, float eps = 0.5f) {
    return std::fabs(a - b) <= eps;
}

// Anda em linha reta aplicando isColide/collide como App::tick.
void walk(Player &p, Entity &block, float dx, int steps) {
    for (int t = 0; t < steps; ++t) {
        p.setX(p.getX() + dx);
        if (p.isColide(block)) p.collide(block);
    }
}
} // namespace

int main() {
    { // ChestHighRight (bloco na altura do peito, vindo da esquerda)
        // Bloco rows 88..138 vs player rows 100..150: gate do topo
        // desvia p/ lateral (base 138 > y+12.5); lado bloqueia em x=30.
        // Antigo: correção vertical teleportava p/ y=138 (embaixo).
        auto block = std::make_unique<Entity>(core::kIdColide, 60.f, 88.f,
                                              core::kBlockSize, core::kBlockSize);
        Player p;
        p.setX(0.f);
        p.setY(100.f);
        walk(p, *block, 5.f, 40);
        std::printf("chestHigh: x=%.1f y=%.1f (edge=%.1f)\n",
                    p.getX(), p.getY(), p.getX() + p.getW());
        if (p.getX() + p.getW() > 60.5f) {
            std::printf("FALHOU: atravessou o canto pela direita\n");
            return 1;
        }
        if (!near(p.getY(), 100.f)) {
            std::printf("FALHOU: teleportou vertical (y=%.1f)\n", p.getY());
            return 1;
        }
    }
    { // FullWallLeft (parede cheia, vindo da direita — o caso "esquerda")
        // Correção antiga: bloco.x + 30 (largura do player) em vez de
        // bloco.x + 50 → embed de 20px. Nova: encosta exato em x=50.
        auto block = std::make_unique<Entity>(core::kIdColide, 0.f, 100.f,
                                              core::kBlockSize, core::kBlockSize);
        Player p;
        p.setX(90.f);
        p.setY(100.f);
        walk(p, *block, -5.f, 40);
        std::printf("fullWallLeft: x=%.1f y=%.1f\n", p.getX(), p.getY());
        if (!near(p.getX(), 50.f)) {
            std::printf("FALHOU: esquerda grudou fora do exato (x=%.1f)\n", p.getX());
            return 1;
        }
        if (!near(p.getY(), 100.f)) {
            std::printf("FALHOU: mexeu no Y encostando de lado\n");
            return 1;
        }
    }
    { // LintelPassUnder (lintel roçando a cabeça, vindo da direita)
        // Bloco rows 52..102 (50px): penetração de 2px no sensor — roçar
        // não empurra (>= 6px p/ corrigir); topo encosta exato em y=102
        // (bonk limpo, sem teleporte). Lock test: passa no código antigo
        // também (lá o teto era exato p/ 50px); trava o threshold — sem
        // ele, o lado emperra em x=50 e este assert quebra.
        auto block = std::make_unique<Entity>(core::kIdColide, 0.f, 52.f,
                                              core::kBlockSize, core::kBlockSize);
        Player p;
        p.setX(90.f);
        p.setY(100.f);
        walk(p, *block, -5.f, 40);
        std::printf("lintel: x=%.1f y=%.1f\n", p.getX(), p.getY());
        if (p.getX() > 10.f) {
            std::printf("FALHOU: emperrou embaixo do lintel\n");
            return 1;
        }
        if (p.getY() < 101.5f || p.getY() > 102.5f) {
            std::printf("FALHOU: teleportou vertical no lintel (y=%.1f)\n", p.getY());
            return 1;
        }
    }

    { // JumpIntoCeiling (teleporte do pulo não sobe pelo teto)
        // Bloco rows 50..100, player teleporta -25/tick (y 100→75→...).
        // Gate no meio (base 100 <= y+25) dispara no primeiro contato e
        // pula os lados (sem fling). Antigo (gate 0.25h): falhava o gate
        // e a lateral atirava o player p/ x=-30.
        auto block = std::make_unique<Entity>(core::kIdColide, 0.f, 50.f,
                                              core::kBlockSize, core::kBlockSize);
        Player p;
        p.setX(10.f);
        p.setY(100.f);
        for (int t = 0; t < 3; ++t) {
            p.setY(p.getY() - 25.f); // jump teleport bruto, como Player::tick
            if (p.isColide(*block)) p.collide(*block);
        }
        std::printf("ceiling: x=%.1f y=%.1f\n", p.getX(), p.getY());
        if (!near(p.getY(), 100.f)) {
            std::printf("FALHOU: subiu pelo teto (y=%.1f)\n", p.getY());
            return 1;
        }
        if (!near(p.getX(), 10.f)) {
            std::printf("FALHOU: fling lateral no bonk (x=%.1f)\n", p.getX());
            return 1;
        }
    }

    std::printf("wall corner test OK\n");
    return 0;
}
