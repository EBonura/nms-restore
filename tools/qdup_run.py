#!/usr/bin/env python3
"""Run QD Up headless in the dev kit emulator, 1996 build or rebuild, with
the game variables of the debug overlay logged every route tick.

    qdup_run.py 96|rebuild|cd OUTDIR [--press SPEC] [--steps N] [--shots N]
                [--watch NAME[+OFF]]...

  96       NMS's POOL.CPE, from a fresh copy of the 1996 folder
  rebuild  build/qdup/qdup.exe, data from a fresh copy of the same folder
           (the game writes TABLES/POTTERS/USTABLE.CSH over the PC link,
           so neither run touches the original)
  cd       the disc image (build/qdup-cd/qdup.cue, --build picks another
           folder) in the retail emulator, 2 MB, no
           PC link

OUTDIR gets out.txt (debug console and summary), route.csv (one row per
route tick: frames, game_frames, active_page, cue_angle, event_flag,
mouse_ydiff, ball[0].x/.y/.vx in metres: doubles in the 1996 build, 8.24 in
the rebuild, which runs FIXBALL.C) and, with --shots N, a screenshot every N
route ticks.  --exe picks another rebuild (default build/qdup/qdup.exe).
--watch adds a game variable (a symbol, +OFFSET for a field) as a signed
32-bit column of vars.csv.
"""
import argparse
import csv
import os
import shutil
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA96 = os.environ.get('QDUP_DATA', os.path.join(ROOT, 'build/originals/qdup/WORK/SONY/POOL2'))
EMU = os.environ.get('PSOXIDE_EMU_DEVKIT', 'psoxide-emulator-devkit')
EMU_RETAIL = os.environ.get('PSOXIDE_EMU', 'psoxide-emulator')
WORDS = ['frames', 'game_frames', 'active_page', 'cue_angle', 'event_flag', 'mouse_ydiff']
BALL_X, BALL_Y, BALL_VX = 64, 72, 136		# 1996 build: BALL.C's doubles
FIX_X, FIX_Y, FIX_VX = 48, 52, 84			# rebuild: FIXBALL.H's 8.24 longs


def symbols96():
    syms = {}
    for line in open(os.path.join(DATA96, 'POOL.MAP'), errors='replace'):
        p = line.split()
        if len(p) == 2 and len(p[0]) == 8:
            try:
                syms.setdefault(p[1], int(p[0], 16))
            except ValueError:
                pass
    return syms


def symbols_rebuild(exe):
    out = subprocess.run(['mipsel-none-elf-nm', exe[:-4] + '.elf'],
                         capture_output=True, text=True, check=True).stdout
    return {p[2]: int(p[0], 16) for p in (l.split() for l in out.splitlines()) if len(p) == 3}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('which', choices=['96', 'rebuild', 'cd'])
    ap.add_argument('out')
    ap.add_argument('--press', default='')
    ap.add_argument('--analog', action='store_true', help='leave the pad in the emulator default, a DualShock in analog mode (ID 0x73), instead of the digital pad')
    ap.add_argument('--steps', default='2000000000')
    ap.add_argument('--shots', type=int, default=0)
    ap.add_argument('--exe', default=os.path.join(ROOT, 'build/qdup/qdup.exe'))
    ap.add_argument('--build', default=os.path.join(ROOT, 'build/qdup-cd'))
    ap.add_argument('--watch', action='append', default=[])
    ap.add_argument('--data', default=DATA96, help='the 1996 folder to copy (a patched POOL.CPE, say)')
    ap.add_argument('--extra', action='append', default=[], help='more emulator arguments')
    a = ap.parse_args()
    if a.which == 'cd':
        a.exe = os.path.join(a.build, 'qdup.exe')
    os.makedirs(a.out, exist_ok=True)
    data = os.path.join(a.out, 'data')
    if os.path.exists(data):
        shutil.rmtree(data)
    if a.which != 'cd':
        shutil.copytree(a.data, data)
    syms = symbols96() if a.which == '96' else symbols_rebuild(a.exe)
    fixed = a.which != '96'
    watch = []
    for name in WORDS:
        watch.append(syms[name] & ~3)
    ball = syms['ball']
    if fixed:
        watch += [ball + FIX_X, ball + FIX_Y, ball + FIX_VX]
    else:
        for off in (BALL_X, BALL_Y, BALL_VX):
            watch += [ball + off, ball + off + 4]
    nbase = len(watch)
    for w in a.watch:
        name, _, off = w.partition('+')
        watch.append((syms[name] + int(off or '0', 0)) & ~3)
    cmd = [EMU_RETAIL if a.which == 'cd' else EMU, 'launch', '--steps', a.steps, *([] if a.analog else ['--digital-pad']), '--route-log', os.path.join(a.out, 'route.csv')]
    if a.which == '96':
        cmd += ['--path', os.path.join(data, 'POOL.CPE')]
    elif a.which == 'cd':
        cmd += ['--path', os.path.join(a.build, 'qdup.cue')]
    else:
        cmd += ['--path', a.exe, '--pcdrv', data]
    if a.press:
        cmd += ['--press', a.press]
    if a.shots:
        cmd += ['--route-screenshot-dir', a.out, '--route-screenshot-interval', str(a.shots)]
    for w in watch:
        cmd += ['--route-watch-u32', hex(w)]
    cmd += a.extra
    with open(os.path.join(a.out, 'out.txt'), 'w') as f:
        subprocess.run(cmd, stdout=f, stderr=subprocess.STDOUT, check=False)
    # decode the watches into named columns
    rows = list(csv.reader(open(os.path.join(a.out, 'route.csv'))))
    n = len(watch)
    with open(os.path.join(a.out, 'vars.csv'), 'w', newline='') as f:
        wr = csv.writer(f)
        wr.writerow(['tick'] + WORDS + ['ball0_x', 'ball0_y', 'ball0_vx'] + a.watch)
        for r in rows[1:]:
            v = [int(x) for x in r[-n:]]
            extra = [struct.unpack('<i', struct.pack('<I', x))[0] for x in v[nbase:]]
            words = []
            for name, w, addr in zip(WORDS, v, watch):
                shift = 8 * ((syms[name] & 3))
                words.append((w >> shift) & (0xFFFF if name == 'event_flag' else 0xFFFFFFFF))
            words[3] = struct.unpack('<i', struct.pack('<I', words[3]))[0]
            words[5] = struct.unpack('<i', struct.pack('<I', words[5]))[0]
            if fixed:
                dbl = [struct.unpack('<i', struct.pack('<I', v[6 + i]))[0] / 16777216.0 for i in range(3)]
            else:
                dbl = [struct.unpack('<d', struct.pack('<II', v[6 + 2 * i], v[7 + 2 * i]))[0] for i in range(3)]
            wr.writerow([r[0]] + words + [f'{d:.6f}' for d in dbl] + extra)
    if os.path.exists(data):
        shutil.rmtree(data)
    print(open(os.path.join(a.out, 'out.txt')).read().strip().splitlines()[-2:])


if __name__ == '__main__':
    sys.exit(main())
