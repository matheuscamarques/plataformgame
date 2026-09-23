<!-- @file: docs/ARCHITECTURE.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: Arquitetura em 4 camadas com mapa de módulos e ordem real do scheduler. -->
<!-- @details: Camadas client/driver/sim/backend, prioridades verificadas no código, GameContext real por ponteiros e modos de falha protegidos. -->

# Architecture

## The 4 layers

```
┌──────────────────────────────────────────────────────────────┐
│  CLIENT (C++17 / SFML)  ✅                                   │
│  Renderer · Audio · UI · Input                               │
├──────────────────────────────────────────────────────────────┤
│  DRIVER LAYER  (IWorld)  🆕 (Phase 2)                        │
│  LocalWorld (offline)   |   NetworkWorld (online)            │
├──────────────────────────────────────────────────────────────┤
│  SHARED SIMULATION                                           │
│  PlayerPhysics 🆕 (Phase 1) · World ✅ · TileGrid ✅          │
├──────────────────────────────────────────────────────────────┤
│  BACKEND (only in online mode)  🆕 (Phase 5)                 │
│  Elixir BEAM · GenServer per seed · C++ NIF (calculations)   │
└──────────────────────────────────────────────────────────────┘
```

## Module map

```
core/          ✅ Kernel — no game logic, no upward deps (test-layers gated)
world/         ✅ Procedural world, generation, lighting
support/       ✅ Gameplay systems (Combat, Enemies, Skills, Effects, UI)
physics/       🆕 PlayerPhysics — shared, pure, no state (Phase 1)
world_io/      🆕 IWorld, LocalWorld, NetworkWorld (Phase 2)
network/       🆕 Socket, SnapshotBuffer, Reconcile (WITH_NETWORK only, Phase 3)
entities/      ✅ Entity, Player
game/          ✅ Game, App, Renderer, Bootstrapper
```

## The rule that makes both modes coexist

**`make game` compiles without the network layer or the backend.**

- The offline binary depends on `core/`, `world/`, `support/`, `physics/`, `world_io/LocalWorld`.
- The online binary adds `network/` and `world_io/NetworkWorld`.
- The backend lives in this monorepo at `multiplayer/online_game_engine/` (Elixir + C++ NIF
  in `c_src/`), not in a separate repo.

If the backend is down, `make game` still works. Contributors never need Elixir.

## Update order (real scheduler priorities, verified in code)

| Priority | System | Responsibility | Estado |
|---|---|---|---|
| 60 | StratumManager | Stratum boundary detection | ✅ |
| 150 | EnemySystem | AI tick, physics, behavior dispatch | ✅ |
| 220 | ThrowSystem | Throwable physics, fuse, explosion trigger | ✅ |
| 250 | BodySystem | Rebuild part hitboxes after movement | ✅ |
| 300 | MeleeSystem | Player melee (`K`) resolution, 1 hit/swing | ✅ |
| 310 | ContactDamageSystem | Player × enemy overlap | ✅ |
| 320 | ExplosionSystem | Damage + tile destruction | ✅ |
| 330 | DeathSystem | Cleanup, drops, events | ✅ |
| 350 | ParticleSystem | Debris and dust pools | ✅ |
| 400 | DropSystem | XP / item orbs, magnet | ✅ |
| 410 | SpawnSystem | Budgeted spawn per stratum | ✅ |
| 200 | MovementSystem | Player physics step (calls `physics::step`) | 🆕 Phase 1 |

> Corrected against the code: there is no `InputSystem`, `RoomSystem` or `CleanupSystem`
> (input is `InputMap` event-driven, consumed in `Game::tick`; destruction is deferred via
> `destroyPending` + `DeathSystem::removeDead`). `MovementSystem` (200) is created in Phase 1
> when `Player::tick` is extracted into `physics::step`.

The scheduler is deterministic (`stable_sort` by `priority()`). Adding a system never changes
execution order of existing systems — you pick a priority, and the order is stable.

## GameContext — non-owning views (real shape)

Not references, not a bus. Nullable pointers owned by `Game`, filled per tick in `Game::tick`
(`src/game/App.cpp`), also usable headless with nulls (call sites check):

```cpp
// src/support/GameContext.h (abbreviated — read the file for ownership notes)
struct GameContext {
    World            *world   = nullptr;
    ::Player         *player  = nullptr;
    InputMap         *input   = nullptr;
    EnemySystem      *enemies = nullptr;
    ThrowSystem      *throws  = nullptr;
    ExplosionSystem  *explodes = nullptr;
    DropSystem       *drops   = nullptr;
    std::vector<ExplosionTarget> *explosionTargets = nullptr; // Game stack, per tick
    ScreenshotSystem *screenshots = nullptr;  // null headless
    DebugFeed        *debug       = nullptr;  // null headless
    core::AudioSystem *audio      = nullptr;  // null headless
    Camera           *camera      = nullptr;  // null = no shake
};
```

There is **no `EventBus`, no `EntityRegistry`** (the latter appears only as a future note in a
`spatialhash.h` comment). Cross-system effects use explicit wiring: injected systems
(`setParticleSystem`, `setDropSystem`), the per-tick `explosionTargets` list, and drops/XP
orbs. The `IWorld` refactor (Phase 2) reshapes but does not replace this contract.

## Failure modes we protect against

| Failure | Protection |
|---|---|
| Two systems edit the same entity list | Deferred destruction (`destroyPending`, swept in `removeDead`) |
| Physics diverges between client and server | Tolerance-based reconcile (~4px), not bit-exact |
| Backend down | Offline binary works standalone (`make game`) |
| Player cheats movement | Server authoritative, client predicts only |
| `core/` gains game logic | `make test-layers` grep gate |
| NIF blocking BEAM scheduler | Dirty CPU scheduler flag (`ERL_NIF_DIRTY_JOB_CPU_BOUND`) |
