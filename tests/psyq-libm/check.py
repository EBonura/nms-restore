#!/usr/bin/env python3
"""Run tests/psyq-libm in the emulator and compare each result with the
host's libm (correctly rounded or within an ulp or so).

    check.py [EXE]     default build/psyq-libm/libm.exe
"""
import math
import os
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EMU = os.environ.get('PSOXIDE_EMU', 'psoxide-emulator')
IN = [0.0, 1e-9, 0.1, 0.5, 0.7853981633974483, 1.0, 1.5707963267948966, 2.0, 3.0,
      3.141592653589793, 4.0, 6.283185307179586, 10.0, 100.0, 12345.678, -0.3, -2.5, -7.0,
      1.9198621771937625, 0.0174532925199433, 0.25, 0.75, 1.75, 0.999, 1e5]
REF = {
    'sin': math.sin, 'cos': math.cos, 'atan': math.atan,
    'atan2': lambda x: math.atan2(x, 0.7 - x), 'atan2n': lambda x: math.atan2(-x, -0.3 - x),
    'sqrt': lambda x: math.sqrt(abs(x)), 'hypot': lambda x: math.hypot(x, 1.3 * x - 0.2), 'floor': math.floor,
}
ATOF = [float('0.0525'), float('-1.2345e-3'), float('1002.5'), float('3.1415927')]


def ulps(a, b):
    ia, ib = (struct.unpack('<q', struct.pack('<d', v))[0] for v in (a, b))
    return abs(ia - ib)


def main():
    exe = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'build/psyq-libm/libm.exe')
    out = subprocess.run([EMU, 'launch', '--path', exe, '--steps', '400000000', '--stop-on-tty', 'LIBM DONE'],
                         capture_output=True, text=True).stdout + ''
    worst = {}
    n = 0
    for line in out.splitlines():
        p = line.split()
        if len(p) != 3 or not (p[0] in REF or p[0] == 'atof'):
            continue
        got = struct.unpack('>d', bytes.fromhex(p[2]))[0]
        i = int(p[1])
        want = ATOF[i] if p[0] == 'atof' else float(REF[p[0]](IN[i]))
        u = ulps(got, want)
        n += 1
        if u > worst.get(p[0], (0, 0))[0]:
            worst[p[0]] = (u, IN[i] if p[0] != 'atof' else i)
    print(f'{n} results; worst error in ulps per function (input):')
    for f in list(REF) + ['atof']:
        print(f'  {f:7s} {worst.get(f, (0, None))[0]:3d}  {worst.get(f, (0, None))[1]}')
    return 0 if n == 8 * len(IN) + 4 and all(v[0] <= 4 for v in worst.values()) else 1


if __name__ == '__main__':
    sys.exit(main())
