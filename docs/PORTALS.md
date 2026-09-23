<!-- @file: docs/PORTALS.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: Portais como sharding — fórmula, artefatos, teleporte e grafo de seeds. -->
<!-- @details: Separação criação/teleporte/matchmaking, fórmula de 5 ops com 6 artefatos, grimório, fluxo de teleporte e edge cases. -->

# Portals — discovery, formula, routing

Portals are not a feature. They are the sharding architecture (§4 of MULTIPLAYER.md):
traversing a portal is a message between two session GenServers, carrying only the player.

## The three concepts, separated

| Concept | What it is | Where it runs |
|---|---|---|
| **Portal creation** | Knowing that `(x,y) → seed N` | Client + backend query |
| **Teleport** | Moving the player from seed A to seed B | Backend transfer |
| **Session find** | Finding players on seed B | Backend matchmaking |

Teleporting does **not** imply finding players. A player can teleport to
an empty seed. That is valid and expected.

**No coordinate is invalid.** Every `(x, y)` maps to a valid seed.
What varies is whether anyone is there.

## Formula — 5 operations + mask

The formula is public, but **discovered in fragments**. Six artifacts, one
per operation. Without all six, the player computes the wrong seed.

```c
// c_src/portal_seed.* 🆕 (C, shared with the client predictor)
#include <stdint.h>
uint32_t seed_from_coords(int32_t x, int32_t y) {
    uint32_t s = (uint32_t)(x + y);
    uint32_t p = (uint32_t)(x * y);
    uint32_t m = (s * 73856093u) ^ (p * 19349663u);
    uint32_t d = (m << 13) | (m >> 19);
    uint32_t a = d;
    a ^= a >> 16; a *= 2246822519u;
    a ^= a >> 13; a *= 3266489917u;
    a ^= a >> 16;
    return a & 0x7FFFFFFFu;
}
```

**Why this formula:**
- Deterministic: pure arithmetic
- Well distributed: xorshift avalanche
- Non-trivial: 5 operations, not obvious without all artifacts
- Decomposable: each operation is one line
- Total coverage: `& 0x7FFFFFFF` yields 2^31 valid seeds
- No secret: no private key, it is archaeology, not cryptography

**Do not use cryptographic hashing.** SHA/MD5 is overkill and the player
cannot compute it by hand. Basic arithmetic is enough and works in a
spreadsheet, on paper, or in code.

## Artifacts — where they appear

| Artifact | Reveals | Where it appears |
|---|---|---|
| Sum Tablet | `S = x + y` | Surface ruins |
| Product Seal | `P = x * y` | Shallow caves |
| Mixing Sigil | `73856093`, `19349663`, XOR | Old Mines |
| Fold Rune | `<< 13`, `>> 19` | Crystals |
| Avalanche Glyph | shift-multiply-shift pattern | Broken Heart |
| Limit Veil | `& 0x7FFFFFFF` | Core |

(Stratum names map to `Stratum.h`: Superficie → Cavernas Rasas → Minas Antigas →
Cristais → Coracao Partido → Nucleo.)

**Without one artifact:** the player computes a different seed. They teleport.
They arrive in an empty world. They learn something is missing.

**With all six:** exact seed. They arrive where intended.

## The grimoire (UI, proposed)

The grimoire is UI, not an item. It shows what the player knows.

```
Portal Grimoire:

  ✓ Sum and Product         → known
  ✓ Mixing Sigil            → known
  ✗ Fold and Shift          → unknown
  ✗ Avalanche               → unknown
  ✗ Limit Veil              → unknown

  Formula incomplete. Portal will lead to an unpredictable destination.
```

**Rule:** the client always allows teleporting, even with an incomplete
formula. It only warns that the destination is unpredictable.

## Coordinates — where the player sees them

Portals are neutral. **Coordinates live on separate artifacts.**

- A scroll contains `(50, 200)`.
- A portal accepts any known coordinate.
- Portals are N:M — N portals, M coordinates.

**Why:** discovery becomes social. "I found coordinates (50, 200) in a ruin."
Portals become reusable stations.

## Teleport flow

```
1. Player stands at a portal, has at least one known coordinate.
2. Opens portal UI, selects coordinate.
3. Client computes seed = seed_from_coords(x, y).
4. Client sends: TELEPORT_REQUEST(seed, coordinate).
5. Server:
   a. Validates the seed exists (or creates it).
   b. Transfers the player's run state (PERSISTENCE.md § Locked resolutions).
   c. Returns: SESSION_INFO(seed, existing_players).
6. Client regenerates the world from the seed.
7. Player spawns at a default position (or the closest checkpoint).
```

**Note:** the client always regenerates the world locally from the seed.
The server does not send terrain. It only confirms the session.

## Seed graph — global and persistent (Phase 9+)

Seeds are **universal**. The same `(x, y)` produces the same seed for every
player. This is a feature, not a bug — it enables coordinates as social
currency.

```
Postgres tables:

seeds:
  id            BIGSERIAL
  seed_hash     INT         -- from formula
  owner_account BIGINT      -- who claimed it first
  created_at    TIMESTAMPTZ
  last_seen_at  TIMESTAMPTZ

edges:
  from_seed_hash INT
  to_seed_hash   INT
  discovered_by  BIGINT
  discovered_at  TIMESTAMPTZ
```

**Ownership is claimed on first discovery.** If player A visits seed 42,
they own it. If player B visits later, they see "owned by A" in the
portal metadata (optional feature).

## Edge cases

| Case | Behavior |
|---|---|
| Valid coord, complete formula, seed has session | Enter normally (if not full — cap 4, invader counts) |
| Valid coord, complete formula, empty seed | Create session, enter alone |
| Valid coord, **incomplete** formula | Arrive at wrong seed. Learn |
| Invalid coord (out of range) | Client blocks. Not a seed |
| Portal without energy | Blocks. Recharge with item (🆕 proposed mechanic — no energy system exists today) |
| Same coordinate twice | Same seed. Idempotent |
| Two coords → same seed | Collision. Acceptable (31-bit space, player thinks in coords, never sees it) |

## Host offline, still visitable (corrected)

If the owner of seed 42 is offline, a visitor can still enter seed 42: the world
regenerates from the seed, the host is absent. Solo exploration, farming explored zones,
foreshadowing PVP.

> Locked correction: **destroyed tiles do NOT persist across sessions.** A fresh session
> starts from the pure seed; holes dug in a previous session are gone. Session memory dies
> with the session (PERSISTENCE.md matrix). No backend delta log — that would need a global
> coordinator and break sharding.
