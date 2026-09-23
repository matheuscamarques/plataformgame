<!-- @file: docs/PERSISTENCE.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: O que sobrevive entre sessões e onde, por fases. -->
<!-- @details: Matriz de camadas, formato RunState, IPersistence local/rede, replay como auditoria e o que nunca persiste. -->

# Persistence — what survives and where

Current state (corrected): **zero on-disk persistence** — `StratumManager`/`RunManager`/`Inventory`
live in memory, there is no `MetaState` yet. Everything below is the locked plan, phased.

## The rule

**Sessions are ephemeral. Accounts are durable. Runs are durable.
Worlds are pure functions.**

## The matrix

| Layer | Lives in | Survives session? | Survives app close? | Survives weeks? |
|---|---|---|---|---|
| **Session state** (players, enemies) | GenServer memory | N/A | ❌ | ❌ |
| **Run state** (HP, inventory, checkpoint) | Disk on client + backend | ✅ | ✅ | ✅ |
| **Account meta** (deepest, deaths, unlocks) | Postgres (Phase 9+) / `save.json` (MVP) | ✅ | ✅ | ✅ |
| **Seed graph** (portals, ownership) | Postgres (Phase 9+) | ✅ | ✅ | ✅ |
| **Reputation** | Postgres (Phase 11) | ✅ | ✅ | ✅ |
| **Replay** (inputs) | Disk (audit only) | ✅ | ✅ | ✅ |
| **Generated terrain** | **Nowhere. Derived from seed.** | — | — | — |
| **Destroyed tiles** | Session memory only (die with it) | ❌ | ❌ | ❌ |

> Locked correction: destroyed tiles do **not** persist across sessions — neither on return
> nor via a backend delta log. They live in session memory and die with it (see PORTALS.md).
> Persisting them would require a global coordinator and break the sharding math.

## Run persistence — the shape (illustrative, not literal registry content)

```json
{
  "version": 1,
  "run": {
    "seed": 1337,
    "tick_count": 108000,
    "started_at": "2026-09-22T14:30:00Z",
    "player": { "pos": [100.5, 512.0], "hp": 45, "facing": 1 },
    "equipment": {
      "RightHand": "iron_sword",
      "Head": "iron_helm",
      "Chest": "iron_chest",
      "Legs": null
    },
    "inventory": [
      {"id": "stone", "qty": 45},
      {"id": "potion_heal", "qty": 3}
    ],
    "checkpoint": { "stratum": 4, "pos": [100, 512] },
    "tiles_destroyed_since_checkpoint": [[100, 200], [101, 200]]
  }
}
```

**Size:** ~2 KB base + ~8 bytes per destroyed tile. A long run (30 min, heavy TNT) reaches
~200 KB. Acceptable. **Frequency:** saved on checkpoint, on death, and on app close. Not every frame.

## Local vs Network persistence

Two implementations of the same interface:

```cpp
// world_io/IPersistence.hpp  🆕 (Phase 7)
class IPersistence {
public:
    virtual ~IPersistence() = default;
    virtual void       saveMeta(const MetaState&) = 0;
    virtual MetaState  loadMeta() = 0;
    virtual void       saveRun(const RunState&)   = 0;
    virtual RunState   loadRun(const std::string& run_id) = 0;
};
```

| Mode | Implementation | Storage |
|---|---|---|
| Solo offline | `LocalPersistence` | `save.json` in `userdata/` |
| Online PVE | `NetworkPersistence` | Backend (Postgres) |
| Online PVP | `NetworkPersistence` | Backend + replay |

**Rule:** the offline binary never links the network implementation.
The online binary can fall back to `LocalPersistence` when disconnected.
(Steam account variants arrive in Phase 8 with `IAuthProvider`; there is no Steam in the repo today.)

## Replay — audit trail, not source of truth

Every run records its inputs. This is **not** a determinism mechanism —
it is an audit log.

| What | Why |
|---|---|
| Debug crashes | Player sends `replay.bin`, dev reproduces |
| Detect cheats | Server re-simulates and compares position |
| Balance analysis | "80% of players die at S5 to Dwarf level 4" |
| Watch your best run | Feature for the player |

**Size:** ~8 bytes per input × 30 Hz × 30 min = ~430 KB.
**Storage:** local disk for solo, object storage for online.
**Never transmitted in real-time.**

## What never persists

| Thing | Why |
|---|---|
| Generated terrain | Function of seed. Re-generate. |
| Destroyed tiles (across sessions) | Session-scoped by design (§ matrix). |
| Active enemies | Session-scoped. They respawn fresh. |
| Active particles | Cosmetic. |
| Camera position | Derived from player. |
| Selected inventory tab | UI state. Not data. |

## Locked resolutions (were open questions)

1. **Run has continuation** → `RunState` persists. Checkpoints are the save points. HP,
   inventory and destroyed-tiles-since-checkpoint survive close.
2. **Accounts arrive in Phase 8** (`IAuthProvider` + mock first). Solo offline stays
   account-less (`save.json`, no login).
3. **Portal carries the run, not the session** → the run's state moves with the player
   through the portal (inventory included); the session keeps nothing.
