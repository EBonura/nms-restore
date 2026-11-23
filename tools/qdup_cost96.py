#!/usr/bin/env python3
"""Estimate what the 1996 build's double-precision physics costs per call.

  qdup_cost96.py SHOT T0 T1

Replays the shot's --press script in dev kit mode twice with an exact
per-instruction histogram (--pc-line-log):
  shot window  route ticks T0..T1 while balls move;
  idle window  the ~100 ticks before the stroke (aiming, no physics).
Instructions are attributed by POOL.MAP: BALL.OBJ (pocketed_ball ..
check_solid_pocket) and the soft-float/libmath block (__adddf3 .. sqrt).
Other game code also calls soft-float (DrawScreen's projections), so the
idle window's soft-float per drawn frame is subtracted once per physics
call (each update_all_balls ends with one DrawScreen).  Calls in the window
come from fitting the oracle rows to the double reference.  Cycles use the
window's measured cycles per instruction.
"""
import bisect, csv, gzip, json, os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import qdup_compare as qc   # noqa: E402
import qdup_oracle as qo    # noqa: E402

ORACLE = os.path.join(qo.NMS, 'work/oracle')
BALL = (0x80017A48, 0x80024A18)
FLOAT = (0x8005ECB8, 0x800618BC)


def run(press, extra, log):
    with open(log, 'w') as f:
        subprocess.run([qo.DEVKIT_EMU, 'launch', '--path', os.path.join(qo.POOL2, 'POOL.CPE'),
                        '--digital-pad', '--steps', '9000000000', '--press', press] + extra,
                       stdout=f, stderr=subprocess.STDOUT, check=True)


def histo(path):
    tot = ball = flt = 0
    for r in csv.DictReader(open(path)):
        pc = int(r['pc'], 16)
        n = int(r['instructions'])
        tot += n
        if BALL[0] <= pc < BALL[1]:
            ball += n
        elif FLOAT[0] <= pc < FLOAT[1]:
            flt += n
    return tot, ball, flt


def window(route_rows, t0, t1, key):
    return sum(int(r[key]) for r in route_rows if t0 <= int(r['route_tick']) < t1)


def main():
    shot, t0, t1 = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    od = os.path.join(ORACLE, shot)
    cd = os.path.join(ORACLE, 'cost')
    os.makedirs(cd, exist_ok=True)
    st = json.load(open(os.path.join(od, 'state.json')))
    press = st['press']
    route = list(csv.DictReader(gzip.open(os.path.join(od, 'route.csv.gz'), 'rt')))
    poll = {int(r['route_tick']): int(r['port1_polls']) for r in route}
    i0, i1 = st['stroke_tick'] - 100, st['stroke_tick'] - 1
    pfx = os.path.join(cd, shot)
    run(press, ['--pc-line-log', pfx + '_shot_pcl.csv', '--pc-log-words',
                '--pc-line-start-route-tick', str(t0), '--stop-at-poll', str(poll[t1]),
                '--route-log', pfx + '_shot_route.csv'], pfx + '_shot.log')
    run(press, ['--pc-line-log', pfx + '_idle_pcl.csv', '--pc-log-words',
                '--pc-line-start-route-tick', str(i0), '--stop-at-poll', str(poll[i1]),
                '--route-log', pfx + '_idle_route.csv'], pfx + '_idle.log')
    sr = list(csv.DictReader(open(pfx + '_shot_route.csv')))
    ir = list(csv.DictReader(open(pfx + '_idle_route.csv')))
    s_end = int(sr[-1]['route_tick']) + 1
    i_end = int(ir[-1]['route_tick']) + 1
    stot, sball, sflt = histo(pfx + '_shot_pcl.csv')
    itot, iball, iflt = histo(pfx + '_idle_pcl.csv')
    iframes = window(ir, i0, i_end, 'display_start_changed')
    s_cyc = window(sr, t0, s_end, 'bus_cycle_delta')
    s_ins = window(sr, t0, s_end, 'cpu_tick_delta')
    calls, subs, _ = qc.run_ref(st, None, False)
    rows = list(csv.DictReader(open(os.path.join(od, 'frames.csv'))))
    ends = sorted(set(c for t, c, _ in qc.fit_oracle(rows, subs) if t0 <= t < s_end))
    if len(ends) < 2:
        sys.exit('no call boundaries in the window')
    ncalls = ends[-1] - ends[0] + 1
    cpi = s_cyc / max(1, s_ins)
    phys = (sball + sflt - iflt / max(1, iframes) * ncalls) / ncalls
    moving = [sum(1 for b in calls[c] if b[4] and b[5] == 1) for c in range(ends[0], ends[-1] + 1)]
    print('%s ticks %d..%d: %d calls (%d..%d), %.1f balls moving on average' %
          (shot, t0, s_end, ncalls, ends[0], ends[-1], sum(moving) / len(moving)))
    print('  window: %d instructions, BALL.OBJ %d, soft-float+libmath %d, %.2f cycles/instruction'
          % (stot, sball, sflt, cpi))
    print('  idle: %d frames, soft-float+libmath %d per frame' % (iframes, iflt / max(1, iframes)))
    print('  physics per update_all_balls call: %.0f instructions, about %.0f cycles (%.1f vblanks)'
          % (phys, phys * cpi, phys * cpi / (33868800 / 59.94)))


if __name__ == '__main__':
    main()
