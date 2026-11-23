#!/usr/bin/env python3
"""Compare the layer's SEQ playback with NMS's March 1996 build of 360X.

The March build (NMS.zip NMS/360WORK/MAIN.CPE, dev kit emulator) loads
OPTIONS.VH/VB, OPTPA.SEQ and OPTPB.SEQ over the PC link and plays OPTPA from
its vertical blank callback with Sony's libsnd; build/psyq-snd/seq.exe makes
the same calls on the layer. Both run headless and muted (audio to WAV
files only). Printed: the voice registers each build's library set at each
key-on (volume, pitch, start address; the 1996 library's from its voice
table in RAM at 4C5A0h, the layer's from its own), matched note for note
within one vertical blank; the 50 ms loudness envelope's correlation and
level ratio; the peaks.

    check_seq.py [SECONDS]      default 48 (two plays of OPTPA and a half)
"""
import csv
import os
import shutil
import subprocess
import sys
import tempfile
import wave

import numpy as np

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EMU = os.environ.get('PSOXIDE_EMU', 'psoxide-emulator')
EMU_DEVKIT = os.environ.get('PSOXIDE_EMU_DEVKIT', 'psoxide-emulator-devkit')
DATA = os.environ.get('X360_DATA', os.path.join(ROOT, 'build/originals/360x'))
# NMS's March 1996 360X build, NMS/360WORK/MAIN.CPE in NMS.zip (tools/originals.sh does not unpack it)
MARCH = os.environ.get('X360_MARCH_CPE', os.path.join(ROOT, 'build/originals/nms/NMS/360WORK/MAIN.CPE'))
MARCH_VBLANK = 0x49F10          # MAIN.SYM: vblank
MARCH_VOICES = 0x4C5A4          # its libsnd's voice table: L|R<<16, pitch|start/8<<16
SECONDS = float(sys.argv[1]) if len(sys.argv) > 1 else 48


def nm(elf, suffix):
    out = subprocess.run(['mipsel-none-elf-nm', elf], capture_output=True, text=True, check=True).stdout
    return int(next(l.split()[0] for l in out.splitlines() if l.endswith(suffix)), 16)


def key_ons(route, words):
    """(vblank since the first key-on, voice, left, right, pitch, start) at
    every change of a voice's registers."""
    rows = list(csv.reader(open(route)))
    ram = [i for i, h in enumerate(rows[0]) if h.startswith('ram_')]
    out, prev, v0 = [], None, None
    for r in rows[1:]:
        v = [int(r[i]) for i in ram]
        st = []
        for n in range(24):
            w = v[1 + words * n:1 + words * (n + 1)]
            start = (w[1] >> 16) * 8 if words == 2 else w[2]
            st.append((w[0] & 0xFFFF, w[0] >> 16, w[1] & 0xFFFF, start))
        if prev:
            for n in range(24):
                if st[n] != prev[n]:
                    v0 = v[0] if v0 is None else v0
                    out.append((v[0] - v0, n) + st[n])
        prev = st
    return out


def wav(path):
    w = wave.open(path)
    return np.frombuffer(w.readframes(w.getnframes()), dtype='<i2').reshape(-1, 2).astype(float)


def main():
    steps = str(int(SECONDS * 33e6) + 300_000_000)
    with tempfile.TemporaryDirectory() as tmp:
        tree = os.path.join(tmp, 'tree')
        os.makedirs(os.path.join(tree, 'SOURCE'))
        shutil.copytree(os.path.join(DATA, 'SOUND'), os.path.join(tree, 'SOUND'))
        shutil.copy(MARCH, os.path.join(tree, 'SOURCE', 'MAIN.CPE'))
        w = ['--route-watch-u32', hex(MARCH_VBLANK)]
        for n in range(24):
            w += ['--route-watch-u32', hex(MARCH_VOICES + 16 * n), '--route-watch-u32', hex(MARCH_VOICES + 16 * n + 4)]
        subprocess.run([EMU_DEVKIT, 'launch', '--path', os.path.join(tree, 'SOURCE', 'MAIN.CPE'), '--pcdrv', tree,
                        '--steps', steps, '--route-log', os.path.join(tmp, 'a.csv'),
                        '--dump-audio', os.path.join(tmp, 'a.wav')] + w, capture_output=True, check=True)
        elf = os.path.join(ROOT, 'build/psyq-snd/seq.elf')
        snd, vbl = nm(elf, '3SND'), nm(elf, ' vbl')
        w = ['--route-watch-u32', hex(vbl)]
        for n in range(24):
            w += [x for o in (0, 4, 8) for x in ('--route-watch-u32', hex(snd + 16 * n + o))]
        subprocess.run([EMU, 'launch', '--path', os.path.join(ROOT, 'build/psyq-snd/seq.exe'), '--pcdrv', DATA,
                        '--steps', '3000000000', '--stop-on-tty', 'seq: done', '--route-log', os.path.join(tmp, 'b.csv'),
                        '--dump-audio', os.path.join(tmp, 'b.wav')] + w, capture_output=True, check=True)
        a, b = key_ons(os.path.join(tmp, 'a.csv'), 2), key_ons(os.path.join(tmp, 'b.csv'), 3)
        A, B = wav(os.path.join(tmp, 'a.wav')), wav(os.path.join(tmp, 'b.wav'))
    end = SECONDS * 50 - 50
    a = [x for x in a if x[0] < end]
    b = [x for x in b if x[0] < end + 2]
    used, regs, voice = set(), 0, 0
    for x in a:
        c = [j for j, y in enumerate(b) if j not in used and abs(y[0] - x[0]) <= 1 and y[2:] == x[2:]]
        if c:
            used.add(c[0])
            regs += 1
            voice += b[c[0]][1] == x[1]
    print(f'1996 key-ons: {len(a)}; the layer set the same registers within one vertical blank for {regs} '
          f'({voice} on the same voice number)')
    on = lambda x: int(np.argmax(np.abs(x).max(axis=1) > 16))
    A, B = A[on(A):], B[on(B):]
    n = int(min(len(A), len(B), SECONDS * 44100)) // 2205 * 2205
    ea = np.sqrt((A[:n] ** 2).reshape(-1, 2205, 2).mean(axis=1))
    eb = np.sqrt((B[:n] ** 2).reshape(-1, 2205, 2).mean(axis=1))
    corr = min(np.corrcoef(ea[:, c], eb[:, c])[0, 1] for c in (0, 1))
    ratio = eb.sum() / ea.sum()
    print(f'50 ms loudness envelope: correlation {corr:.4f}, level ratio (layer/1996) {ratio:.3f}; '
          f'peaks 1996 {int(np.abs(A).max())}, layer {int(np.abs(B).max())} of 32767')
    ok = regs >= 0.95 * len(a) and corr > 0.95 and abs(ratio - 1) < 0.05
    print('OK' if ok else 'FAIL')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
