<!-- @file: docs/CLIENT_DRIVERS.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: Drivers IWorld que permitem solo e online coexistirem sem duplicação. -->
<!-- @details: Contrato IWorld, LocalWorld, NetworkWorld, PlayerPhysics compartilhada e 5 invariantes com mapeamento real de input. -->

# Client Drivers — `IWorld`

The single mechanism that lets solo and online coexist without duplication.
All proposed (`🆕`); the offline game underneath is `✅` and unchanged.

## The contract

```cpp
// world_io/IWorld.hpp  🆕 (Phase 2)
namespace world_io {

class IWorld {
public:
    virtual ~IWorld() = default;

    // 1. Per-frame input from the OS/controller.
    virtual void submitInput(const InputSnapshot& in, float dt) = 0;

    // 2. Advance one tick. Local simulates. Network reconciles.
    virtual void tick(float dt) = 0;

    // 3. Read-only views consumed by the Renderer.
    virtual const std::vector<EntityView>& entities() const = 0;
    virtual const PlayerView&              player()   const = 0;

    // 4. World access (tiles, light, camera).
    virtual World&  world()  = 0;
    virtual Camera& camera() = 0;

    // 5. Debug.
    virtual const char* driverName() const = 0;
};

} // namespace world_io
```

`EntityView` and `PlayerView` are **read-only**. The Renderer consumes them.
Nothing writes through them.

## The two implementations

### `LocalWorld` — offline

- Owns the player, the enemy system, the combat system (wraps today's `World` + scheduler).
- `submitInput` applies input directly.
- `tick` runs the existing scheduler (60 → 410, see ARCHITECTURE.md).
- Zero network dependencies. Compiles without `network/`.

### `NetworkWorld` — online

- Owns the same `World`, generated locally from the seed (terrain never syncs, only tile deltas).
- **Does not own enemies.** Server sends them.
- `submitInput` runs local prediction for the player and sends the input to the server.
- `tick` polls the socket, reconciles the prediction, samples the interpolation buffer.

## `PlayerPhysics` — the shared truth

This is the only physics implementation. Three callers:

1. `LocalWorld::tick` — offline simulation.
2. `NetworkWorld::submitInput` — local prediction.
3. The C++ NIF — authoritative server simulation (same language, same code — see BACKEND.md).

They must be **logically identical**, not bit-identical. The tolerance in
`NetworkWorld::reconcile` (4px) absorbs float drift.

```cpp
// physics/PlayerPhysics.hpp  🆕 (Phase 1)
namespace physics {

struct PlayerState {
    sf::Vector2f pos, vel;   // ✅ from Entity today
    bool   onGround;         // ✅ today's `jumping` flag, inverted
    int    facing;           // ✅
    int    hp;               // ✅ (hp/hpMax 10000, i-frames 0.6s)
    // 🆕 proposed with extraction (do NOT exist on Player today):
    float  posture;          // posture exists only on enemies (EnemyResources)
    float  coyoteTimer;      // does not exist — new feel mechanic
    float  jumpBuffer;       // does not exist — new feel mechanic
    float  rollTimer;        // Roll is bound (LShift) but has no mechanic yet
};

struct Input {
    bool left, right, jump;  // WASD/arrows + Space (real binds)
    bool attack;             // Heavy = K (melee). Throw = J is separate (ThrowSystem).
    bool roll;               // reserved bind, no mechanic yet
};

struct Output {
    PlayerState state;
    uint32_t    events;   // bitmask: rollStarted, hurt, landed, jumped
};

Output step(const PlayerState& prev,
            const Input&      in,
            const World&      world,
            float dt);

} // namespace physics
```

> Corrected: the first draft listed `posture/coyote/jumpBuffer/roll` as if they were real
> `Player` fields. They are not (`grep posture|coyote Player.h` is empty). They enter as **new**
> state owned by the extracted physics, which is also the correct moment to add coyote time
> and jump buffer the game is missing.

## Selecting the mode at runtime

```cpp
// main.cpp  🆕 (today it only calls Game::main in try/catch)
std::unique_ptr<world_io::IWorld> driver;

#ifdef WITH_NETWORK
    if (flags.online) {
        auto nw = std::make_unique<world_io::NetworkWorld>();
        if (!nw->connect(flags.host, flags.port)) {
            std::cerr << "[net] falling back to local\n";
            driver = std::make_unique<world_io::LocalWorld>();
        } else {
            driver = std::move(nw);
        }
    } else
#endif
    {
        driver = std::make_unique<world_io::LocalWorld>();
    }
```

Online failure never blocks the game. Offline is always available.

## The 5 invariants

1. **Offline never includes `network/`.** No `#include "network/..."` in any file compiled by `make game`.
2. **`NetworkWorld` never bypasses the interface.** It never touches `player_.pos` directly. All read/write goes through `predicted_` or `views_`.
3. **No shared mutable state between drivers.** Each driver owns its own world, player, camera.
4. **Offline is shippable at any time.** CI runs the offline tests. Releases always have a working `game` binary.
5. **Backend never forces refactor on offline.** If the NIF needs serialization, it creates its own `PlayerSnapshot` type. It does not force the C++ `Player` to be copyable.
