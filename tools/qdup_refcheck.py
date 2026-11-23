#!/usr/bin/env python3
"""Check the double reference (qdup_ref.py --substeps output) against an oracle.

The oracle samples RAM once per vblank, so a row can catch a sub-step half
done (some balls or fields already updated).  A row fits reference state k
when every watched field equals its value at sub-step k or k+1 (within TOL,
relative to max(1, |value|)).  Rows caught inside a cushion or collision
sequence (position set, then velocity, then the extra step) fit neither and
are skipped.  Reports how many rows fit, the worst position difference among
them, and the last row that fits (after it the two runs have parted).
  qdup_refcheck.py ORACLE_DIR REF.txt [TOL]
"""
import csv, sys

FIELDS = ('x', 'y', 'vx', 'vy')


def load_ref(path):
    states = []
    for line in open(path):
        if line[0] not in 'CS':
            continue
        t = line.split()
        balls = {}
        for f in t[1:]:
            i, rest = f.split(':')
            v = rest.split(',')
            balls[int(i)] = tuple(float(a) for a in v[:4])
        states.append((t[0], balls))
    return states


def fits(o, a, b, tol):
    worst = 0.0
    for n in o:
        for f in range(4):
            d = min(abs(o[n][f] - a[n][f]), abs(o[n][f] - b[n][f]))
            if d > tol * max(1.0, abs(o[n][f])):
                return None
            if f < 2:
                worst = max(worst, d)
    return worst


def main():
    od, refp = sys.argv[1], sys.argv[2]
    tol = float(sys.argv[3]) if len(sys.argv) > 3 else 1e-6
    rows = list(csv.DictReader(open(od + '/frames.csv')))
    ref = load_ref(refp)
    j = 0
    worst = 0.0
    matched = 0
    skipped = 0
    last_fit = None
    for row in rows:
        o = {b: tuple(float(row['b%d_%s' % (b, f)]) for f in FIELDS) for b in range(16)}
        hit = None
        for k in range(j, min(len(ref) - 1, j + 60)):
            w = fits(o, ref[k][1], ref[k + 1][1], tol)
            if w is not None:
                hit = (k, w)
                break
        if hit is None and j >= len(ref) - 2:
            w = fits(o, ref[-1][1], ref[-1][1], tol)
            if w is not None:
                hit = (len(ref) - 1, w)
        if hit is None:
            skipped += 1
            continue
        j = hit[0]
        worst = max(worst, hit[1])
        matched += 1
        last_fit = (row['tick'], ref[j][0], skipped)
    print('rows %d, fit %d (tol %g), worst position diff %.3g m, reference sub-steps %d, calls %d'
          % (len(rows), matched, tol, worst, len(ref), sum(1 for t, _ in ref if t[0] == 'C') - 1))
    print('last fitting row: tick %s = reference %s (%d rows skipped before it)' % last_fit)


main()
