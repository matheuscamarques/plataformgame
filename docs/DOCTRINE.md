# Doutrina — decisões de arquitetura pendentes

## P1 — Fallback de parte sem pixels: preservar ou invalidar?

**Estado:** `Body::rebuildFromSprite` cai no schema estático quando a
parte não tem pixels (`Body.cpp:147-154`). Partes sem pixel (ex.:
`ArmL`/`LegL` do player, braços/pernas/arma do slime) ganham caixas
de fallback — às vezes fora do sprite visível.

**Travado por teste:** `WeaponFallsBackWhenNoPixels`
(`tests/test_body_from_sprite.cpp`) preserva o fallback como
comportamento intencional.

**Contexto:** `humanoid(50, 30)` (fix 1) pôs os fallbacks do player a
~2px da borda — mesmo baseline aceito pros braços do slime. Caixas
dramaticamente flutuantes (12px) eram só dims invertidas, já
corrigidas.

**Virar prioridade quando:** inimigo com hitbox per-part real,
decepamento, mecânica de braço do player, ou o filete de 2px doer
no playtest. Aí: invalidar fallback no `Body.cpp:147-154`, reescrever
o teste acima com o novo invariante, e anotar que slime perde
braços/pernas/arma como alvos em produção.

**Não fazer junto:** bugfix visual (foi o fix 1, 1 linha).
