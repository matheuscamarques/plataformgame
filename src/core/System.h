#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace support {
struct GameContext; // definido em support/GameContext.h (Support conhece jogo)
}

namespace core {

// Sistema: unidade de update com prioridade declarada.
// Convenção: 0-99 input/time, 100-199 IA, 200-299 física,
// 300-399 combate, 400+ progressão/spawn, 900+ cleanup.
class System {
public:
    virtual ~System() = default;
    virtual const char *name() const = 0;
    virtual int priority() const = 0;
    virtual void tick(float dt, support::GameContext &ctx) = 0;
    virtual bool enabled() const { return true; }
};

class SystemScheduler {
public:
    template <typename T, typename... Args>
    T &add(Args &&...args) {
        auto sys = std::make_unique<T>(std::forward<Args>(args)...);
        T &ref = *sys;
        systems_.push_back(std::move(sys));
        dirty_ = true;
        return ref;
    }

    void tick(float dt, support::GameContext &ctx) {
        if (dirty_) {
            std::stable_sort(systems_.begin(), systems_.end(),
                [](const auto &a, const auto &b) {
                    return a->priority() < b->priority();
                });
            dirty_ = false;
        }
        for (auto &s : systems_) {
            if (s->enabled()) s->tick(dt, ctx);
        }
    }

    std::size_t count() const { return systems_.size(); }

private:
    std::vector<std::unique_ptr<System>> systems_;
    bool dirty_ = false;
};

} // namespace core
