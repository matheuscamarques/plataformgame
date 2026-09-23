<!-- @file: docs/PVP.md | @author: Matheus de Camargo Marques <matheuscamarques@gmail.com> -->
<!-- @brief: PVP por invasão — papéis Ally/Hostile/Traitor, reputação e consentimento. -->
<!-- @details: Ambiguidade do host, recompensas assimétricas, reputação global persistente, matchmaking com consentimento e edge cases. -->

# PVP — invasion, roles, reputation

All proposed (`🆕`, Phase 11). Nothing here exists in the repo. Constraint inherited from
MULTIPLAYER.md: the invader occupies a session slot — **cap 4 includes the invader**; a full
session blocks the portal (backend decides).

## The pillar

**The invader is a free agent. The host does not know what to expect.
The choice is irreversible. The consequence is permanent.**

## Three roles

| Role | Chosen where | What they can do | Consequence |
|---|---|---|---|
| **Ally** | Portal, option 1 | Help in combat, heal host, mark ores, block enemies | Attacking the host turns them into a **Traitor** |
| **Hostile** | Portal, option 2 | Attack host, attack host's enemies (chaos) | Helping the host gives no Ally reward |
| **Traitor** | Ally who attacked | Same as Hostile | Double penalty. Cannot return to Ally |

**Traitor is not a choice.** It is a consequence. The player picked
Ally, betrayed, became a Traitor. There is no direct option.

## The host sees a stranger, not a role

| Aspect | Visible? |
|---|---|
| Name | Yes |
| Reputation badge (green / red) | Yes, if `> 20` or `< -20` |
| Declared intention | **No** |
| Equipment | Partial (weapon visible, neutral color) |
| Aura | Neutral (no red/blue) |

The host discovers by action:
- Ally heals the host → "he is Ally"
- Hostile attacks immediately → "he is Hostile"
- Ally stands still for 5 seconds → pure tension

**This is the core.** Without ambiguity, there is no drama.

## Reward structures

**The classic mistake is making one role worth more.** The fix is
**qualitatively different rewards**.

### Ally — shared progress

| Event | Reward |
|---|---|
| Host kills a boss with you alive | +1 rare item, generous XP |
| You save the host from death | + "guardian" bonus |
| Host finishes a stratum | + covenant progress |

**Ally earns more the further the host goes.** Interest is aligned.

### Hostile — immediate loot

| Event | Reward |
|---|---|
| Host dies | Steal a % of host inventory |
| You destroy 50+ tiles | + "chaos" bonus |
| You survive 5 min without dying | + "duel" bonus |

**Hostile earns faster but without investment.** If they die before the
host, they lose everything.

### Traitor — reward and punishment

| Event | Reward | Punishment |
|---|---|---|
| Kill host | Loot as Hostile | −2 reputation (permanent) |
| Kill host **after** 3 min as Ally | Loot + "backstab" bonus | −3 reputation (permanent) |

**Betrayal pays once.** The second host does not trust. Low reputation
means no one accepts you as Ally.

## Reputation — global and persistent (Postgres, Phase 11)

```
Reputation scale:

  -100 = "Marked Traitor"    (hosts see warning before opening portal)
     0 = "Unknown"
    50 = "Trusted Ally"      (hosts see green badge)
   100 = "Portal Legend"     (high covenant rank)
```

**How it moves:**

| Action | Delta |
|---|---|
| Help an Ally session to its end | +2 |
| Kill a Hostile | +1 |
| Betray (Ally attacks host) | −5 (permanent) |
| Hostile fails and dies | +0 (host gets XP bonus) |

**Impact:** betrayal is expensive. If you want to be Hostile, be Hostile
from the portal. Do not pretend.

## Matchmaking and consent

| Rule | Consequence |
|---|---|
| Portal opens only if host is online | No invader in empty worlds |
| Portal opens only if host is **not** in combat | Fairness |
| Portal opens only if session has a free slot | Cap 4 includes invader |
| Host can **refuse** the portal | Once per hour. Then must accept |
| Host can **expel** an invader | 30s cooldown. Costs one rare item |

**Refuse and expel are vital.** Without them, the host is a victim of
griefing. With them, the game is fair.

## What each path does

### Ally enters

```
1. Session receives: JOIN(player_id, role=Ally)
2. Host sees: "An invader entered" (no color)
3. Invader spawns: 200px behind host, 2s invulnerability

If host attacks the invader first:
  - Invader may defend without reputation loss
  - Server marks "host aggressor"

If invader attacks host first:
  - Reputation −= 5 (permanent)
  - Role becomes Traitor
  - Optional: session enemies ignore Traitor
```

### Hostile enters

```
1. Session receives: JOIN(player_id, role=Hostile)
2. Host sees: "An invader entered" (no color)
3. Invader spawns: 400px away (far from host)
4. Invader has a 3s telegraph: red pulse (0.5s)
   This is mandatory. PVP without warning is unfair.
```

### Betrayal during Ally

```
1. Ally attacks host after 2 minutes of helping
2. Server:
   - Cancels shared quests
   - Returns XP earned by host (they do not lose progress)
   - Marks Traitor permanently in this run
   - Reputation −= 5 globally
   - Host receives notification: "You were betrayed by <name>"

Host consequence: real shock. The game tells them.
Invader consequence: reputation stained forever.
```

## Edge cases

| Case | Behavior |
|---|---|
| Host and Ally both die | Both respawn. Ally loses shared progress bonus |
| Ally leaves mid-session | No penalty. Only loses shared progress bonus |
| Hostile dies to host | Loses the rare item they brought. It drops on the ground |
| Traitor re-enters same host | Blocked by reputation. Host has prior knowledge |
| Session empty (no host) | Ally/Hostile cannot enter. Teleport still works |
| Host in boss fight | Portal blocked until boss ends or player dies |

## Reputation is the weight

**Without reputation, betrayal is free.** With reputation, it is expensive.

The system answers the question "why not betray every time?":

- −5 rep is permanent. Recovery takes 5 successful Ally sessions.
- Below −20, hosts see a warning. You stop being accepted.
- Below −50, the covenant expels you. No more portals.

**Rule:** the first betrayal is tempting. The second is impossible.
