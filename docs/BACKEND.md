<!-- @file: docs/BACKEND.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: Backend Elixir com NIF C++ — BEAM orquestra, NIF calcula. -->
<!-- @details: GenServer por seed a 30Hz, fronteira do NIF puro com dirty scheduler, sharding, ciclo de sessão e tabelas quentes/frias. -->

# Backend — Elixir + C++ NIF

Two languages, one responsibility each. **NIF does not orchestrate.
BEAM does not calculate.**

> Locked correction: the NIF is **C++ (erl_nif via `elixir_make`, in `c_src/`)**, not Rust.
> Rationale: the deterministic sim already exists in C++ (`src/core/`, `src/world/`, pure parts
> of `src/support/`). A Rust NIF would require `rustler` (not in `mix.exs`) **and** a full
> rewrite of the physics — destroying the "shared truth" the whole design relies on.
> C++ NIF = same code, same flags (`-ffp-contract=off -fno-fast-math`).

## The split

| Layer | Language | What it does | What it never does |
|---|---|---|---|
| Orchestration | Elixir (BEAM) | Session lifecycle, matchmaking, broadcast, persistence | CPU-heavy loops |
| Calculations | C++ (NIF) | Pure functions: `(input, state-slice) → output` | I/O, mutable state, sockets |

## BEAM — one GenServer per seed

Every seed is a process. Every session is a supervision tree.

```elixir
defmodule Session do
  use GenServer

  @max_players 4
  @tick_ms 33     # 30 Hz, same as the offline fixed-step (1/30)

  def start_link(seed) do
    GenServer.start_link(__MODULE__, %{
      seed: seed,
      players: %{},
      world_state: World.init(seed)
    }, name: via(seed))
  end

  def handle_info(:tick, state) do
    {new_state, events} =
      state
      |> Physics.tick_all()    # NIF calls, pure
      |> Combat.resolve_all()  # NIF calls, pure
      |> Events.emit_all()

    broadcast_snapshot(new_state)
    handle_events(events)

    Process.send_after(self(), :tick, @tick_ms)
    {:noreply, new_state}
  end
end
```

**Rules:**
- The BEAM scheduler **is** the game loop. No manual `while`.
- Each seed is isolated. Failure in one does not affect others.
- Supervision trees restart sessions on crash.
- Node distribution routes `seed → node` via consistent hashing (`:erlang.phash2` + `:pg`/Registry).

## NIF — calculations only, dirty scheduler

```c
// c_src/physics_nif.c  🆕
#include <erl_nif.h>

// Pure: (state, input, world-slice) -> (state', events).
// No globals, no I/O, no allocations held across calls.
static ERL_NIF_TERM step_player(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    /* decode pos/vel/input/nearby-tiles ... run physics::step ... encode */
}

static ErlNifFunc nifs[] = {
    {"step_player", 5, step_player, ERL_NIF_DIRTY_JOB_CPU_BOUND}
    // ^ every NIF that can exceed ~1ms MUST carry the dirty flag.
    // Normal schedulers must never block.
};
```

**Rules:**
- The NIF receives only the slices it needs (nearby tiles, one player state). It never sees the full world.
- NIFs are **pure functions**. No globals. No allocations held across calls.
- The existing C++ sim compiles into the NIF with the same determinism flags.

## The session lifecycle

```
1. Client sends: JOIN(seed, player_id, token)
2. Cluster routes to node responsible for seed
3. Node spawns/recovers the GenServer for that seed
4. Server adds player to state, sends welcome snapshot
5. Tick loop runs at 30 Hz
6. On player disconnect: remove from state, broadcast
7. On empty state after grace period (60s): session shuts down
```

**Sessions are ephemeral.** State lives in memory. If the process dies,
it restarts empty. **No persistence per session.**

## Sharding (estimates, to be measured in P5)

```
100k players
  ÷ 4 per session = 25k sessions
  ÷ 125 sessions per node = 200 nodes
```

Each node is a BEAM VM. Budget assumption (*estimate*): ~1ms of NIF work per session tick →
125 × 30 × 1ms ≈ 3.75 CPU-s/s, i.e. ~4 cores per node. Scales linearly: more nodes, more
capacity, no global coordinator. P5 measures the real 1ms figure headless before any scaling claim.

## What lives in Postgres (later, Phase 9+)

Only **account-level** data, never session state:

| Table | Purpose |
|---|---|
| `accounts` | Display name, reputation |
| `runs` | Completed run history (seed, tick count, outcome) |
| `seeds` | Global seed graph (who owns which seed) |
| `edges` | Discovered portals between seeds |
| `reputation_log` | Every reputation event (audit) |

## What lives in ETS (hot)

| Table | Purpose |
|---|---|
| `seed_index` | `seed_hash → node_pid` |
| `session_registry` | `session_id → GenServer` |
| `player_index` | `player_id → session_pid` |

ETS is in-memory, per-node, fast. Rebuilt on demand.
