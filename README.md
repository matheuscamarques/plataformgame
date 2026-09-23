# PlataformGame

> A 2D action-platformer with an infinite procedural underworld and per-part combat —
> built on a custom C++17 engine where **sprites are ASCII and audio is synthesized**.

[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)](#)
[![Backend](https://img.shields.io/badge/backend-SFML-orange.svg)](#)
[![Tests](https://img.shields.io/badge/tests-94%20passing-brightgreen.svg)](#)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey.svg)](#)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

**[Build](#quickstart)** · **[Controls](#controls)** · **[Architecture](#architecture)** · **[Contributing](#contributing)** · **[Roadmap](#roadmap)**

---

## Why this exists

Most 2D games look the same because they use the same pipeline.
This one barely has a pipeline.

Sprites are `const char*[]` + palette. Sounds are functions. Enemies, items,
blocks and skills are single files behind a macro. There is no editor, no SDK —
the engine is part of the game.

The bet: **architecture over tooling**. Adding content should be
`1 file + 1 macro`, never an edit to a central switch.

![melee](screenshots/0000_20260921_133754_melee_32.png)

| Cave / hurt | Manual / debug |
|---|---|
| ![hurt](screenshots/0000_20260921_211221_hurt_0.png) | ![manual](screenshots/0000_20260921_210155_manual.png) |

> Screenshots are auto-dumped by `ScreenshotSystem` (F12) into `screenshots/`.
> The two above are real golden captures (`hurt_*`, `manual_*` suites).

---

## What it is

- **Genre:** 2D action-platformer, single-player, run-based (checkpoint respawn)
- **World:** infinite, procedural, deterministic by seed
- **Depth:** 12,000 tiles across 11 vertical strata (`WORLD_BOTTOM=12000`, ~1h descent)
- **Combat:** directional melee + throwable TNT, per-part damage, posture, i-frames on hurt
- **Runtime:** custom C++17 engine, SFML as window/render/audio backend only
- **Assets:** generated at runtime — ASCII → texture, synth → `SoundBuffer`
  (repo ships only `arial.ttf` for HUD text + `screenshots/` goldens)

---

## Gameplay features

### Movement
- 4-dir move (WASD + arrows), jump with recharge, run modifier (`Space`/`LShift` combo)
- Water swim, lava no-clip as wall (damage phase is separate)
- Walk anim (4 frames), throw anim (0.4s), facing, full respawn reset

### Combat (K = melee, J = throw — not the other way around)
- **Melee on `K` (Heavy):** `Idle → Windup → Active → Recovery`, 3-hit combo, 1 hit per swing per enemy (`lastHitSwing`)
- **Throw on `J` (Light):** dynamite with 0.5s cooldown, fixed arc by facing, pool of 64
- **Per-part hitboxes:** `Head / Torso / ArmL / ArmR / LegL / LegR / Weapon` (`core/BodyPart.h`)
- **Per-part multipliers:** Head `2.0x` dmg / `1.5x` posture, arms `0.6x` / `0.5x`, legs `0.7x` / `0.8x` (`support/Combat/Body.cpp`)
- **Narrowphase:** biggest `damageMult` among touched parts wins (same rule for melee + explosion); inside AABB but outside parts = whiff
- **8-way aim** (`AimDir.h`) with frozen `swingAim` snapshot — hitbox doesn't follow input mid-swing
- **Hurt i-frames 0.6s**, HP 10000, knockback with `knockbackLock` (AI doesn't override)
- **Parry:** seam only (`parryWindowActive()`, window from 0.06s of Active) — no caller yet, reserved for a future rebate system

### Enemies (data, not code)
- **Slime (trash):** patrol (turn on wall + accumulated distance) + chase + hop + bite telegraph, spit skill (linear, 40–220px)
- **Dwarf (elite):** 7-state FSM (`Patrol / Alert / ThrowWindup / ThrowRelease / Recover / Melee / Retreat`), home 96px, aggro 200px, `Recover 0.6s` as punish window, no interrupt-on-hit
- **7 skills registered:** `slime_spit`, `dwarf_dynamite` (48–128px, fuse 0.8s), `dwarf_melee` (0–40px), `dwarf_smoke` (dash), `dwarf_barrel` (fuse 2.0s), `dwarf_dig` (teleport 128px + 3x3 break), `dwarf_collapse` (special, 6-tile line + 40 dmg). Base dwarf equips only `dynamite + melee` — the rest unlock via variants
- **Variants L1–L5 by depth** (S3→nv1 … S7+→nv5): hp/damage scaling + `smoke / barrel / dig+collapse`, aura + beard tiers (`VariantRegistry.h`)
- **Patience / betrayal:** 100pts, mining drains, gold nugget restores, 3 warnings with rising pitch + betray SFX, 4 stages up to passive
- **Barks:** text + fade, audio-ready (`dwarf_warn1-3`, `dwarf_betray`)

### Loot & progression
- **XP orbs** (pool 128, 12s lifetime) + **item orbs** (pool 128, 60s lifetime, 0.5s pickup delay), magnet 64px / collect 12px / 280px/s, gravity 500
- **Inventory:** 40 slots, stacking, atomic remove, deterministic `sort()`, gold counter
- **Equipment:** 4 slots (`RightHand / Head / Chest / Legs`), swap-on-equip, iron starter set, survives death
- **Hotbar:** 5 slots (`1–5`)
- **Dark-Souls-style UI, keyboard only, no mouse:** `E` grid, `Q/Tab` main tabs, `A/D` sub-tabs (`All / Materials / Consumables / Weapons / Armor / Keys`), `Home/End` first/last, `T` arrange, `U/Enter` use, `F` interact, `Esc` closes menu first
- **TNT:** fuse + radius 40px + `tilesRadius` 3, breaks tiles in circle + drops via `dropId`, glow while burning, writes `blockLight` into the grid
- **Run / strata:** `StratumManager` unlocks checkpoint on entry, `RunManager` respawns at deepest checkpoint (`R` voluntary restart, clears enemies/pools). No disk save yet — checkpoints live in-run

### World
- **8 surface biomes** (`Ocean / Beach / Desert / Savanna / Grassland / Forest / Taiga / Tundra` — `Mountain` is a mask, not a biome)
- **11 strata (PT names, the source of truth):** `Superficie → Cavernas Rasas → Bosque Fungico → Veios de Prata → Minas Antigas → Cristais → Saloes Derretidos → Vazio Primordial → Coracao Partido → Nucleo → O Fundo`
- **Terrain:** relief `freq 0.02 / amp 9`, ridged mountain mask `0.35`, peak mask `/512 0.75 +25`, cliff bands `0.80 x4` (wall-climb steps), trench `0.70 +20`, sea level 26 (~14–21% oceanic), coastal sand ±3
- **Caves:** blob `0.62−0.06` + mountain bonus, worm iso-line `|n−0.5|<0.02` (5–17% area, connectivity 0.77–1.0), floating islands (4–7 tiles, never over ocean), lakes paint carved air (`0.68`), lava pools only `ty≥6200`
- **Ores:** per absolute `ty` band, one vein identity per stratum (`Copper / Iron / Gold / Silver / Crystal / Platinum / Mithril / Adamant` generate; `Coal / Diamond` are legacy IDs that no longer generate)
- **116 tile IDs** (`Tile.h`, slot 7 intentionally empty — never reused), 5 base rocks + 5 flavors per stratum + 10 rares (2 gated: `VolcanicBomb`, `SolarFlare`)
- **Breakable world:** `breakTile` (Bedrock never) → `Air` + `modified` (pinned in LRU) + color-matched particles + drop; idle craters evict after 120s and regen from seed

### Lighting
- **Grids:** `skyLight` + `blockLight` 0–15 per tile, `lightDirty` per chunk
- **Sky:** top-down column fill + lateral flood −1/step, water −3 vertical, never through solid; cross-chunk seeding so borders don't seam
- **Block:** torch-free flood vs player-masked flood (raycast mask + BFS = hard shadow)
- **Raycast:** 96 rays, DDA step 0.5 tile, range 16 tiles, cross-chunk cast from world px
- **Lightmap:** 32×32 per chunk (`scale 2`), blur kernel 7 single-stage (profile D), `setSmooth(false)` — smoothness comes from values, not bilinear
- **Player light:** level 13 re-added on tile change / relight, radial glow default radius 90px, alpha `0.35 day → 0.75 night`
- **Day/night:** 10min cycle (600s), 12 anchors, boot 9h, sun/moon intensities + sky/tint colors, sun/moon/star arcs in world space
- **Post:** capture-based bloom (bright-pass + separable 9-tap blur + additive) + 128px vignette + trauma screen-shake (quadratic decay 1.5/s, 0.15 on melee hit, 0.3 on hurt, 0.8 on death)

---

## Tech highlights (for engineers)

### Layers
- **`core/` is pure** — no game logic, no upward deps. Enforced by `make test-layers` (`! grep support/ src/core/`)
- **`support/` is feature layer** — systems + registries + gameplay, always behind `support::World` facade and `GameContext` non-owning views
- **`SystemScheduler`** — `tick(dt, ctx)` ordered by `priority()` via `stable_sort`. Convention: `0–99` input/time, `100–199` AI, `200–299` physics, `300–399` combat, `400+` progression/spawn, `900+` cleanup
- **Determinism:** `-ffp-contract=off -fno-fast-math` (no FMA, stable `sin/cos/sqrt` — without it `noise_view/test_cave/test_ores` diverge 1 bit vs `-O0`). Generation is a pure function of `(tile, seed)`

### Extension pattern
Seven registries, one macro each. New content never touches a central file:

```cpp
// Enemy: data + behavior (Behaviors.cpp pattern)
REGISTER_ENEMY_ARCHETYPE("dwarf", []{
    support::EnemyArchetype a;
    a.hp = 60; a.behaviorKind = "dwarf";
    a.skills = {"dwarf_dynamite", "dwarf_melee"};
    return a;
}());

// Behavior (needs a .cpp + the macro above)
SUPPORT_REGISTER_BEHAVIOR("dwarf", DwarfAI);

// Skill
REGISTER_SKILL("dwarf_dynamite", []{
    support::SkillDef s;
    s.cooldown = 1.8f; s.telegraph = 0.30f;
    s.isRanged = true; s.minRange = 48.f; s.maxRange = 128.f;
    s.execute = [](support::Enemy& self, support::GameContext& ctx){ /* ... */ };
    return s;
}());

// Block / item follow the same shape:
// REGISTER_BLOCK(Tile::CrystalCluster, []{ ... });
// REGISTER_ITEM("iron_sword", []{ ... });
```

No `switch(kind)`. No `if (name == "archer")`. No central dispatcher.

### Performance
- **Tile batching:** 1 draw per chunk (`TileRenderer::rebuild` CPU-only + `upload` to 800×800 `RenderTexture`)
- **Culling:** `forEachEntityInRect` over visible set only (`SpatialHash`, cell 100px)
- **Pools everywhere:** particles, throws (64), XP/item orbs (128+128) — no alloc per frame
- **Deferred destruction:** mark during iteration, sweep at end (`destroyPending`, never erase in loop)
- **Chunks:** 16×16 (256B), streaming radius 2, LRU evicts clean-only, modified pinned + 120s idle evict, async `ChunkLoader` worker opt-in (adopt budget 2/frame)
- **Body from ASCII:** `rebuildFromSprite` sweeps sprite pixels per `BodyPartId` — hitboxes always match the art; missing part falls back to schema (locked by `Body.cpp:147-154` + `WeaponFallsBackWhenNoPixels`)

### Audio synthesis (zero `.wav`)
- **26 SFX** (`SoundBank.h`): jump/land/roll-reserved/hurt/death, swing/hit, throw/fuse/explosion (2-layer crack+rumble), tile-break/xp/checkpoint, slime bite/hurt/death, dwarf alert/throw/melee/hurt/death + warn1-3/betray, ui-select. 16 voices, quadratic attenuation
- **6 music tracks** (`MusicBank.h`): `surface` (C Major 90bpm) → `shallowCaves` → `fungalWoods` → `oldMines` → `moltenHalls` → `core` (F Major 130bpm). 4 bars × 16 steps, bass/mel/harm/perc, crossfade + switch by stratum. 5 tracks (`SilverVeins, Crystals, PrimordialVoid, BrokenHeart, TheBottom`) planned after playtest approval
- **Note:** `player_roll` SFX is registered but has no call site yet — roll mechanic doesn't exist. Same for splash/bubble (water is silent by design for now)

---

## Architecture

```
src/
├── core/            Kernel — no game logic (test-layers gated)
│   ├── Cooldown, Pool, Time, Random, Math, Log
│   ├── Config (kBlockSize=50, kWorldSeed=1337), EntityKind, BodyPart, Material
│   ├── Noise, DayNightCycle, Celestial, Bloom, RadialTexture
│   ├── Item, ItemDef, Inventory (40), Equipment (4), EquipSlot, DropTable
│   └── Synth, AudioSystem (16ch), Music, sprite_from_ascii, System
│
├── world/           Procedural world
│   ├── Tile (116 IDs), Block, BlockRegistry, Blocks/*.cpp
│   ├── Stratum (11), Generation, World, Chunk (16x16), ChunkManager, ChunkLoader
│   └── LightPropagator (scale 2 / kernel 7), RaycastLight (96 rays), TileRenderer
│
├── support/         Gameplay systems
│   ├── Combat/    Body, BodySchema(+Registry), BodySystem, Melee (300),
│   │              ContactDamage, Explosion (320), Death
│   ├── Enemies/   Behavior(+Registry), Archetype, Factory, EnemySystem,
│   │              SlimeAI, DwarfAI, SpawnSystem, VariantRegistry, Barks, Patience
│   ├── Skills/    Skill(+Registry), SkillSystem, UtilityAI
│   ├── Effects/   ThrowSystem (pool 64), Throwable, ParticleSystem (512+4096)
│   ├── Progression/ DropSystem (400), RunManager, StratumManager (60)
│   ├── Lighting/  LightingSystem (sky/tint/radial glow)
│   ├── Camera/    follow + deadzone + trauma shake
│   ├── Input/     InputMap (only place touching sf::Keyboard)
│   ├── UI/        HotbarUI (5), InventoryUI (DS-style), ItemIcon
│   └── Debug/     Overlay (F1/F2/F4/F5/F6/F7), Feed, BodyDump, Screenshots (F12)
│
├── entities/        Entity, Player (J=throw, K=melee)
├── game/            Game, App (fixed-step 1/30), Renderer, Bootstrapper,
│                    SoundBank (26), MusicBank (6)
├── assets/          ASCII sprites, PlayerSprite, EquipmentLayout,
│                    SpriteFrameRegistry
└── tests/           94 headless binaries (no framework, raw assert(), no GL)

tools/               noise_view (12 layers → build/noise/), light_view (→ build/light/)
docs/                DOCTRINE.md, MELEE_PER_PART_PLAN.md, PARALLEL_WORK.md
uml/                 StarUML project (v0.1, outdated)
```

### System priority (excerpt)

| Priority | System | Responsibility |
|---|---|---|
| 60 | StratumManager | Stratum boundary / checkpoint unlock |
| 150 | EnemySystem | AI tick, physics, behavior dispatch |
| 250 | BodySystem | Rebuild part hitboxes after movement |
| 300 | MeleeSystem | `K` attack resolution, 1 hit/swing |
| 320 | ExplosionSystem | Damage + tile destruction in circle |
| 350 | ParticleSystem | Debris + dust pools |
| 400 | DropSystem | XP / item orbs, magnet + collect |

Full parallel-work contract: [`docs/PARALLEL_WORK.md`](docs/PARALLEL_WORK.md).
Melee per-part roadmap: [`docs/MELEE_PER_PART_PLAN.md`](docs/MELEE_PER_PART_PLAN.md).

---

## Quickstart

### Dependencies
- C++17 (`g++`), SFML (`graphics/window/system/audio`), `pthread`
- Optional: `ccache` (30–50% faster rebuilds when present)

### Linux / macOS
```bash
git clone https://github.com/matheuscamarques/plataformgame
cd plataformgame
make -j$(nproc)
./build/plataformer
```

### Tests
```bash
make test        # fast loop (excludes *_stress, ~30s)
make test-all    # full suite incl. slime_stress (~1min)
make test-layers # gate: core/ must never include support/
```

Every test is a standalone headless binary (`tests/test_*.cpp` → `build/tests/*`).
No framework — `int main()` + `assert()`. Sprites never `build()` in tests (no GL).

### Tools
```bash
make noise-view  # 12 noise layers → PNGs (seed x0 x1 y0 y1 layer out.png)
make light-view  # CPU golden lightmaps → build/light/ (surface + cave + glow + tints)
```

---

## Controls

| Key | Action |
|---|---|
| **WASD / Arrows** | Move (8-way aim source) |
| **Space** | Jump |
| **LShift** | Roll bind (reserved — **no mechanic yet**, SFX only) |
| **J** | Throw dynamite |
| **K** | Melee (3-hit combo) |
| **E** | Inventory / Equipment grid |
| **Q / Tab** | Main tab prev / next |
| **A / D** | Sub-tab prev / next |
| **Home / End** | First / last slot |
| **T** | Arrange all |
| **U / Enter** | Use selected |
| **F** | Interact / action menu |
| **1–5** | Hotbar slot |
| **R** | Respawn at checkpoint (consumed while menu open) |
| **Esc** | Close menu / pause |
| **F1** | Debug overlay master |
| **F2 / F4 / F5 / F6 / F7** | Hitboxes / events / AI / world+hash / light mask |
| **F3** | Char view (no texture) |
| **F9** | File log (`logs/debug.log`) |
| **F10 / F11** | Auto-hurt / auto-melee (test helpers) |
| **F12** | Screenshot to `screenshots/` |

---

## What makes it different

**No asset pipeline.**

```bash
$ ls *.png *.wav *.ogg 2>/dev/null; echo "—"
—                                   # (only arial.ttf + screenshots/ goldens ship)
$ grep -rn "loadFromFile.*png\|loadFromFile.*wav" src/ | wc -l
0
```

A player frame is ASCII:

```cpp
// src/assets/Sprites/PlayerSprites.h (excerpt pattern)
inline const char* const kPlayerIdle[] = {
    "....KKKK....",
    "..KKKKKKKK..",
    // ... 1 char = 1 BodyPartId via palette
};
```

A jump sound is a function call:

```cpp
// src/game/SoundBank.h (excerpt pattern)
a.registerSound("player_jump",
    tone({{300.f, 0.f, 0.05f, 1.f}, {400.f, 0.02f, 0.05f, 0.5f}},
         0.07f, Wave::Triangle, {0.003f,0.02f,0.4f,0.03f}, 0.22f));
```

Trade-off is real: complex sprites are tedious and iteration is a recompile.
Payoff: `git diff` shows pixel changes, tests are deterministic, and adding
content never edits a central file.

---

## Roadmap

### Now
- **Per-part melee narrowphase polish** — whiff vs AABB-fallback, `postureMult` tuning (`docs/MELEE_PER_PART_PLAN.md` + 15min playtest gate)
- **Torch item** — first static grid light source (today: player + TNT + blasts only)
- **Remaining 5 music tracks** — after the current 6 pass playtest

### Next
- **Save system** — disk persistence (today in-memory run only)
- **Smoke visual** for `dwarf_smoke` (dash exists, cloud doesn't)
- **Swim SFX** (water is silent today to avoid 30×/s retrigger)

### Later
- **Crafting** — depends on ore economy design
- **Procedural quests / objectives** — run-scoped
- **Dismemberment** — part removal on heavy hits (`breakable` flag already in `PartDef`)
- **Roll mechanic** — bind + SFX exist, movement/i-frames don't

### Out of scope
- Multiplayer
- Mobile (needs renderer abstraction)
- External asset pipeline

---

## Contributing

Small, self-contained features. One invariant = one `tests/test_*.cpp`.

### Before you open a PR
1. Read [`docs/PARALLEL_WORK.md`](docs/PARALLEL_WORK.md) — the `1 file + 1 macro` contract.
2. Run `make -j8 test` (and `make test-all` 1×/day, `make test-layers` if you touched `core/`).
3. New behavior needs a headless test. No GL in tests (`sprites::build()` never runs there).

### Good first issues
- New ore flavor for an existing stratum (`src/world/Blocks/` + 1 test)
- New dwarf bark line (`Barks.h` + SFX reuse)
- New dwarf skill wiring (copy `dwarf_barrel`, new cooldown/telegraph)
- New item (potion/key/consumable in `ItemDefs` + drop-table entry)
- New enemy variant tier (extend `registerDefaultVariants()`)

### Not open yet
- Renderer refactors (backend abstraction pending)
- Save format (design pending)
- Multiplayer

---

## Credits

- **Author:** Matheus Camarques — [@matheuscamarques](https://github.com/matheuscamarques)
- **Backend:** [SFML](https://www.sfml-dev.org/) — window, input, GL context, audio voice
- **Font:** `arial.ttf` (HUD text only)
- **Inspirations:** Terraria · Dark Souls · Vampire Survivors · Hades · Dead Cells · Hollow Knight

---

## License

MIT — see [`LICENSE`](LICENSE).

---

<p align="center">
  <em>"Don't build a game. Build a world that builds games."</em>
</p>
