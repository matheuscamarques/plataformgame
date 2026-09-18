#include "Time.h"

#include <chrono>

namespace core {

float Time::fixedStep_ = 1.0f / 60.0f;
float Time::delta_ = 0.0f;
float Time::elapsed_ = 0.0f;
float Time::accumulator_ = 0.0f;
uint64_t Time::frames_ = 0;

void Time::beginFrame() {
    using clock = std::chrono::steady_clock;
    static clock::time_point prev;
    static bool first = true;
    auto now = clock::now();
    if (first) {
        prev = now;
        first = false;
    }
    float dt = std::chrono::duration<float>(now - prev).count();
    prev = now;
    if (dt < 0.0f) dt = 0.0f;
    if (dt > 0.25f) dt = 0.25f; // anti-espiral após hitch/debug
    delta_ = dt;
    elapsed_ += dt;
    frames_++;
}

int Time::consumeTicks() {
    accumulator_ += delta_;
    int n = 0;
    while (accumulator_ >= fixedStep_) {
        accumulator_ -= fixedStep_;
        n++;
    }
    return n;
}

} // namespace core
