#!/usr/bin/env python3
"""Compare QD Up's computer shot searches: NMS's (exact) and qdup/new/aifast.c.

    tests/qdup-ai/run.py NAME [--view 3d|2d] [--cflags "-DQDUP_AI_BUDGET=4 ..."]
                         [--games all|cvc|hvc] [--jobs N] [--steps N] [--play-fast]

Builds QD Up's disc with QDUP_AI_COMPARE (both searches on every computer
turn; the game goes on with NMS's choice, or with the new search's with
--play-fast) and plays a fixed set of scripted games headless in the
retail emulator:

  (QDUP_AI_LABEL in --cflags also plays out every one of the 60 candidates
  on every turn and prints them, AILAB: the labelled corpus)
  cvc  the computer against itself (QDUP_TEST_CPU_BOTH), one game per game
       type (9-ball, UK pool, US 8-ball, trick shots)
  hvc  one player against the computer, each game type, with eight scripts
       of player shots (aim held for different times)

Each computer turn prints AIPOS (game type, player, every ball's x, y,
active) and AICMP (each search's number of the angle taken, cue angle,
whether that shot pots under the full physics and which balls, its time in
VBlanks; how many candidates were judged and played).  The turns go to
build/qdup-ai/NAME/<game>.txt; the summary to build/qdup-ai/NAME/summary.txt.
The positions and NMS's results are the corpus: with NMS's choice played,
every variant meets the same positions.
"""
import argparse, os, shutil, statistics, subprocess, sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../..'))
EMU = os.environ.get('PSOXIDE_EMU', 'psoxide-emulator')
VIEW = '3d'    # the view the games are played in: picked on the choice screen after START (menu); main sets it from --view
TYPES = {0: '9ball', 1: 'uk', 2: 'us8'}
# (the balls icon has three choices, no trick shots: BallType 1, 2, 0 in the order it cycles)


def menu(balltype, cpu):
    """Presses from the title through the option screen: players icon to
    the computer, balls icon to the game type, Start."""
    p = ['1000:start:10', '1150:up:6', '1210:up:6']     # (the title takes until about tick 1000 now: QDUP.DAT)
    p.append('1270:right:6')                    # players: computer
    p += ['1330:up:6', '1390:up:6']
    t = 1450
    for _ in range((balltype - 1) % 3):         # the balls icon starts on UK pool (BallType 1), three choices
        p.append(f'{t}:right:6'); t += 60
    for _ in range(4):
        p.append(f'{t}:down:6'); t += 60
    p.append(f'{t + 60}:cross:6')
    # the choice screen after START: 3D TABLE is picked at first, Down picks 2D FLAT; Cross starts the game
    if VIEW == '2d':
        p.append(f'{t + 180}:down:6')
    p.append(f'{t + 300}:cross:6')
    return p


def player_shots(script):
    """A player shot every 1500 ticks from 1700: aim left or right for a
    script-dependent time, then cross held, pull back, push."""
    p = []
    for n, t in enumerate(range(3800, 32000, 1500)):   # the game is up by about tick 3500 (with the choice screen)
        hold = 6 + ((script * 7 + n * 13) % 40)
        p.append(f'{t - 100}:{"left" if (script + n) % 2 else "right"}:{hold}')
        p += [f'{t}:cross:300', f'{t + 40}:down:120', f'{t + 166}:up:100']
    return p


def build(name, view, cflags, cpu_both, play_fast):
    out = os.path.join(ROOT, 'build/qdup-ai', name, f'{view}-{"cvc" if cpu_both else "hvc"}')
    os.makedirs(out, exist_ok=True)
    flags = f'-DQDUP_AI_COMPARE {cflags}'
    if cpu_both:
        flags += ' -DQDUP_TEST_CPU_BOTH'
    if play_fast:
        flags += ' -DQDUP_AI_PLAY_FAST'
    env = dict(os.environ, QDUP_CFLAGS=flags, LC_ALL='C', PSYQ_DEBUG='1')  # RESTORE: the run reads the debug console (owner's decision, 2026-11-13)
    subprocess.run([os.path.join(ROOT, 'qdup/disc.sh')], env=env, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    src = os.path.join(ROOT, 'build/qdup-cd')
    for f in ('qdup.cue', 'qdup.bin', 'qdup.elf'):
        shutil.copy(os.path.join(src, f), out)
    return out


def emu(args, timeout=None):
    """Run the emulator; its console output.  (Through a file, not a pipe:
    collecting it through a pipe sometimes never saw the end of it.)  With a
    timeout in seconds of host time the run is stopped there (some 3D games
    reach states the emulator runs very slowly); what it wrote is kept."""
    import tempfile
    with tempfile.TemporaryFile('w+') as f:
        p = subprocess.Popen([EMU] + args, stdout=f, stderr=subprocess.DEVNULL)
        try:
            p.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            p.kill()
            p.wait()
        f.seek(0)
        return f.read()


def play(disc, presses, steps):
    out = emu(['launch', '--path', os.path.join(disc, 'qdup.cue'), '--steps', str(steps),
               '--digital-pad', '--press', ','.join(presses)])
    return [l for l in out.splitlines() if l[:5] in ('AIPOS', 'AICMP', 'AILAB', 'AIPOC')]


def parse(lines):
    turns, pos = [], None
    for l in lines:
        t = l.split()
        if t[0] == 'AIPOS':
            pos = l
        elif t[0] == 'AILAB' and turns:
            turns[-1]['lab'] = l
        elif t[0] == 'AICMP' and pos:
            ex = dict(k=int(t[2]), angle=int(t[3]), pot=int(t[4]), mask=t[5], t=int(t[6]))
            fa = dict(k=int(t[8]), angle=int(t[9]), pot=int(t[10]), mask=t[11], t=int(t[12]))
            turns.append(dict(pos=pos, ex=ex, fa=fa, judged=int(t[14]), trials=int(t[16]),
                              passes=int(t[18]) if len(t) > 18 else 0, work=int(t[20]) if len(t) > 20 else 0))
            pos = None
    return turns


def summary(allturns):
    n = len(allturns)
    if not n:
        return 'no turns'
    same = sum(1 for x in allturns if x['ex']['angle'] == x['fa']['angle'])
    expot = [x for x in allturns if x['ex']['pot']]
    agree_pot = sum(1 for x in expot if x['fa']['pot'])
    exnot = [x for x in allturns if not x['ex']['pot']]
    agree_not = sum(1 for x in exnot if x['fa']['angle'] == x['ex']['angle'])
    fa_extra = sum(1 for x in exnot if x['fa']['pot'])
    agree = sum(1 for x in allturns if x['ex']['pot'] == x['fa']['pot'])
    def st(v):
        v = sorted(v)
        return f'min {v[0]} median {statistics.median(v)} p90 {v[int(0.9 * (len(v) - 1))]} max {v[-1]}'
    return '\n'.join([
        f'turns {n}',
        f'same shot (angle) {same}/{n} = {100 * same / n:.1f}%',
        f'pot agreement {agree}/{n} = {100 * agree / n:.1f}%',
        f'  NMS shot pots: {len(expot)}; new shot also pots {agree_pot} ({100 * agree_pot / max(1, len(expot)):.1f}%)',
        f'  NMS shot pots nothing: {len(exnot)}; new takes the same fall-back {agree_not}, new pots {fa_extra}',
        f'search time in VBlanks, NMS: {st([x["ex"]["t"] for x in allturns])}',
        f'search time in VBlanks, new: {st([x["fa"]["t"] for x in allturns])}',
        f'candidates judged to pot: {st([x["judged"] for x in allturns])}; trials: {st([x["trials"] for x in allturns])}',
        f'passes: {st([x["passes"] for x in allturns])}; work (moving balls x passes): {st([x["work"] for x in allturns])}',
    ])


def main():
    global VIEW
    ap = argparse.ArgumentParser()
    ap.add_argument('name')
    ap.add_argument('--view', default='3d', choices=['2d', '3d'])
    ap.add_argument('--cflags', default='')
    ap.add_argument('--games', default='all')
    ap.add_argument('--jobs', type=int, default=6)
    ap.add_argument('--steps', type=int, default=9000000000)
    ap.add_argument('--play-fast', action='store_true')
    a = ap.parse_args()
    VIEW = a.view
    out = os.path.join(ROOT, 'build/qdup-ai', a.name)
    os.makedirs(out, exist_ok=True)
    games = []
    if a.games in ('all', 'cvc'):
        disc = build(a.name, a.view, a.cflags, True, a.play_fast)
        for bt in TYPES:
            games.append((f'{a.view}-cvc-{TYPES[bt]}', disc, menu(bt, True)))
    if a.games in ('all', 'hvc'):
        disc = build(a.name, a.view, a.cflags, False, a.play_fast)
        for bt in TYPES:
            for s in range(8):
                games.append((f'{a.view}-hvc-{TYPES[bt]}-{s}', disc, menu(bt, False) + player_shots(s)))
    allturns = []
    with ThreadPoolExecutor(a.jobs) as ex:
        for (gname, _, _), lines in zip(games, ex.map(lambda g: play(g[1], g[2], a.steps), games)):
            open(os.path.join(out, gname + '.txt'), 'w').write('\n'.join(lines) + '\n')
            allturns += parse(lines)
    s = summary(allturns)
    open(os.path.join(out, 'summary.txt'), 'w').write(s + '\n')
    print(s)


if __name__ == '__main__':
    sys.exit(main())
