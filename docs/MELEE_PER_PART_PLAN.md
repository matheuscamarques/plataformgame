# Plano — alinhar melee com per-part narrowphase

## Contexto
Hoje coexistem dois modelos de colisão para o mesmo inimigo:
- `MeleeSystem` → AABB cru do `Enemy::body`
- `ExplosionSystem` → narrowphase por `bodyParts` com `bestMult`

O refactor do `Body` per-part não é usado no ataque principal.

## Decisões a travar antes do patch

### D1 — Fallback quando nenhuma parte é tocada
Sem schema → comportamento antigo AABB com `dmgMult = 1.0`
Com schema e nenhuma parte tocada → whiff, sem dano

### D2 — `postureMult`
`PartDef` possui `damageMult` e `postureMult`. Hoje está morto.
Usar `best->postureMult` no melee e na explosão para manter consistência.

### D3 — Broadphase obrigatória
Manter teste AABB do corpo antes do per-part para performance.

### D4 — Guardar referência da parte
Manter `const PartDef* best` e não só `bestMult` para acessar `postureMult` e debug.

## Patch final

`src/support/Combat/MeleeSystem.cpp`
```cpp
void MeleeSystem::tick(float dt, GameContext &ctx) {
    Player *p = ctx.player;
    if (!p || !ctx.enemies) return;
    if (ctx.input && ctx.input->pressed(Action::Heavy)) p->startSwing();
    if (p->updateMelee(dt) != MeleePhase::Active) return;

    const sf::FloatRect box = p->meleeHitbox();
    if (box.width <= 0.f) return;

    const int dmgBase = p->meleeDamage();
    const float postureBase = p->meleePosture();

    ctx.enemies->forEach([&](Enemy &s) {
        if (s.resources.isDead()) return;
        if (s.lastHitSwing == p->meleeSwingId) return;

        const sf::FloatRect sb{s.body.getX(), s.body.getY(),
                               s.body.getW(), s.body.getH()};
        if (!box.intersects(sb)) return;

        const PartDef* best = nullptr;
        float bestDmgMult = 0.f;
        s.bodyParts.forEach([&](const PartState &st, const PartDef &def){
            if (!box.intersects(st.worldBox)) return;
            if (def.damageMult > bestDmgMult) {
                bestDmgMult = def.damageMult;
                best = &def;
            }
        });

        float dmgMult = 1.0f;
        float postureMult = 1.0f;
        if (best) {
            dmgMult = best->damageMult;
            postureMult = best->postureMult;
        } else if (s.bodyParts.schema) {
            return; // whiff em inimigo real
        }

        const int dmg = static_cast<int>(dmgBase * dmgMult);
        s.resources.takeDamage(dmg);
        s.resources.damagePosture(postureBase * postureMult);
        if (s.ai) s.ai->onTakeHit(s, dmg, ctx);
        s.lastHitSwing = p->meleeSwingId;

        if (particles_) {
            particles_->spawnHitSpark({
                sb.left + sb.width * 0.5f,
                sb.top  + sb.height * 0.5f
            });
        }
    });
}
```

## Testes

`tests/test_melee_parts.cpp`
1. Head hit → 2x dano
2. Torso hit → 1x
3. Arm hit → 0.6x
4. Whiff → sem dano
5. Sem schema → AABB cru 1x compatibilidade
6. Dedup por swing
7. Partícula spawna

Teste de regressão obrigatório:
`NoRegressionOnExistingAABBBehavior` com inimigo sem schema.

## Impacto de gameplay

Antes: dano flat.
Depois:
- Cabeça = 2x
- Torso = 1x
- Braço = 0.6x

Muda a curva. Playtest obrigatório depois do patch.

## Ordem de execução
1. Commit A: MeleeSystem per-part + testes
2. Playtest 15 min
3. Decisão de balance de multipliers se necessário
4. Rollback simples com `git revert`

Não fazer junto: ContactDamageSystem simétrico, ajuste de multipliers antes do playtest.
