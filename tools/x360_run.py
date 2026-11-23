#!/usr/bin/env python3
"""Run 360X headless, NMS's 1996 build or the rebuild, with game variables
logged every route tick.

    x360_run.py 96|rebuild|cd OUTDIR [--press SPEC] [--steps N] [--shots N]
                [--watch NAME ...] [--emu retail|devkit] [--audio]
                [--dump-ram FILE] [--memcard CARD.mcd | --no-memcard]

  96       NMS's MAIN.CPE (July 1996), symbols from MAIN.SYM
  rebuild  build/360x/360x.exe, symbols from build/360x/360x.elf (--build DIR
           for another output folder of 360x/build.sh, X360_OUT)
  cd       the disc image 360x/disc.sh makes, build/360x-cd/360x.cue, booted
           in the retail emulator with no PC link; symbols from the
           360x.elf next to it (--build DIR for another folder)

Both run on a fresh copy of the 1996 tree (X360_DATA, default the unpacked
360X.ZIP), served over the PC link from its root: the game opens
../INTRO/... from SOURCE. The 1996 build runs in the retail emulator by
default (it fits 2 MB; the dev kit emulator's GPU DMA is masked to 2 MB and
draws its heap-resident images black), the rebuild in the dev kit one
(linked for 8 MB). OUTDIR gets out.txt (debug console), vars.csv (one row
per route tick: the watched variables, 32-bit words read at each symbol's
word address and shifted to the symbol), screenshots with --shots N, and
audio.wav with --audio. --memcard gives port 1 a memory card kept in that
file (128 KiB; a missing file starts as a formatted card); --no-memcard
leaves both slots empty.
"""
import argparse
import csv
import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
from sn_sym import read_sym  # noqa: E402

DATA96 = os.environ.get('X360_DATA', os.path.join(ROOT, 'build/originals/360x'))
EMUS = {'retail': os.environ.get('PSOXIDE_EMU', 'psoxide-emulator'),
        'devkit': os.environ.get('PSOXIDE_EMU_DEVKIT', 'psoxide-emulator-devkit')}
DEFAULT_WATCH = ['gamecycle', 'fade', 'level', 'selectno', 'mallocused', 'craftx', 'crafty']
SIZES = {'difficulty': 1, 'fade': 1, 'level': 1, 'selectno': 1, 'screentoggle': 1, 'pause': 1, 'whichmenu': 1,
         'fuel': 2, 'shields': 2, 'weapontype': 2, 'craftdestroyed': 1, 'menuselection': 1}


def symbols96():
    return {n: a for a, n in read_sym(os.path.join(DATA96, 'SOURCE', 'MAIN.SYM'))[0]}


def symbols_rebuild(build):
    out = subprocess.run(['mipsel-none-elf-nm', os.path.join(build, '360x.elf')],
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
    ap.add_argument('--watch', nargs='*', default=DEFAULT_WATCH)
    ap.add_argument('--emu', choices=list(EMUS))
    ap.add_argument('--audio', action='store_true')
    ap.add_argument('--build')
    ap.add_argument('--dump-ram')
    ap.add_argument('--memcard')
    ap.add_argument('--no-memcard', action='store_true')
    a = ap.parse_args()
    a.build = a.build or os.path.join(ROOT, 'build/360x-cd' if a.which == 'cd' else 'build/360x')
    os.makedirs(a.out, exist_ok=True)
    data = os.path.join(a.out, 'data')
    if os.path.exists(data):
        shutil.rmtree(data)
    if a.which != 'cd':
        shutil.copytree(DATA96, data, ignore=shutil.ignore_patterns('*.OBJ', '*.ZIP', 'MAKEBACK', 'STARS'))
        if a.which == 'rebuild':
            # missions 2 to 9 load creature files the 1996 tree's LEVELn
            # folders lack; build them into the copy (360x/disc.sh does the same)
            subprocess.run([sys.executable, '-I', os.path.join(ROOT, 'tools', 'x360_txt2spr.py'), '--stage',
                            DATA96, os.path.join(ROOT, '360x', 'SOURCE', 'LOADDATA.C'), data], check=True)
        if a.which == 'rebuild':
            subprocess.run([sys.executable, '-I', os.path.join(ROOT, 'tools', 'x360_scs2spr.py'), '--stage',
                            os.environ.get('X360_SATURN', os.path.join(ROOT, 'build/originals/360x-saturn')), data], check=True)
    syms = symbols96() if a.which == '96' else symbols_rebuild(a.build)
    emu = EMUS[a.emu or ('devkit' if a.which == 'rebuild' else 'retail')]
    # NAME+OFFSET watches a word inside a structure (PWorld+4: its vz)
    for n in a.watch:
        if '+' in n and n.split('+')[0] in syms:
            syms[n] = syms[n.split('+')[0]] + int(n.split('+')[1], 0)
    names = [n for n in a.watch if n in syms]
    cmd = [emu, 'launch', '--steps', a.steps, *([] if a.analog else ['--digital-pad']), '--route-log', os.path.join(a.out, 'route.csv')]
    if a.which == '96':
        cmd += ['--pcdrv', data, '--path', os.path.join(data, 'SOURCE', 'MAIN.CPE')]
    elif a.which == 'rebuild':
        cmd += ['--pcdrv', data, '--path', os.path.join(a.build, '360x.exe')]
    else:
        cmd += ['--path', os.path.join(a.build, '360x.cue')]
    if a.dump_ram:
        cmd += ['--dump-ram', a.dump_ram]
    if a.memcard:
        cmd += ['--memcard', a.memcard]
    if a.no_memcard:
        cmd += ['--no-memcard']
    if a.press:
        cmd += ['--press', a.press]
    if a.shots:
        cmd += ['--route-screenshot-dir', a.out, '--route-screenshot-interval', str(a.shots)]
    if a.audio:
        cmd += ['--dump-audio', os.path.join(a.out, 'audio.wav')]
    for n in names:
        cmd += ['--route-watch-u32', hex(syms[n] & ~3)]
    with open(os.path.join(a.out, 'out.txt'), 'w') as f:
        subprocess.run(cmd, stdout=f, stderr=subprocess.STDOUT, check=False)
    rows = list(csv.reader(open(os.path.join(a.out, 'route.csv'))))
    with open(os.path.join(a.out, 'vars.csv'), 'w', newline='') as f:
        wr = csv.writer(f)
        wr.writerow(['tick'] + names)
        for r in rows[1:]:
            v = [int(x) for x in r[len(r) - len(names):]] if names else []
            vals = []
            for n, w in zip(names, v):
                size = SIZES.get(n, 4)
                vals.append((w >> (8 * (syms[n] & 3))) & ((1 << (8 * size)) - 1))
            wr.writerow([r[0]] + vals)
    if a.which != 'cd':
        shutil.rmtree(data)
    print('\n'.join(l for l in open(os.path.join(a.out, 'out.txt')).read().splitlines()
                    if not l.startswith('[pcdrv]'))[-1500:])


if __name__ == '__main__':
    sys.exit(main())
