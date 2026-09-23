/**
 * @file tests/test_camera.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Teste headless que trava follow com deadzone e retorno à origem.
 * @details Cobre Camera centralizada e simétrica, roda com make test que compila em build/tests/test_camera.
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include "support/Camera/Camera.h"

int main() {
    support::Camera c;
    c.setViewport(800, 800);

    // deadzone zero: sempre centraliza, simétrico
    c.follow(500, 900);
    assert(std::fabs(c.position().x - 100.f) < 1e-4f);
    assert(std::fabs(c.position().y - 500.f) < 1e-4f);

    // volta para a esquerda/origem: acompanha (era o bug)
    c.follow(0, 0);
    assert(std::fabs(c.position().x + 400.f) < 1e-4f);
    assert(std::fabs(c.position().y + 400.f) < 1e-4f);

    // viewRect acompanha
    auto r = c.viewRect();
    assert(r.left == -400.f && r.top == -400.f && r.width == 800.f && r.height == 800.f);

    // roundtrip mundo <-> tela
    auto w = c.screenToWorld({10, 20});
    assert(w.x == -390.f && w.y == -380.f);
    auto s = c.worldToScreen(w);
    assert(std::fabs(s.x - 10.f) < 1e-4f && std::fabs(s.y - 20.f) < 1e-4f);

    // deadzone segura a câmera perto do centro
    support::Camera c2;
    c2.setViewport(800, 800);
    c2.setDeadzone(200, 200);
    c2.follow(400, 400); // centro exato: parada na origem
    assert(c2.position().x == 0.f && c2.position().y == 0.f);
    c2.follow(450, 400); // |450-400|*2=100 <= 200: parada
    assert(c2.position().x == 0.f);
    c2.follow(0, 400); // |0-400|*2=800 > 200: move p/ -400
    assert(std::fabs(c2.position().x + 400.f) < 1e-4f);

    // lerp 0.5 anda metade do caminho até o alvo
    support::Camera c3;
    c3.setViewport(800, 800);
    c3.setLerp(0.5f);
    c3.follow(1000, 0); // alvo x=600
    assert(std::fabs(c3.position().x - 300.f) < 1e-3f);

    std::printf("camera test OK\n");
    return 0;
}
