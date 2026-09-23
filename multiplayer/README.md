# multiplayer/

Projetos futuros de online do plataformgame. **Nada aqui roda o jogo ainda.**

| Pasta | O quê | Status |
|---|---|---|
| `online_game_engine/` | Backend Elixir autoritativo (Phoenix + NIFs de cálculo em `c_src/`) | Scaffold `mix phx.new` (sqlite); `c_src/` vazio |
| `client/` | Thin client (input → socket, snapshot → render) | Vazio; direção: reusar o SFML C++ via drivers `IWorld` |

**Design canônico: [`../docs/MULTIPLAYER.md`](../docs/MULTIPLAYER.md)** — netcode Modelo 4
(mundo determinístico + entidades autoritativas + deltas), sharding por seed via portais,
sessões co-op 1–4, NIF só-cálculos, persistência por fases, protocolo e roadmap P1–P5.

## Rodar o scaffold (backend)

Pré-requisitos: Elixir ≥ 1.15 (testado com 1.20 + OTP 29 via asdf).

```bash
cd multiplayer/online_game_engine
mix setup
mix phx.server  # http://localhost:4000
```

## Convenções

- `deps/`, `_build/`, `priv/static/` são gerados — não entram em commit de código
  (quando `multiplayer/` for versionado, espelhar o `.gitignore` do scaffold).
- NIFs moram em `online_game_engine/c_src/` e **só contêm cálculos puros**
  (sem I/O, sem estado — ver §5 do design doc).
- O jogo offline nunca depende disto: `make game` na raiz sempre funciona sem backend.
