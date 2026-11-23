#!/usr/bin/env python3
"""Run build/psyq-snd/snd.exe in the emulator with the 360X tree over the PC
link and measure its audio: silence until the tick, then the B1_5 waveform
for its length at pitch 7D4h (172732 samples * 4096 / 2004 / 44100 Hz =
8.00 s), left channel only.

    check.py [DATA]      DATA: the 360X tree (default the unpacked 360X.ZIP)
"""
import os
import subprocess
import sys
import tempfile
import wave

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EMU = os.environ.get('PSOXIDE_EMU', 'psoxide-emulator')
DATA = sys.argv[1] if len(sys.argv) > 1 else os.environ.get('X360_DATA', os.path.join(ROOT, 'build/originals/360x'))


def main():
    vb = os.path.getsize(os.path.join(DATA, 'SOUND', 'B1_5.VB'))
    with tempfile.TemporaryDirectory() as tmp:
        wav = os.path.join(tmp, 'a.wav')
        out = subprocess.run([EMU, 'launch', '--path', os.path.join(ROOT, 'build/psyq-snd/snd.exe'), '--pcdrv', DATA,
                              '--steps', '600000000', '--stop-on-tty', 'snd: done', '--dump-audio', wav],
                             capture_output=True, text=True).stdout
        print('\n'.join(l for l in out.splitlines() if l.startswith('snd:')))
        w = wave.open(wav)
        rate, frames = w.getframerate(), w.readframes(w.getnframes())
    import struct
    n = len(frames) // 4
    left = [abs(struct.unpack_from('<h', frames, 4 * i)[0]) for i in range(n)]
    right = [abs(struct.unpack_from('<h', frames, 4 * i + 2)[0]) for i in range(n)]
    on = [i for i, v in enumerate(left) if v > 16]
    if not on:
        sys.exit('FAIL: silent')
    start, end = on[0], on[-1]
    expected = (vb // 16 * 28) * 4096 / 0x7D4 / rate
    print(f'sound from {start / rate:.3f} s for {(end - start) / rate:.3f} s (expected {expected:.3f} s), '
          f'left peak {max(left)}, right peak {max(right)}')
    ok = abs((end - start) / rate - expected) < 0.05 and max(right) == 0
    print('OK' if ok else 'FAIL')
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
