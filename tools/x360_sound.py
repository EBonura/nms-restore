#!/usr/bin/env python3
"""What 360X's disc build plays, key-on by key-on, from a headless run.

    x360_sound.py OUTDIR [--press SPEC] [--steps N] [--build DIR] [--song SEQ]

Runs tools/x360_run.py cd (retail emulator, muted: the audio goes to
OUTDIR/audio.wav only) with the layer's voice table (psyq/src/snd.rs, SND's
first 24 entries: the registers last set for each voice) watched every route
tick, and writes OUTDIR/keyons.csv: route tick, vertical blank, screen
(selectno, and whether a mission is on), voice, left and right volume,
pitch, start address in sound RAM, and the bank, program and note those
registers stand for. Banks are told apart by their waveforms' addresses
from 1010h; OPTIONS and the B banks both start there, so a start of 1010h
with the right volume at 0 and pitch 7D4h is read as B1_5 (the ship demo's
note: program pan 0, note 65), anything else at 1010h as OPTIONS program 49.
A voice keyed again with the same registers is not seen.

--song SEQ checks the key-ons from the first OPTIONS key-on of a mission on
(or of the route ticks --window FROM TO, for the menu music; in a stretch
where the song is not restarted, by a volume change or a new screen's sound)
against what the song's file says, played at 9.6 ticks a vertical blank (the
PAL rate) for ever: for each note, each tone of its program whose key range
holds it, with the 1996 library's volume (velocity x channel volume / 127,
squared, 3FFFh full) and pitch; printed: how many are seen within 3 vertical
blanks, and the timing.
"""
import argparse
import bisect
import csv
import math
import os
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA = os.environ.get('X360_DATA', os.path.join(ROOT, 'build/originals/360x'))
BODY = 0x1010


def vab(path):
    """{program: (volume, [tones])}, waveform sound-RAM addresses from 1010h."""
    h = open(path, 'rb').read()
    ps, vs = struct.unpack_from('<H', h, 18)[0], struct.unpack_from('<H', h, 22)[0]
    sizes = [struct.unpack_from('<H', h, 32 + 2048 + ps * 512 + 2 * i)[0] * 8 for i in range(vs + 1)]
    progs, block = {}, 0
    for p in range(128):
        n = h[32 + p * 16]
        if not n:
            continue
        tones = []
        for k in range(n):
            o = 32 + 2048 + (block * 16 + k) * 32
            wave = struct.unpack_from('<h', h, o + 22)[0]
            tones.append({'centre': h[o + 4], 'shift': h[o + 5], 'min': h[o + 6], 'max': h[o + 7],
                          'start': BODY + sum(sizes[1:wave])})
        progs[p] = tones
        block += 1
    return progs


def pitch(note, centre, shift):
    """The 1996 library's note-to-pitch rule (psyq/src/snd.rs)."""
    f = shift // 8
    carry = 1 if f >= 16 else 0
    f -= 16 * carry
    s = note + 60 - centre + carry
    o, r = s // 12, s % 12
    base = math.floor(4096 * 2 ** ((r + f / 16) / 12) + 1e-9)
    return base << (o - 5) if o > 5 else base >> (5 - o)


def song(path):
    """(tick, channel, program, note, velocity, channel volume) per note-on, and the length."""
    d = open(path, 'rb').read()
    i, t, st, prog, vol, out = 15, 0, 0, {}, {}, []
    while True:
        dt = 0
        while True:
            b = d[i]
            i += 1
            dt = dt << 7 | b & 0x7F
            if not b & 0x80:
                break
        t += dt
        if d[i] & 0x80:
            st = d[i]
            i += 1
        if st == 0xFF:
            if d[i] == 0x2F:
                return out, t
            i += 4 if d[i] == 0x51 else 2
            continue
        hi, ch = st & 0xF0, st & 15
        a, b = d[i], d[i + 1] if hi not in (0xC0, 0xD0) else 0
        i += 1 if hi in (0xC0, 0xD0) else 2
        if hi == 0xC0:
            prog[ch] = a
        elif hi == 0xB0 and a == 7:
            vol[ch] = b
        elif hi == 0x90 and b:
            out.append((t, ch, prog.get(ch, 0), a, b, vol.get(ch, 127)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('out')
    ap.add_argument('--press', default='')
    ap.add_argument('--steps', default='3000000000')
    ap.add_argument('--build', default=os.path.join(ROOT, 'build/360x-cd'))
    ap.add_argument('--song')
    ap.add_argument('--window', nargs=2, type=int, metavar=('FROM', 'TO'),
                    help='check the song over these route ticks instead of the mission')
    a = ap.parse_args()
    nm = subprocess.run(['mipsel-none-elf-nm', os.path.join(a.build, '360x.elf')], capture_output=True, text=True,
                        check=True).stdout
    snd = next(l.split()[2] for l in nm.splitlines() if l.endswith('3SND') or '3snd3SND' in l)
    watch = ['vblank', 'selectno', 'craftx'] + [f'{snd}+{16 * n + o}' for n in range(24) for o in (0, 4, 8)]
    subprocess.run([sys.executable, '-I', os.path.join(ROOT, 'tools/x360_run.py'), 'cd', a.out, '--build', a.build,
                    '--press', a.press, '--steps', a.steps, '--audio', '--watch'] + watch,
                   stdout=subprocess.DEVNULL, check=True)
    options = vab(os.path.join(DATA, 'SOUND/OPTIONS.VH'))
    by_start = {t['start']: (p, t) for p, ts in options.items() for t in ts}
    rows = list(csv.DictReader(open(os.path.join(a.out, 'vars.csv'))))
    keys, prev = [], None
    for r in rows:
        st = [tuple(int(r[f'{snd}+{16 * n + o}']) for o in (0, 4, 8)) for n in range(24)]
        if prev:
            for n in range(24):
                if st[n] != prev[n] and st[n][2]:
                    lr, pp, start = st[n]
                    l, rr, p = lr & 0xFFFF, lr >> 16, pp & 0xFFFF
                    if start == BODY and rr == 0 and p == 0x7D4:
                        bank, prog, note = 'B1_5', 0, 65
                    elif start in by_start:
                        prog, t = by_start[start]
                        bank = 'OPTIONS'
                        note = next((x for x in range(128) if pitch(x, t['centre'], t['shift']) == p), '')
                    else:
                        bank, prog, note = '?', '', ''
                    keys.append((int(r['tick']), int(r['vblank']), int(r['selectno']), int(r['craftx']) != 0, n,
                                 l, rr, p, start, bank, prog, note))
        prev = st
    with open(os.path.join(a.out, 'keyons.csv'), 'w', newline='') as f:
        w = csv.writer(f)
        w.writerow(['tick', 'vblank', 'selectno', 'mission', 'voice', 'left', 'right', 'pitch', 'start', 'bank',
                    'program', 'note'])
        w.writerows(keys)
    count = {}
    for k in keys:
        c = ('mission' if k[3] else f'screen {k[2]}', k[9], k[10])
        count[c] = count.get(c, 0) + 1
    print(f'{len(keys)} key-ons seen; by screen, bank and program:')
    for c, n in sorted(count.items(), key=str):
        print(f'  {c[0]:>12}  {c[1]:8} program {c[2]!s:4} {n}')
    if a.song:
        notes, length = song(a.song)
        expect = []
        for t, ch, prog, note, vel, cvol in notes:
            for tone in options.get(prog, []):
                if tone['min'] <= note <= tone['max']:
                    x = vel * cvol // 127
                    expect.append((t, x * x * 0x3FFF // (127 * 127), pitch(note, tone['centre'], tone['shift']),
                                   tone['start']))
        if a.window:
            seen = [k for k in keys if a.window[0] <= k[0] <= a.window[1] and k[9] == 'OPTIONS']
        else:
            seen = [k for k in keys if k[3] and k[9] == 'OPTIONS']
        # the first key-on seen is the song's first note, which need not be at tick 0
        v0 = seen[0][1] - expect[0][0] / 9.6
        last = seen[-1][1] - v0 - 3
        plays = int(last * 9.6 / length) + 1
        allexp = [((t + n * length) / 9.6, v, p, s) for n in range(plays) for t, v, p, s in expect]
        allexp = [e for e in allexp if e[0] <= last]
        ts = [k[1] - v0 for k in seen]
        used, late = set(), []
        for t, v, p, s in allexp:
            lo, hi = bisect.bisect_left(ts, t - 3), bisect.bisect_right(ts, t + 3)
            c = [j for j in range(lo, hi) if j not in used and (seen[j][5], seen[j][7], seen[j][8]) == (v, p, s)]
            if c:
                j = min(c, key=lambda j: abs(ts[j] - t))
                used.add(j)
                late.append(ts[j] - t)
        print(f'{os.path.basename(a.song)}: {len(expect)} key-ons a play of {length} ticks ({length / 9.6:.1f} vertical '
              f'blanks); over {plays} plays {len(late)} of {len(allexp)} seen within 3 vertical blanks with the '
              f"song's volume, pitch and start; timing {min(late):+.1f} to {max(late):+.1f} vertical blanks")


if __name__ == '__main__':
    main()
