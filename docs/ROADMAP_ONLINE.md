<!-- @file: docs/ROADMAP_ONLINE.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: Roadmap online em 12 fases verificáveis (22 semanas de trabalho online). -->
<!-- @details: Fases P1-P12 com entregáveis e critérios de saída, do refactor PlayerPhysics ao anti-cheat, em paralelo com o offline. -->

# Online Roadmap — 22 weeks, layered

> Locked correction: the total is **22 weeks** of online work (1+2+2+2+3+2+2+1+1+2+3+1),
> not 18. At interleaved pace with offline work, calendar time is 5–6 months.

Every phase ends with something playable. Phases are **not consecutive** —
offline work continues in parallel (every offline week stays shippable via `make game`).

## Phase 1 — Refactor `PlayerPhysics` (1 week)

**Goal:** extract pure physics from `Player::tick` (`src/entities/Player/Player.cpp:160`).

**Deliverables:**
- `physics/PlayerPhysics.hpp/.cpp` (see CLIENT_DRIVERS.md for the struct —
  new fields like coyote/jumpBuffer arrive here, they don't exist on `Player` today)
- `Player::tick` becomes a wrapper (~40 lines)
- Offline behavior is **behavior-identical** (verified by existing tests, not bit-identical)

**Exit criteria:** all existing tests pass. `make game` works. No new feature.

## Phase 2 — `IWorld` + `LocalWorld` (2 weeks)

**Goal:** introduce the driver interface without changing gameplay.

**Deliverables:**
- `world_io/IWorld.hpp`
- `world_io/LocalWorld.hpp/.cpp`
- `Game` reads through `IWorld` instead of direct member access
- `Renderer` consumes `EntityView` / `PlayerView`

**Exit criteria:** offline still works. `make game` produces the same binary
behavior. No new feature.

## Phase 3 — Socket + SnapshotBuffer (2 weeks)

**Goal:** client can send and receive over the wire.

**Deliverables:**
- `network/Socket.hpp/.cpp` (WebSocket over TCP for the MVP)
- `network/SnapshotBuffer.hpp/.cpp`
- `network/Protocol.hpp/.cpp`
- `world_io/NetworkWorld.hpp/.cpp` (basic connect, no reconcile yet)

**Exit criteria:** client connects to a local stub server. Sends input.
Receives a fake snapshot. Renders it.

## Phase 4 — Player prediction + reconciliation (2 weeks)

**Goal:** local player moves immediately, server corrects.

**Deliverables:**
- `NetworkWorld::submitInput` runs `physics::step` locally
- `NetworkWorld::reconcile` applies server correction (tolerance 4px)
- Interpolation buffer of 100ms for other entities

**Exit criteria:** local player feels responsive at 100ms latency.
No visible snap unless input is lost.

## Phase 5 — Backend MVP: GenServer + NIF (3 weeks)

**Goal:** a real Elixir server (in `multiplayer/online_game_engine/`, not a separate repo).

**Deliverables:**
- `session.ex` — GenServer, 30 Hz tick
- C++ NIF in `c_src/` via `elixir_make`, dirty CPU flag (see BACKEND.md)
- `seed_router.ex` — hash-based routing
- Local test: client connects, moves, sees own snapshot

**Exit criteria:** one client, one server, one seed. Player moves correctly.
NIF does not block scheduler.

## Phase 6 — Co-op 2 players (2 weeks)

**Goal:** two clients, one session.

**Deliverables:**
- Session supports 2 players (cap stays 4; solo = session of 1)
- Snapshot packs both players
- Both see each other interpolate correctly
- HP scaling `1 + (n−1) × 0.6`

**Exit criteria:** two clients on the same seed see each other move.
Kill a slime together.

## Phase 7 — Run persistence (2 weeks)

**Goal:** run survives disconnect.

**Deliverables:**
- `IPersistence` + `LocalPersistence`
- `RunState` serialization (JSON, shape in PERSISTENCE.md)
- Save on checkpoint, death, and app close
- Load on session resume

**Exit criteria:** close the app mid-run. Reopen. HP, inventory, destroyed
tiles-since-checkpoint are preserved.

## Phase 8 — Auth + mock (1 week)

**Goal:** account layer with fallback.

**Deliverables:**
- `IAuthProvider` + `MockAuth` (+ Steam impl only if Steam SDK is integrated —
  there is no Steam in the repo today)
- User data directory per account
- Modes wired (solo offline first; online modes as they land)

**Exit criteria:** switch between available modes without recompiling.

## Phase 9 — Seed graph (Postgres) (1 week)

**Goal:** persistent seed discovery.

**Deliverables:**
- Postgres schema (`seeds`, `edges`)
- Backend endpoint: `GET /seeds/:hash`, `POST /seeds`
- Client UI: grimoire shows known artifacts
- Portal teleport works end-to-end (formula in PORTALS.md)

**Exit criteria:** two players on different machines can discover the same
seed via coordinates.

## Phase 10 — Matchmaking + portal (2 weeks)

**Goal:** invite another player to a seed.

**Deliverables:**
- `POST /portal/request` (invader side)
- Host receives request via WebSocket
- Accept / refuse / expel (rules in PVP.md)
- Session join for invader (counts toward cap 4)

**Exit criteria:** two players, one seed, one portal. Host and invader
in the same session.

## Phase 11 — PVP roles (3 weeks)

**Goal:** Ally / Hostile / Traitor.

**Deliverables:**
- Role selection in portal UI
- Server-side role enforcement
- Telegraph for Hostile entry
- Reputation system in Postgres
- Betrayal detection and permanent mark

**Exit criteria:** three test sessions: Ally helps, Hostile kills, Ally
betrays. Reputation changes correctly.

## Phase 12 — Anti-cheat baseline (1 week)

**Goal:** basic protection.

**Deliverables:**
- Server re-simulates player movement from inputs
- Rejects client position deltas > 20px
- Logs suspicious patterns
- Rate limits portal requests

**Exit criteria:** a modified client that teleports instantly gets corrected.

## Total — 22 weeks

| Phase | Weeks | Proves |
|---|---|---|
| 1 | 1 | Physics can be extracted |
| 2 | 2 | Two drivers can coexist |
| 3 | 2 | Network plumbing works |
| 4 | 2 | Prediction is feasible |
| 5 | 3 | BEAM + NIF is viable |
| 6 | 2 | Co-op core is sound |
| 7 | 2 | Runs can persist |
| 8 | 1 | Accounts integrate |
| 9 | 1 | Seed graph is real |
| 10 | 2 | Matchmaking works |
| 11 | 3 | PVP roles are enforceable |
| 12 | 1 | Cheating is bounded |

**Each phase is a bet on the next.** If phase 6 fails, phases 7–12 are
not worth building. The roadmap is designed so the cheapest tests come first.

## What NOT to do first

Do not touch the rest of online for 4–6 weeks after Phase 1. Finish the
single-player game first. The recommended immediate next step is writing
`physics/PlayerPhysics.hpp/.cpp` with headless tests — no backend, no network.
