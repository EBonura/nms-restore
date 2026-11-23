#!/usr/bin/env python3
"""Whole games: QD Up's new computer search against NMS's.

    tests/qdup-ai/duel.py NAME [--cflags=...] [--view 2d|3d] [--starts N] [--jobs N]

Builds the disc with QDUP_AI_DUEL and QDUP_TEST_CPU_BOTH: the computer plays
both sides, one with NMS's search and one with qdup/new/aifast.c's.  Which
side, and a small turn of the opening shot, follow from the VBlank count at
the first turn, so each game type is played from N different times of the
Start press at the title.  Per game: the winner (the player on turn when
the rules set game_over), the balls each side potted, the turns played.
Results to build/qdup-ai/NAME-duel/.
"""
import argparse, os, subprocess, sys, tempfile, csv
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import run as R  # noqa: E402


def presses(balltype, delay):
    out = []
    for p in R.menu(balltype, True):
        t, rest = p.split(':', 1)
        out.append(f'{int(t) + delay}:{rest}')
    return out


def symbols(elf):
    out = subprocess.run(['mipsel-none-elf-nm', elf], capture_output=True, text=True).stdout
    return {p[2]: int(p[0], 16) for p in (l.split() for l in out.splitlines()) if len(p) == 3}


def play(disc, pr, steps, syms):
    with tempfile.TemporaryDirectory() as d:
        log = os.path.join(d, 'route.csv')
        cmd = ['launch', '--path', os.path.join(disc, 'qdup.cue'), '--steps', str(steps), '--digital-pad',
               '--press', ','.join(pr), '--route-log', log]
        for s in ('game_over', 'Player_Active'):
            cmd += ['--route-watch-u32', hex(syms[s] & ~3)]
        stdout = R.emu(cmd)
        rows = list(csv.reader(open(log)))[1:]
    over = None
    for row in rows:
        go, pa = int(row[-2]) >> (8 * (syms['game_over'] & 3)), int(row[-1]) >> (8 * (syms['Player_Active'] & 3))
        if go & 0xff:
            over = pa & 0xff
            break
    return [l for l in stdout.splitlines() if l[:6] in ('AIPOS ', 'AIDUEL')], over


def score(lines, over):
    turns = []
    pos = None
    for l in lines:
        t = l.split()
        if t[0] == 'AIPOS':
            pos = sum(1 for b in t[3:] if b.endswith(',1'))
        elif t[0] == 'AIDUEL' and pos is not None:
            turns.append((int(t[1]), int(t[2]), pos))
    fast_side = next((p if f else 1 - p for p, f, _ in turns), None)
    potted = {0: 0, 1: 0}
    for i in range(len(turns) - 1):
        potted[turns[i][0]] += max(0, turns[i][2] - turns[i + 1][2])
    return dict(turns=len(turns), fast_side=fast_side, winner=over,
                fast_pots=potted[fast_side] if fast_side is not None else 0,
                exact_pots=potted[1 - fast_side] if fast_side is not None else 0)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('name')
    ap.add_argument('--cflags', default='')
    ap.add_argument('--view', default='3d', choices=['2d', '3d'])
    ap.add_argument('--starts', type=int, default=10)
    ap.add_argument('--jobs', type=int, default=10)
    ap.add_argument('--steps', type=int, default=12000000000)
    a = ap.parse_args()
    R.VIEW = a.view
    out = os.path.join(R.ROOT, 'build/qdup-ai', a.name + '-duel')
    os.makedirs(out, exist_ok=True)
    disc = R.build(a.name + '-duel', a.view, a.cflags + ' -DQDUP_AI_DUEL', True, False)
    syms = symbols(os.path.join(disc, 'qdup.elf'))
    games = [(bt, d) for bt in R.TYPES for d in range(0, 7 * a.starts, 7)]
    res = []
    with ThreadPoolExecutor(a.jobs) as ex:
        for (bt, d), (lines, over) in zip(games, ex.map(lambda g: play(disc, presses(g[0], g[1]), a.steps, syms), games)):
            open(os.path.join(out, f'{a.view}-{R.TYPES[bt]}-{d}.txt'), 'w').write('\n'.join(lines) + f'\ngame_over_player {over}\n')
            s = score(lines, over)
            s['game'] = f'{R.TYPES[bt]}-{d}'
            res.append(s)
    lines = []
    fin = [s for s in res if s['winner'] is not None]
    fw = sum(1 for s in fin if s['winner'] == s['fast_side'])
    lines.append(f'games {len(res)}, finished {len(fin)}: new search won {fw}, NMS search won {len(fin) - fw}')
    lines.append(f'balls potted per game: new {sum(s["fast_pots"] for s in res) / len(res):.2f}, '
                 f'NMS {sum(s["exact_pots"] for s in res) / len(res):.2f}')
    for s in res:
        lines.append(f'  {s["game"]:10s} turns {s["turns"]:3d} new side {s["fast_side"]} winner {s["winner"]} '
                     f'pots new {s["fast_pots"]} NMS {s["exact_pots"]}')
    open(os.path.join(out, 'summary.txt'), 'w').write('\n'.join(lines) + '\n')
    print('\n'.join(lines))


if __name__ == '__main__':
    sys.exit(main())
