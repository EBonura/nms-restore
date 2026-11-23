#!/usr/bin/env python3
"""Computer turn times on the real disc: NMS's search (QDUP_EXACT_AI) or the
new one, 2D or 3D.

    tests/qdup-ai/times.py NAME [--exact] [--cflags=...] [--view 2d|3d] [--jobs N]

Builds the disc (no compare code) and plays run.py's one-player games
against the computer (eight scripts per game type) and the computer
against itself (QDUP_TEST_CPU_BOTH), logging calculating_computer_shot every
route tick.  A turn runs from the flag going on to it going off: the camera
ease, the search, the cue stroke (about 1.6 s of it is NMS's presentation).
Times in seconds (59.3 route ticks a second).  Each game runs for at most
ten minutes of host time.  Output in
build/qdup-ai/NAME-times/summary.txt.
"""
import argparse, csv, os, statistics, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import run as R  # noqa: E402
import duel as D  # noqa: E402


def build(name, view, cflags, cpu_both):
    out = os.path.join(R.ROOT, 'build/qdup-ai', name + '-times', f'{view}-{"cvc" if cpu_both else "hvc"}')
    os.makedirs(out, exist_ok=True)
    flags = cflags + (' -DQDUP_TEST_CPU_BOTH' if cpu_both else '')
    env = dict(os.environ, QDUP_CFLAGS=flags, LC_ALL='C')
    subprocess.run([os.path.join(R.ROOT, 'qdup/disc.sh')], env=env, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    src = os.path.join(R.ROOT, 'build/qdup-cd')
    for f in ('qdup.cue', 'qdup.bin', 'qdup.elf'):
        subprocess.run(['cp', os.path.join(src, f), out], check=True)
    return out


def turns(disc, presses, steps, addr, shift):
    with tempfile.TemporaryDirectory() as d:
        log = os.path.join(d, 'route.csv')
        R.emu(['launch', '--path', os.path.join(disc, 'qdup.cue'), '--steps', str(steps), '--digital-pad',
               '--press', ','.join(presses), '--route-log', log, '--route-watch-u32', hex(addr)], timeout=600)
        rows = [r for r in list(csv.reader(open(log)))[1:] if r and r[0].isdigit() and r[-1].isdigit()]
    out, start, prev = [], None, 0
    for row in rows:
        v = (int(row[-1]) >> shift) & 0xff
        if v and not prev:
            start = int(row[0])
        if prev and not v and start is not None:
            out.append((int(row[0]) - start) / 59.3)
        prev = v
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('name')
    ap.add_argument('--exact', action='store_true')
    ap.add_argument('--cflags', default='')
    ap.add_argument('--view', default='3d', choices=['2d', '3d'])
    ap.add_argument('--jobs', type=int, default=10)
    ap.add_argument('--steps', type=int, default=9000000000)
    a = ap.parse_args()
    R.VIEW = a.view
    cflags = a.cflags + (' -DQDUP_EXACT_AI' if a.exact else '')
    jobs = []
    for cpu_both in (True, False):
        disc = build(a.name, a.view, cflags, cpu_both)
        syms = D.symbols(os.path.join(disc, 'qdup.elf'))
        addr, shift = syms['calculating_computer_shot'] & ~3, 8 * (syms['calculating_computer_shot'] & 3)
        for bt in R.TYPES:
            if cpu_both:
                jobs.append((disc, R.menu(bt, True), addr, shift))
            else:
                for s in range(8):
                    jobs.append((disc, R.menu(bt, False) + R.player_shots(s), addr, shift))
    allt = []
    with ThreadPoolExecutor(a.jobs) as ex:
        for t in ex.map(lambda j: turns(j[0], j[1], a.steps, j[2], j[3]), jobs):
            allt += t
    allt.sort()
    n = len(allt)
    s = (f'{a.name} {a.view} {"NMS search" if a.exact else "new search"}: {n} computer turns, seconds: '
         f'min {allt[0]:.2f} median {statistics.median(allt):.2f} p90 {allt[int(0.9 * (n - 1))]:.2f} '
         f'p99 {allt[int(0.99 * (n - 1))]:.2f} max {allt[-1]:.2f}')
    out = os.path.join(R.ROOT, 'build/qdup-ai', a.name + '-times')
    open(os.path.join(out, 'summary.txt'), 'w').write(s + '\n' + ' '.join(f'{x:.2f}' for x in allt) + '\n')
    print(s)


if __name__ == '__main__':
    sys.exit(main())
