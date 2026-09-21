# Trabalho em paralelo (2+ devs / agentes)

Fronteiras pós-split: cada área tem dono e arquivos exclusivos.
Conflito só acontece na fronteira — combine antes de cruzar.

## Mapa de áreas

| Área | Arquivos | Dono típico | Verificação |
|---|---|---|---|
| Config core | `src/core/Config.h`, `src/core/EntityKind.h`, `src/defines.h` | — (congelar) | `make -j8 test` |
| Sprites player | `src/assets/Sprites/PlayerSprites.h` | dev A | `make build/tests/test_sprites_split build/tests/test_sprites && ./build/tests/test_sprites_split && ./build/tests/test_sprites` |
| Sprites inimigos | `src/assets/Sprites/EnemySprites.h` | dev B | `make build/tests/test_sprites && ./build/tests/test_sprites` |
| Sprites equip | `src/assets/Sprites/EquipSprites.h` | dev A/B | `make build/tests/test_material build/tests/test_equipment && ./build/tests/test_material && ./build/tests/test_equipment` |
| Agregado | `src/assets/Sprites/SpriteSet.h`, `src/assets/PlayerSprite.h`, `src/assets/SpriteFrameRegistry.*` | — (acordo) | `make -j8 test` |
| App | `src/game/App.cpp` (ctor/start/run/tick) | dev A | `make -j8 test` |
| Renderer | `src/game/Renderer.cpp` | dev B | `make -j8 test` + run visual |
| Input | `src/game/Input.cpp`, `src/support/Input/*` | dev A/B | `make -j8 test` |
| Bootstrapper | `src/game/Bootstrapper.cpp` | — (estável) | `make -j8 test` |
| Inimigos | `src/support/Enemies/*` | dev B | `make -j8 test` |

## Regras

1. `make -j8 test` (fast) antes de cada push. Verde = pode subir.
2. `make test-all` (inclui `*_stress`) 1x/dia ou antes de merge na main.
3. `make test-layers` se tocar em `src/core/` (core nunca inclui support).
4. Mudança em header agregado (`SpriteSet.h`, `game.h`, `EnemyArchetype.h`)
   avisa no chat antes — todo mundo recompila contra ele.
5. Teste novo segue o padrão `tests/test_*.cpp` (Makefile descobre sozinho).
6. Sem GL em teste: `sprites::build()` nunca roda headless (só no `run()`).
