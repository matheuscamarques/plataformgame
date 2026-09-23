# Multiplayer — visão e arquitetura

> **Status: design doc (P0). Nada aqui está implementado.**
> O que existe hoje: `multiplayer/online_game_engine/` (scaffold `mix phx.new`, Phoenix + sqlite,
> `c_src/` vazio) e `multiplayer/client/` (vazio). Este documento trava as decisões para que o
> protótipo P1 comece sem retrabalho.

<!-- @file: docs/MULTIPLAYER.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: Design do multiplayer (backend Elixir+NIF, client thin) com sharding por seed via portais. -->
<!-- @details: Define netcode Modelo 4, sessões co-op 1-4, fronteira do NIF, drivers IWorld, persistência por fases, protocolo e roadmap P1-P5. -->

## 1. Visão em 1 parágrafo

O backend Elixir é **autoritativo** e simula o jogo dentro de NIFs que contêm **só cálculos**
(física, colisão, IA, geração — sem I/O, sem estado). O client C++/SFML **só desenha** o que o
backend computou: envia input, recebe snapshots, interpola e renderiza. O mundo é particionado
**por seed**: cada seed é uma sessão isolada (1 GenServer) e o **portal é o sharding** — atravessar
um portal é uma mensagem entre dois GenServers, transportando só o player. Terreno nunca trafega
na rede: é função pura de `(seed, tile)`, então o client gera local e aplica só deltas de destruição.

## 2. Decisões travadas

| # | Decisão | Valor | Motivo |
|---|---|---|---|
| 1 | Netcode | **Modelo 4**: mundo determinístico + entidades autoritativas + deltas (§3) | Não sincroniza o que é derivável; lockstep cross-platform é inferno |
| 2 | Players por sessão | **1–4, co-op PvE**; solo = sessão de 1, mesmo código | Preserva o single-player como caso base (`n=1`) |
| 3 | Tick | Server **30Hz** (igual ao fixed-step atual `1/30` em `src/game/App.cpp:85`), snapshot **10Hz**, client render 60Hz | 10Hz + interpolação é indistinguível de 30Hz e 3× mais barato |
| 4 | Client | **Um codebase, dois drivers** (`IWorld`: `LocalWorld` vs `NetworkWorld`), dois targets `game` / `game-net` (§6) | Sem fork, sem drift; `make game` nunca exige backend |
| 5 | NIF | **Função pura** `(input, state_slice) → output`, sem estado, sem I/O; >1ms vai pra dirty scheduler | NIF bloqueante trava o scheduler BEAM inteiro |
| 6 | Sessão | **Efêmera em memória**; morre vazia, sem persistência de sala | É o que faz o sharding escalar (sem coordinator global) |
| 7 | Meta/persistência MVP | **`save.json` local** no client; zero Ecto no MVP (§7) | Não há queries nem contas compartilhadas ainda |
| 8 | Replay | **Gravar sempre** (seed + inputs, ~430KB/30min), ler quando útil | Debug, anti-cheat por re-simulação e balanceamento de graça |
| 9 | Transporte MVP | **WebSocket (Phoenix Channel)**; UDP só se a latência doer | TCP resolve ~80% dos casos |
| 10 | Terreno na rede | **Nunca** (só seed 1× no join + deltas de tile por evento) | GBs economizados por construção |

## 3. Modelo de netcode (Modelo 4)

Nem snapshots-cegos (sincronizam terreno derivável), nem lockstep (exige 2 físicas bit-idênticas
e trava no client mais lento). O servidor é autoritativo **só sobre o que muda**: entidades e
tiles destruídos.

### O que trafega

| Dado | Direção | Frequência | Tamanho (estimado) |
|---|---|---|---|
| Seed do mundo | Server → Client | 1× no join | 4 bytes |
| Input do player (com nº de sequência) | Client → Server | 30Hz | ~8 bytes |
| Snapshot de entidades (delta) | Server → Client | 10Hz | ~N×20 bytes |
| Tile deltas (quebrados desde o snapshot) | Server → Client | por evento | ~5 bytes cada |
| Confirmação de input (ack seq) | Server → Client | 10Hz | 4 bytes |
| Eventos (morte, explosão, som) | Server → Client | por evento | ~10 bytes |

Custo por client ≈ **5 KB/s**. 100k players ≈ 500 MB/s agregados em 50–200 nós BEAM.

### Fluxo

```
Client                          Server (BEAM + NIF)
  │                                    │
  │ ── join(seed) ────────────────────▶│ valida, cria/recupera GenServer da seed
  │ ◀─ welcome(seed, tick, entities) ──│
  │   [gera terreno local pela seed]   │
  │ ── input(seq=1..n, 30Hz) ─────────▶│
  │                                    │ simula ticks (NIF puro), 30Hz
  │ ◀─ snapshot(tick, ack_seq, ────────│
  │     entities, tile_deltas) 10Hz    │
  │   [prediz player local + reconcilia]│
  │   [interpola demais (~100ms buffer)]│
  │   [aplica tile deltas + renderiza] │
```

Client **prediz o próprio player** (aplica input na hora, reconcilia com tolerância de ~4px) e
**interpola os outros**. Terreno nunca sincroniza.

## 4. Sharding por seed (o portal é a arquitetura)

Cada seed = **1 GenServer** = 1 unidade de simulação (10–100 players no limite teórico, **4 no MVP**).
Mundo = grafo de seeds conectadas por portais. Roteamento: `hash(seed) → nó` (ex.: `:erlang.phash2`
+ `:pg`/Registry). Portal = `cast` entre GenServers serializando só o player (pos, hp, inventário,
build); **respeita o teto da sessão destino** (cheia = portal não abre, backend decide, não o client).

- **Solo e co-op são o mesmo caso**: sessão com N ∈ [1, 4]; `@max_players = 4`.
- **HP scaling**: `1 + (n−1) × 0.6` (1 player = 1.0×, 4 players = 2.8×) — o single-player segue intacto.
- **Refino do portal** (estilo Nether, mas descobrível): cálculo revelado em pedaços por artefatos
  no mundo (arqueologia multi-passo, não puzzle de 1 passo); nem todo portal leva a player (seeds
  virgens geram na hora); travessia **custa** (stamina + cooldown ~5min) para regular churn de estado.

## 5. NIF — só cálculos

### Entra no NIF (hot path, determinístico, sem I/O)

Física do player (AABB, gravidade, colisão com tiles), colisão per-part, raycast/LOS, geração de
chunk (noise puro), scoring de UtilityAI. Mesmas unidades do jogo atual (`src/core/`, `src/world/`,
partes puras de `src/support/`), compiladas como NIF — reaproveita o determinismo já travado
(`-ffp-contract=off -fno-fast-math`, geração pura de `(tile, seed)`).

> Linguagem travada: **C++ (`erl_nif` via `elixir_make`, em
> `multiplayer/online_game_engine/c_src/`)**, não Rust — ver BACKEND.md. O NIF reaproveita o
> sim C++ existente em vez de reescrevê-lo.

### Não entra no NIF

Network I/O, estado do mundo, coordenação entre seeds, persistência, loop de simulação.
**O BEAM é o loop** (`Process.send_after(self(), :tick, 33)`); **o NIF é a calculadora**.
Estado mora no GenServer da seed; NIF recebe slice, devolve output.

> Atenção honesta: NIF ≠ bit-identidade grátis com o client. As 3 implementações da física
> (Local, predição do client, NIF) precisam ser **logicamente idênticas**, não bit-idênticas —
> a tolerância de ~4px no `reconcile()` absorve drift.

## 6. Client — um codebase, dois drivers

`Renderer`, `Game`, UI, sprites e áudio **não mudam**: passam a ler de `IWorld` (views read-only).

```
src/
├── world_io/      NOVO: IWorld (interface) + LocalWorld (wrapper do World atual) + NetworkWorld
├── physics/       NOVO: PlayerPhysics — função pura extraída de Player::tick (src/entities/Player/Player.cpp:160)
└── network/       NOVO (só no target game-net): Socket, SnapshotBuffer, Reconcile, Protocol
```

- **Builds**: `game` compila **sem** `network/` e sem toolchain Elixir/C++-NIF; `game-net` define
  `WITH_NETWORK`. `make game` sempre funciona, mesmo com o backend quebrado.
- **Runtime**: `--online --host ...` seleciona `NetworkWorld`; falha ao conectar → **fallback
  automático para solo**. Dev = Online contra backend em localhost (não é um modo à parte).
- **Regras**: offline nunca inclui header de `network/`; network nunca bypassa a interface;
  estado não compartilhado entre drivers (views read-only); backend nunca dita refactor no offline
  (ex.: `PlayerSnapshot` separado em vez de forçar `Player` a ser copiável).

## 7. Persistência por fases

Estado atual (corrigido): **zero persistência em disco** — `StratumManager`/`RunManager`/`Inventory`
vivem em memória. Não existe `MetaState` ainda; meta-progressão nasce na Fase 2.

| Fase | O que persiste | Onde |
|---|---|---|
| **1 (MVP)** | Meta (`deepest`, `deaths`, runs) em `save.json` local (~200B); sessão e tiles em memória; inventário **não** persiste (roguelike) | Arquivo no client, ~30 linhas |
| **2 (online)** | Contas + meta-progressão centralizada (mesmo formato do `save.json` via POST); replay de inputs sempre gravado | Postgres via Ecto (já no scaffold) |
| **3 (se persistent world)** | Contas + seed graph + reputação (tabelas em BACKEND.md) | Postgres; posição online segue em memória |

**Nunca persiste**: chunks gerados (função da seed), estado de inimigos, build/XP da run, assets.
`IPersistence` com `LocalPersistence` (JSON) e `NetworkPersistence` (API) mantém `make game`
independente do backend.

## 8. Protocolo (MVP, WebSocket/Phoenix Channel)

Tópicos: `seed:<id>` (1 channel por sessão). Mensagens client→server: `join`, `input{seq, bits}`.
Server→client: `welcome{seed, tick, entities}`, `snapshot{tick, ack_seq, entities[], tile_deltas[]}`,
`event{kind, payload}`. Serialização: JSON no protótipo, binário compacto (ex.: MsgPack) quando a
banda doer. Reconnect: re-`join` recebe `welcome` fresco (terreno regenerado local + snapshot atual —
sem re-simulação, vantagem do Modelo 4 sobre lockstep).

## 9. Roadmap do protótipo (2 semanas)

| Fase | Escopo | Critério de saída |
|---|---|---|
| **P1** (3d) | 1 nó, 1 GenServer/seed, loop 30Hz, NIF de física, 1 client move e se vê | Input→snapshot redondo |
| **P2** (2d) | 2º client na mesma seed, interpolação remota | 2 players se veem com 100–300ms de lag artificial |
| **P3** (3d) | 2ª seed + portal (serializa player, transfere, client regenera mundo) | Travessia preserva hp/inventário |
| **P4** (2d) | Tile deltas entre clients | Buraco cavado por A aparece em B |
| **P5** (2d) | 100 clients headless em 1 seed | CPU do NIF + banda dentro do orçamento (§3) |

**Só escala para N seeds depois de P5 verde em 1 nó.** `PlayerPhysics` (§6) pode começar antes de
tudo: não exige backend nem rede e testa a disciplina de separação com 1 arquivo.

## 10. Anti-patterns

Lockstep BEAM↔client bit-idêntico; sincronizar terreno; NIF com estado global ou >1ms fora de
dirty scheduler; 1 GenServer para N seeds; snapshot a 60Hz; `#ifdef WITH_NETWORK` espalhado;
`if (online)` dentro de `Game::tick`; segundo repositório/client com source separada; persistir
chunks; usar replay como sincronização em tempo real.

## 11. Perguntas — resolvidas e em aberto

Travadas em [PERSISTENCE.md](PERSISTENCE.md) e [PORTALS.md](PORTALS.md):

1. Run tem continuação via `RunState` (checkpoints são save points; HP/inventário/deltas sobrevivem).
2. Solo é anônimo (`save.json`); contas nascem na Fase 8.
3. Portal transporta a run (inventário incluído); a sessão não guarda nada.
4. Grafo de seeds persistente nasce na Fase 9 (tabelas `seeds`/`edges`).

Em aberto: transporte final — WebSocket basta ou o roadmap prevê UDP para input?
Responder antes de P2.
