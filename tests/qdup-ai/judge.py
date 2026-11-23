#!/usr/bin/env python3
"""Tune the geometric judge of qdup/new/aifast.c on the labelled corpus.

    tests/qdup-ai/judge.py tests/qdup-ai/corpus [--tol 150] [--budget 0] [--sweep|--capped]

tests/qdup-ai/corpus holds the labelled corpus this branch was tuned on
(549 computer turns, 2D, every candidate played out), gzipped.

The labelled corpus (tests/qdup-ai/run.py with -DQDUP_AI_LABEL) has, for
every computer turn, the position and all 60 of NMS's candidate angles
played out with the full physics: the white's starting velocity, whether
the candidate pots an object ball, which.  This replays the new search's
choice for a setting (a model of judge() in floating point) and compares
it with NMS's (the first candidate that pots; the ghost angle from the
45th on or when none pots): same shot, pot agreement, full trials played.
"""
import argparse, glob, gzip, math, os, statistics, sys


def load(d):
    turns, pockets = [], None
    for f in sorted(glob.glob(os.path.join(d, '*.txt')) + glob.glob(os.path.join(d, '*.txt.gz'))):
        if f.endswith('summary.txt'):
            continue
        pos = lab = None
        for l in (gzip.open(f, 'rt') if f.endswith('.gz') else open(f)):
            t = l.split()
            if not t:
                continue
            if t[0] == 'AIPOCK':
                pockets = [tuple(int(v) / 16777216 for v in p.split(',')) for p in t[1:7]]
                radii = (int(t[7]) / 16777216, int(t[8]) / 16777216)
            elif t[0] == 'AIPOS':
                pos = [tuple(int(v) for v in b.split(',')) for b in t[3:]]
                lab = None
            elif t[0] == 'AILAB' and len(t) == 62 and t[-1].count(',') == 5:   # (a run can end mid-line)
                ghost = int(t[1])
                cands = []
                for c in t[2:]:
                    a, vx, vy, r, m, n = c.split(',')
                    cands.append((int(a), int(vx) / 16777216, int(vy) / 16777216, int(r), int(m, 16), int(n)))
                lab = (ghost, cands)
            elif t[0] == 'AICMP' and pos and lab:
                turns.append(dict(game=os.path.basename(f), pos=pos, ghost=lab[0], cands=lab[1],
                                  ex_angle=int(t[3]), ex_k=int(t[2]), ex_t=int(t[6])))
                pos = lab = None
    return turns, pockets, radii


def judge(pos, vx, vy, pockets, radii, tol):
    w = pos[0]
    wx, wy = w[0] / 16777216, w[1] / 16777216
    v = math.hypot(vx, vy)
    if v == 0:
        return False
    ux, uy = vx / v, vy / v
    rw, rb = radii
    best, hit = 1e9, None
    for i in range(1, len(pos)):
        if pos[i][2] != 1:
            continue
        dx, dy = pos[i][0] / 16777216 - wx, pos[i][1] / 16777216 - wy
        t = dx * ux + dy * uy
        p = dx * uy - dy * ux
        if t > 0 and abs(p) < rw + rb and t < best:
            best, hit, hp = t, i, p
    if hit is None:
        return False
    R = rw + rb
    back = math.sqrt(R * R - hp * hp)
    cx, cy = wx + ux * (best - back), wy + uy * (best - back)
    bx, by = pos[hit][0] / 16777216, pos[hit][1] / 16777216
    ox, oy = (bx - cx) / R, (by - cy) / R
    for px, py, pr in pockets:
        ex, ey = px - bx, py - by
        s = ex * ox + ey * oy
        q = ex * oy - ey * ox
        if s <= 0 or abs(q) >= pr * tol / 100:
            continue
        blocked = False
        for i in range(1, len(pos)):
            if i == hit or pos[i][2] != 1:
                continue
            fx, fy = pos[i][0] / 16777216 - bx, pos[i][1] / 16777216 - by
            a = fx * ox + fy * oy
            b = fx * oy - fy * ox
            if 0 < a < s and abs(b) < 2 * rb:
                blocked = True
                break
        if not blocked:
            return True
    return False


def nms_choice(turn):
    for k, c in enumerate(turn['cands']):
        if c[3]:
            return k
    return 60


def fast_choice(turn, judged, budget):
    trials = 0
    for k, c in enumerate(turn['cands']):
        if judged[k]:
            trials += 1
            if c[3]:
                return k, trials
    extra = 0
    for k, c in enumerate(turn['cands']):
        if judged[k] or extra >= budget:
            continue
        extra += 1
        trials += 1
        if c[3]:
            return k, trials
    return 60, trials


def angle_of(turn, k):
    return turn['cands'][k][0] if k < 45 else turn['ghost']


def pots(turn, k):
    return k < 45 and turn['cands'][k][3] == 1


def evaluate(turns, pockets, radii, tol, budget, quiet=False):
    same = agree = recall_hits = recall_n = 0
    trials = []
    for t in turns:
        judged = [judge(t['pos'], c[1], c[2], pockets, radii, tol) for c in t['cands']]
        ke = nms_choice(t)
        kf, n = fast_choice(t, judged, budget)
        trials.append(n)
        same += angle_of(t, ke) == angle_of(t, kf)
        agree += pots(t, ke) == pots(t, kf)
        for k, c in enumerate(t['cands']):
            if c[3]:
                recall_n += 1
                recall_hits += judged[k]
    n = len(turns)
    trials.sort()
    return (f'tol {tol:4d} budget {budget:2d}: same shot {100 * same / n:5.1f}%  pot agreement {100 * agree / n:5.1f}%  '
            f'judge recall {100 * recall_hits / max(1, recall_n):5.1f}% of {recall_n} potting candidates  '
            f'full trials median {statistics.median(trials)} p90 {trials[int(0.9 * (n - 1))]} max {trials[-1]}')


def capped(turn, judged, cap, budget):
    """The capped search: NMS's order; a judged candidate is played out in
    full, the others for at most cap passes; the search stops after budget
    passes in all (the ghost angle then).  Returns (k, passes)."""
    used = 0
    for k, c in enumerate(turn['cands']):
        n = c[5] if judged[k] else min(c[5], cap)
        if used + n > budget:
            return 60, budget
        used += n
        if c[3] and c[5] <= (c[5] if judged[k] else cap):
            return k, used
    return 60, used


def exact_cost(turn):
    used = 0
    for k, c in enumerate(turn['cands']):
        used += c[5]
        if c[3]:
            return used
    return used


def evaluate_capped(turns, pockets, radii, tol, cap, budget, judges):
    same = agree = 0
    cost, ecost, est = [], [], []
    for i, t in enumerate(turns):
        judged = judges[i]
        ke = nms_choice(t)
        kf, n = capped(t, judged, cap, budget)
        same += angle_of(t, ke) == angle_of(t, kf)
        agree += pots(t, ke) == pots(t, kf)
        ec = exact_cost(t)
        cost.append(n); ecost.append(ec)
        est.append(t['ex_t'] * n / ec if ec else 0)
    n = len(turns)
    q = lambda v: (sorted(v)[int(0.5 * (n - 1))], sorted(v)[int(0.9 * (n - 1))], max(v))
    return (f'tol {tol} cap {cap:3d} budget {budget:5d}: same shot {100 * same / n:5.1f}%  pot agreement {100 * agree / n:5.1f}%  '
            f'passes median/p90/max {q(cost)} (NMS {q(ecost)})  est. VBlanks median/p90/max {tuple(round(x) for x in q(est))}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dir')
    ap.add_argument('--tol', type=int, default=150)
    ap.add_argument('--budget', type=int, default=0)
    ap.add_argument('--sweep', action='store_true')
    ap.add_argument('--capped', action='store_true')
    a = ap.parse_args()
    turns, pockets, radii = load(a.dir)
    print(f'corpus: {len(turns)} turns, {sum(1 for t in turns if nms_choice(t) < 60)} where a candidate pots, '
          f'{sum(sum(c[3] for c in t["cands"]) for t in turns)} potting candidates of {60 * len(turns)}')
    if a.capped:
        judges = [[judge(t['pos'], c[1], c[2], pockets, radii, a.tol) for c in t['cands']] for t in turns]
        rate = sum(t['ex_t'] for t in turns) / max(1, sum(exact_cost(t) for t in turns))
        print(f'VBlanks per pass over the corpus: {rate:.3f}')
        for cap in (30, 50, 75, 100, 120, 100000):
            for budget in (1500, 2500, 4000, 100000):
                print(evaluate_capped(turns, pockets, radii, a.tol, cap, budget, judges))
        return
    if a.sweep:
        for tol in (100, 125, 150, 200, 250, 300):
            for budget in (0, 2, 4, 8):
                print(evaluate(turns, pockets, radii, tol, budget))
    else:
        print(evaluate(turns, pockets, radii, a.tol, a.budget))


if __name__ == '__main__':
    sys.exit(main())


def hybrid(turn, judged, cap, budget):
    """Judged candidates first, in NMS's order, played out in full; then
    the others in NMS's order for at most cap passes each; at most budget
    passes in all.  Returns (k, passes)."""
    used = 0
    for k, c in enumerate(turn['cands']):
        if not judged[k]:
            continue
        if used + c[5] > budget:
            return 60, budget
        used += c[5]
        if c[3]:
            return k, used
    for k, c in enumerate(turn['cands']):
        if judged[k]:
            continue
        n = min(c[5], cap)
        if used + n > budget:
            return 60, budget
        used += n
        if c[3] and c[5] <= cap:
            return k, used
    return 60, used


def sweep_hybrid(d):
    turns, pockets, radii = load(d)
    rate = sum(t['ex_t'] for t in turns) / max(1, sum(exact_cost(t) for t in turns))
    for tol in (150, 250):
        judges = [[judge(t['pos'], c[1], c[2], pockets, radii, tol) for c in t['cands']] for t in turns]
        for cap in (30, 50, 75, 100):
            for budget in (1500, 2500, 3500, 5000):
                same = agree = 0
                cost = []
                for i, t in enumerate(turns):
                    ke = nms_choice(t)
                    kf, n = hybrid(t, judges[i], cap, budget)
                    same += angle_of(t, ke) == angle_of(t, kf)
                    agree += pots(t, ke) == pots(t, kf)
                    cost.append(n)
                cost.sort(); n = len(turns)
                print(f'hybrid tol {tol} cap {cap:3d} budget {budget:5d}: same shot {100 * same / n:5.1f}%  pot agreement {100 * agree / n:5.1f}%  '
                      f'passes median {cost[n // 2]} p90 {cost[int(0.9 * (n - 1))]} max {cost[-1]}  (~{rate:.3f} VBlank/pass)')


if __name__ == '__main__' and len(sys.argv) > 2 and sys.argv[2] == '--hybrid':
    sweep_hybrid(sys.argv[1])


def F(v):
    return v / 16777216


def first_on_ray(pos, sx, sy, ux, uy, exclude, rad):
    best = None
    for i in range(1, len(pos)):
        if i in exclude or pos[i][2] != 1:
            continue
        dx, dy = F(pos[i][0]) - sx, F(pos[i][1]) - sy
        t = dx * ux + dy * uy
        p = dx * uy - dy * ux
        if t > 0 and abs(p) < rad and (best is None or t < best[0]):
            best = (t, i, p)
    return best


def to_pocket(pos, b, ox, oy, pockets, rb, tol, exclude):
    bx, by = F(pos[b][0]), F(pos[b][1])
    for px, py, pr in pockets:
        ex, ey = px - bx, py - by
        s = ex * ox + ey * oy
        q = ex * oy - ey * ox
        if s <= 0 or abs(q) >= pr * tol / 100:
            continue
        blocked = False
        for i in range(1, len(pos)):
            if i == b or i in exclude or pos[i][2] != 1:
                continue
            fx, fy = F(pos[i][0]) - bx, F(pos[i][1]) - by
            a = fx * ox + fy * oy
            if 0 < a < s and abs(fx * oy - fy * ox) < 2 * rb:
                blocked = True
                break
        if not blocked:
            return True
    return False


def judge2(pos, vx, vy, pockets, radii, tol, combo=True, cannon=True):
    """judge, plus a combination (the first ball on the struck ball's line,
    sent on through the centres at contact) and the white going on along the
    tangent to the next ball."""
    rw, rb = radii
    R = rw + rb
    wx, wy = F(pos[0][0]), F(pos[0][1])
    v = math.hypot(vx, vy)
    if v == 0:
        return False
    ux, uy = vx / v, vy / v
    fh = first_on_ray(pos, wx, wy, ux, uy, {0}, R)
    if fh is None:
        return False
    t, h, p = fh
    back = math.sqrt(R * R - p * p)
    cx, cy = wx + ux * (t - back), wy + uy * (t - back)
    bx, by = F(pos[h][0]), F(pos[h][1])
    ox, oy = (bx - cx) / R, (by - cy) / R
    if to_pocket(pos, h, ox, oy, pockets, rb, tol, set()):
        return True
    if combo:
        nx = first_on_ray(pos, bx, by, ox, oy, {0, h}, 2 * rb)
        if nx:
            t2, x, p2 = nx
            back2 = math.sqrt(4 * rb * rb - p2 * p2)
            c2x, c2y = bx + ox * (t2 - back2), by + oy * (t2 - back2)
            xx, xy = F(pos[x][0]), F(pos[x][1])
            if to_pocket(pos, x, (xx - c2x) / (2 * rb), (xy - c2y) / (2 * rb), pockets, rb, tol, {h}):
                return True
    if cannon:
        dot = ux * ox + uy * oy
        tx, ty = ux - dot * ox, uy - dot * oy
        tn = math.hypot(tx, ty)
        if tn > 1e-3:
            tx, ty = tx / tn, ty / tn
            nx = first_on_ray(pos, cx, cy, tx, ty, {0, h}, R)
            if nx:
                t3, y, p3 = nx
                back3 = math.sqrt(R * R - p3 * p3)
                c3x, c3y = cx + tx * (t3 - back3), cy + ty * (t3 - back3)
                yx, yy = F(pos[y][0]), F(pos[y][1])
                if to_pocket(pos, y, (yx - c3x) / R, (yy - c3y) / R, pockets, rb, tol, {h}):
                    return True
    return False


def sweep2(d):
    turns, pockets, radii = load(d)
    for tol in (150, 200, 300):
        for combo, cannon in ((False, False), (True, False), (True, True)):
            judges = [[judge2(t['pos'], c[1], c[2], pockets, radii, tol, combo, cannon) for c in t['cands']] for t in turns]
            rec = sum(j for i, t in enumerate(turns) for k, j in enumerate(judges[i]) if t['cands'][k][3])
            npot = sum(c[3] for t in turns for c in t['cands'])
            nj = sum(sum(j) for j in judges)
            res = []
            for model, fn in (('order', capped), ('hybrid', hybrid)):
                for cap, budget in ((30, 2000), (40, 2000), (60, 3000)):
                    same = agree = 0
                    cost = []
                    for i, t in enumerate(turns):
                        ke = nms_choice(t)
                        kf, n = fn(t, judges[i], cap, budget)
                        same += angle_of(t, ke) == angle_of(t, kf)
                        agree += pots(t, ke) == pots(t, kf)
                        cost.append(n)
                    cost.sort()
                    n = len(turns)
                    res.append(f'{model} cap {cap} budget {budget}: same {100 * same / n:.1f}% pots {100 * agree / n:.1f}% passes med {cost[n // 2]} p90 {cost[int(0.9 * (n - 1))]}')
            print(f'tol {tol} combo {combo} cannon {cannon}: recall {100 * rec / npot:.1f}%, judged {nj} of {60 * len(turns)}')
            for r in res:
                print('   ', r)
