#!/usr/bin/env python3
"""Compare the fixed-point physics test against the 1996 build's physics.

  qdup_compare.py [--oracle ROOT] [--emu EMU] SHOT...

For each recorded shot this runs build/qdup-physics/<shot>_orig.exe and
<shot>_fixed.exe in the retail emulator, runs the double reference
(qdup_ref.py) with the same tables and cushion logic, and fits the oracle's
per-tick samples to reference sub-steps (as qdup_refcheck.py does).

_orig  : 1996 RAM tables, 1996 cushion logic.  Compared per call with the
         reference (tables 'ram', no clamp) and with the oracle rows that
         fit a call boundary.
_fixed : tables from the text file, extra-step clamp.  Compared per call
         with the reference (text tables, clamp); the 1996 build has no
         counterpart once a ball reaches a corrupted cushion.

Prints, per shot and variant: calls, position error against the reference
per call (max, and the first call over 1 mm and 1 cm), error against the
oracle on fitted rows, final positions, pocketed balls, the largest speed
ratio across a cushion hit, and CPU cycles per update_all_balls call.
"""
import argparse, csv, json, math, os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..'))
sys.path.insert(0, HERE)
import qdup_ref  # noqa: E402

TEXT_TABLE = os.path.join(os.environ.get('QDUP_DATA', os.path.join(ROOT, 'build/originals/qdup/WORK/SONY/POOL2')), 'TABLES/POTTERS/USTABLE.BMP')
EMU = os.environ.get('PSOXIDE_EMU', 'psoxide-emulator')
F24 = float(1 << 24)


def run_test(exe, emu):
    out = subprocess.run([emu, 'launch', '--path', exe, '--steps', '3000000000',
                          '--stop-on-tty', 'DONE'], capture_output=True, text=True).stdout
    res = {'calls': [], 'hit': None, 'init': []}
    for line in out.splitlines():
        t = line.split()
        if not t:
            continue
        if t[0] == 'HIT':
            res['hit'] = [int(v) for v in t[1:]]
        elif t[0] == 'INIT':
            res['init'].append(line)
        elif t[0][0] == 'C' and t[0][1:].isdigit():
            balls = []
            for f in t[2:]:
                i, rest = f.split(':')
                x, y, vx, vy, mv, ac = rest.split(',')
                balls.append((int(x) / F24, int(y) / F24, int(mv), int(ac), int(vx) / F24, int(vy) / F24))
            res['calls'].append((int(t[1]), balls))
        elif t[0] == 'END':
            res['end'] = line
    return res


def run_ref(state, tables, clamp, max_calls=3000):
    sim = qdup_ref.Sim(state, clamp=clamp, tables=tables)
    dt = 1.0 / sim.frame_rate
    snap = lambda: [(b.x, b.y, b.vx, b.vy, b.moving, b.active) for b in sim.balls[:sim.LAST_BALL]]
    calls = [snap()]
    subs = [('C0', snap())]
    n = [0]
    sim.substep_log = lambda cur: subs.append(('S', snap()))
    while n[0] < max_calls and sim.any_balls_moving():
        n[0] += 1
        sim.update_all_balls(dt)
        calls.append(snap())
        subs.append(('C%d' % n[0], snap()))
    return calls, subs, sim.events


def fit_oracle(rows, subs, tol=1e-6):
    """Map oracle rows to reference calls: [(tick, call, row_balls)]."""
    out = []
    j = 0
    for row in rows:
        o = [(float(row['b%d_x' % b]), float(row['b%d_y' % b]), float(row['b%d_vx' % b]),
              float(row['b%d_vy' % b])) for b in range(16)]
        for k in range(j, min(len(subs), j + 80)):
            st = subs[k][1]
            if all(abs(o[b][f] - st[b][f]) <= tol * max(1.0, abs(o[b][f]))
                   for b in range(len(st)) for f in range(4)):
                j = k
                if subs[k][0][0] == 'C':
                    out.append((int(row['tick']), int(subs[k][0][1:]), o))
                elif k + 1 < len(subs) and subs[k + 1][0][0] == 'C' and subs[k + 1][1] == st:
                    out.append((int(row['tick']), int(subs[k + 1][0][1:]), o))
                break
    return out


def pockets_of(states):
    """(call, ball, x, y) where a ball's active flag drops from 1."""
    res = []
    for n in range(1, len(states)):
        for b, (p, c) in enumerate(zip(states[n - 1], states[n])):
            if p[-1] == 1 and c[-1] != 1:
                res.append((n, b, c[0], c[1]))
    return res


def fmt_pockets(pk):
    return ', '.join('b%d@C%d (%.3f,%.3f)' % (b, n, x, y) for n, b, x, y in pk) or 'none'


def compare(shot, variant, args):
    od = os.path.join(args.oracle, shot)
    st = json.load(open(os.path.join(od, 'state.json')))
    exe = os.path.join(ROOT, 'build/qdup-physics', '%s_%s.exe' % (shot, variant))
    t = run_test(exe, args.emu)
    tables = None if variant == 'orig' else qdup_ref.text_tables(TEXT_TABLE)
    clamp = variant != 'orig'
    rcalls, subs, events = run_ref(st, tables, clamp)
    port = [c[1] for c in t['calls']]
    n = min(len(port), len(rcalls))
    errs = []
    for k in range(n):
        e = 0.0
        for b in range(len(port[k])):
            pb, rb = port[k][b], rcalls[k][b]
            if rb[5] == 1 or pb[3] == 1:
                e = max(e, math.hypot(pb[0] - rb[0], pb[1] - rb[1]))
        errs.append(e)
    first = lambda lim: next((k for k, e in enumerate(errs) if e > lim), None)
    print('  %-6s calls port %d ref %d | vs reference: max %.3g m, first call >1 mm: %s, >1 cm: %s'
          % (variant, len(port) - 1, len(rcalls) - 1, max(errs) if errs else 0,
             first(1e-3), first(1e-2)))
    for k in (10, 50, 100, 200):
        if k < n:
            print('         error at call %d: %.3g m' % (k, errs[k]))
    # final positions
    fp, fr = port[-1], rcalls[-1]
    fin = [math.hypot(fp[b][0] - fr[b][0], fp[b][1] - fr[b][1])
           for b in range(len(fp)) if fp[b][3] == 1 and fr[b][5] == 1]
    print('         final positions vs reference: max %.3g m, mean %.3g m over %d balls on the table'
          % (max(fin) if fin else 0, sum(fin) / len(fin) if fin else 0, len(fin)))
    print('         pocketed, port: %s' % fmt_pockets(pockets_of([[(b[0], b[1], b[3]) for b in c] for c in port])))
    print('         pocketed, ref:  %s' % fmt_pockets(pockets_of([[(b[0], b[1], b[5]) for b in c] for c in rcalls])))
    cush = [e for e in events if e[0] == 'cush']
    if cush:
        print('         ref cushion hits %d, largest speed ratio across a hit %.3f'
              % (len(cush), max(e[6] / e[5] for e in cush if e[5] > 0)))
    # kinetic energy of the balls on the table (translation only), per call
    def ke(c):
        return sum((0.115 if b == 0 else 0.1) * (p[4] * p[4] + p[5] * p[5])
                   for b, p in enumerate(c) if p[3] == 1)
    kes = [ke(c) for c in port]
    rise = [(kes[k] / kes[k - 1], k) for k in range(1, len(kes)) if kes[k - 1] > 0]
    if rise:
        r, k = max(rise)
        print('         port kinetic energy: largest call-to-call ratio %.4g (call %d)%s'
              % (r, k, '' if r <= 1.0 else '  ENERGY GAIN'))
    cyc = [c[0] for c in t['calls'][1:]]
    if cyc:
        print('         cycles per call: mean %d, max %d (%.1f%% of a 50 Hz frame at 33.87 MHz)'
              % (sum(cyc) / len(cyc), max(cyc), 100.0 * max(cyc) / (33868800 / 50)))
    if variant == 'orig':
        rows = list(csv.DictReader(open(os.path.join(od, 'frames.csv'))))
        fits = fit_oracle(rows, subs)
        oe = []
        for tick, call, o in fits:
            if call < len(port):
                e = 0.0
                for b in range(len(port[call])):
                    e = max(e, math.hypot(port[call][b][0] - o[b][0], port[call][b][1] - o[b][1]))
                oe.append((call, e))
        if oe:
            print('         vs oracle: %d rows at call boundaries (calls %d..%d), max error %.3g m, mean %.3g m'
                  % (len(oe), oe[0][0], oe[-1][0], max(e for _, e in oe), sum(e for _, e in oe) / len(oe)))
        rest = next((r for r in rows if not any(int(r['b%d_moving' % b]) and int(r['b%d_active' % b]) == 1
                                                 for b in range(16))), None)
        if rest:
            ofin = [math.hypot(fp[b][0] - float(rest['b%d_x' % b]), fp[b][1] - float(rest['b%d_y' % b]))
                    for b in range(16) if fp[b][3] == 1 and int(rest['b%d_active' % b]) == 1]
            print('         final positions vs oracle (tick %s, balls at rest): max %.3g m, mean %.3g m over %d balls'
                  % (rest['tick'], max(ofin) if ofin else 0, sum(ofin) / len(ofin) if ofin else 0, len(ofin)))
            gone = [b for b in range(16) if int(rest['b%d_active' % b]) != 1]
            print('         oracle balls off the table at rest: %s' % (gone or 'none'))
    if variant == 'orig' and t['hit']:
        h = t['hit']
        print('         hit_ball: vx %.6f vy %.6f, oracle %.6f %.6f (diff %.2g, %.2g m/s)'
              % (h[0] / F24, h[1] / F24, h[5] / F24, h[6] / F24, (h[0] - h[5]) / F24, (h[1] - h[6]) / F24))
    return errs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('shots', nargs='+')
    ap.add_argument('--oracle', default=os.path.join(ROOT, 'build/oracle'))
    ap.add_argument('--emu', default=EMU)
    ap.add_argument('--variants', default='orig,fixed')
    a = ap.parse_args()
    for s in a.shots:
        print(s)
        for v in a.variants.split(','):
            compare(s, v, a)


if __name__ == '__main__':
    main()
