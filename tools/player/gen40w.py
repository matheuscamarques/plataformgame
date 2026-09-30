#!/usr/bin/env python3
"""Fase1: canvas 40 centralizado (14 + 12 + 14). Mecânico, pixel-idêntico."""
import re

P = '/home/sanonichan/projetos/plataformgame/src/assets/Sprites/'
pp = P + 'PlayerParts.h'
ps = P + 'PlayerSprites.h'

txt = open(pp).read()
arr_re = re.compile(r'inline const char\* const (\w+)\[\] = \{(.*?)\};', re.S)
arrays = {}
for m in arr_re.finditer(txt):
    rows = re.findall(r'\"([^\"]*)\"', m.group(2))
    assert len(rows) == 40 and all(len(r) == 12 for r in rows), m.group(1)
    arrays[m.group(1)] = rows

# Reescreve arrays: 14 dots + conteúdo + 14 dots (colunas 14-25).
out, pos = [], 0
for m in arr_re.finditer(txt):
    n = m.group(1)
    out.append(txt[pos:m.start()])
    lines = [f'inline const char* const {n}[] = {{']
    for i, r in enumerate(arrays[n]):
        lines.append('    "' + '.' * 14 + r + '.' * 14 + f'", // y={i}')
    lines.append('};')
    out.append('\n'.join(lines))
    pos = m.end()
out.append(txt[pos:])
new = ''.join(out)

# Part entries { name, 12, 40, 0, 0 } -> { name, 40, 40, 0, 0 }.
entry_re = re.compile(r'\{\s*(\w+),\s*12,\s*40,\s*0,\s*0\s*\}')
n_fix = 0
def fix(m):
    global n_fix
    if m.group(1) not in arrays:
        return m.group(0)
    n_fix += 1
    return '{ ' + f'{m.group(1)},'.ljust(22) + '40, 40, 0, 0 }'
new = entry_re.sub(fix, new)
open(pp, 'w').write(new)

# kPlayerW 12 -> 40 (linha exata).
s = open(ps).read()
old = 'inline constexpr int kPlayerW = 12;'
assert old in s
open(ps, 'w').write(s.replace(old, 'inline constexpr int kPlayerW = 40;'))
print(f'arrays: {len(arrays)} entries: {n_fix} kPlayerW=40')
