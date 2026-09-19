#include <cassert>
#include <cmath>
#include <cstdio>
#include "core/Cooldown.h"

static bool near(float a, float b) { return std::fabs(a - b) < 1e-5f; }

int main() {
    using core::Cooldown;

    { // ReadyByDefault
        Cooldown c(1.0f);
        assert(c.ready() && !c.running() && near(c.remaining(), 0.f));
    }
    { // TriggerMakesRunning
        Cooldown c(1.0f);
        c.trigger();
        assert(!c.ready() && c.running() && near(c.remaining(), 1.f));
    }
    { // TickReducesRemaining
        Cooldown c(1.0f);
        c.trigger();
        c.tick(0.3f);
        assert(near(c.remaining(), 0.7f) && !c.ready());
    }
    { // ReadyAfterFullDuration
        Cooldown c(1.0f);
        c.trigger();
        c.tick(1.0f);
        assert(c.ready() && near(c.remaining(), 0.f));
    }
    { // TickClampsAtZero
        Cooldown c(0.5f);
        c.trigger();
        c.tick(100.f);
        assert(near(c.remaining(), 0.f) && c.ready());
    }
    { // RatioProgressesZeroToOne
        Cooldown c(1.0f);
        c.trigger();
        assert(near(c.ratio(), 0.f));
        c.tick(0.5f);
        assert(near(c.ratio(), 0.5f));
        c.tick(0.5f);
        assert(near(c.ratio(), 1.f));
    }
    { // TriggerWithNewDuration
        Cooldown c(1.0f);
        c.trigger();
        c.tick(0.5f);
        c.trigger(2.0f);
        assert(near(c.remaining(), 2.f) && near(c.duration(), 2.f));
    }
    { // ResetMakesReady
        Cooldown c(1.0f);
        c.trigger();
        c.tick(0.3f);
        c.reset();
        assert(c.ready());
    }

    std::printf("cooldown test OK\n");
    return 0;
}
