#include <cassert>
#include <cmath>
#include <cstdio>
#include <thread>
#include <chrono>
#include "core/Time.h"
#include "core/Math.h"

int main() {
    // default 1/60 preservado para quem não trava valor próprio
    assert(std::fabs(core::Time::fixedStep() - 1.0f / 60.0f) < 1e-6f);

    core::Time::setFixedStep(1.0f / 30.0f);
    assert(std::fabs(core::Time::fixedStep() - 1.0f / 30.0f) < 1e-6f);

    // sem tempo passado: zero ticks, delta >= 0, frames conta
    core::Time::beginFrame();
    assert(core::Time::consumeTicks() == 0);
    assert(core::Time::deltaTime() >= 0.0f);
    assert(core::Time::frameCount() == 1);
    float e0 = core::Time::elapsed();

    // 40ms > passo 1/30 (~33.3ms): pelo menos 1 tick
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    core::Time::beginFrame();
    int n = core::Time::consumeTicks();
    assert(n >= 1 && n <= 3);
    assert(core::Time::elapsed() >= e0);

    // Math escalar
    assert(core::clamp(5, 0, 3) == 3);
    assert(core::clamp(-1, 0, 3) == 0);
    assert(std::fabs(core::lerp(0.0f, 10.0f, 0.25f) - 2.5f) < 1e-6f);
    assert(core::sign(-7) == -1 && core::sign(0) == 0 && core::sign(7) == 1);
    assert(std::fabs(core::approach(0.0f, 10.0f, 3.0f) - 3.0f) < 1e-6f);
    assert(std::fabs(core::approach(9.0f, 10.0f, 3.0f) - 10.0f) < 1e-6f);

    std::printf("core time/math test OK (ticks=%d)\n", n);
    return 0;
}
