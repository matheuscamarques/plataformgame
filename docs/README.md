# Documentation

Internal design docs. These are **decision-locked** — no TBDs.
`✅` = exists in the repo today. `🆕` = proposed, built in the phase cited.

| Doc | What it covers |
|---|---|
| [ARCHITECTURE.md](ARCHITECTURE.md) | System overview, module map, real scheduler order |
| [CLIENT_DRIVERS.md](CLIENT_DRIVERS.md) | `IWorld`, `LocalWorld`, `NetworkWorld`, `PlayerPhysics` |
| [BACKEND.md](BACKEND.md) | Elixir BEAM + C++ NIF, GenServer per seed |
| [PERSISTENCE.md](PERSISTENCE.md) | What survives across sessions, and where |
| [PORTALS.md](PORTALS.md) | Portal formula, artifact discovery, seed graph |
| [PVP.md](PVP.md) | Invader roles, reputation, betrayal |
| [ROADMAP_ONLINE.md](ROADMAP_ONLINE.md) | 22-week roadmap in verifiable milestones |

**Status:** pre-implementation. Code exists for the offline single-player
(fixed-step `1/30`, 93 headless tests green, `make test-layers` gate).
The online layer is designed, not built.

Related: [MULTIPLAYER.md](MULTIPLAYER.md) (vision + 10 locked decisions),
[PARALLEL_WORK.md](PARALLEL_WORK.md) (how to contribute without conflicts).
