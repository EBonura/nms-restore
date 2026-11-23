#!/usr/bin/env python3
"""Record QD Up ball-physics oracles from the 1996 build (POOL.CPE).

For one shot (a --press script) this does two emulator runs in dev kit mode:
  1. a route run that watches every ball's x, y, vx, vy and moving/active
     bytes once per route tick, until the run's step budget ends;
  2. a run that stops just after the stroke (hit_ball has set the cue ball's
     velocity, no physics step yet) and dumps RAM, from which the physics
     globals, cushion and pocket tables and the ball structs are extracted.

Output, in <out>/<name>/:
  state.json   start state: globals, balls, pockets, cushion tables
               (doubles kept as their exact 64-bit patterns, plus floats)
  frames.csv   one row per route tick on which any watched value changed,
               from the stroke until the balls stop
  route.csv.gz the raw route log (watched words as unsigned ints)

Addresses come from POOL.MAP (Sep 19 1996 build).

Side effect: the 1996 build is not FINAL, so init_cushions writes
tables\\potters\\ustable.csh back over the PC link on every run.  The unpacked
copy in QDUP_DATA therefore no longer matches the archive after a run.
"""
import argparse, csv, gzip, json, os, struct, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
POOL2 = os.environ.get('QDUP_DATA', os.path.join(ROOT, 'build/originals/qdup/WORK/SONY/POOL2'))
DEVKIT_EMU = os.environ.get('PSOXIDE_EMU_DEVKIT', 'psoxide-emulator-devkit')

# POOL.MAP addresses
BALL = 0x801672F4
BALL_SIZE = 280
NBALLS = 16                      # LAST_BALL on the pool tables
SYMS = {
    'LAST_BALL': (0x8007BD24, 'l'),
    'frame_rate': (0x8007B94C, 'd'),
    'ball_mass': (0x8007C2CC, 'd'),
    'bumper_scale': (0x8007BEBC, 'd'),
    'min_cush_x1': (0x8007BB30, 'd'),
    'max_cush_y2': (0x8007BB50, 'd'),
    'min_cush_y3': (0x8007BB48, 'd'),
    'max_cush_x4': (0x8007BB58, 'd'),
    'xpoints': (0x8007BF88, 'l'),
    'ypoints': (0x8007BF8C, 'l'),
    'SNOOKER_MODE': (0x8007C2B8, 'l'),
    'game_type': (0x8007C340, 'l'),
    'activated_lines': (0x8007BA4C, 'l'),
    'calculating_computer_shot': (0x8007BBEC, 'l'),
    'speed_meter': (0x8007BC38, 'd'),
    'butt': (0x8007C334, 'l'),
    'x_hit_pos': (0x8007C3DC, 'l'),
    'y_hit_pos': (0x8007C3E0, 'l'),
    'cue_weight': (0x8007B9B8, 'd'),
    'ball_hits_pos': (0x8007C160, 'h'),
    'cue_angle': (0x8007C278, 'l'),
    'q_factor': (0x8007B46C, 'd'),
    'r_factor': (0x8007B474, 'd'),
}
FRICTION = 0x800101F0            # const double friction[6][2]
ANGLE_Z_FRICTION = 0x80010250    # const double angle_z_friction[8]
GENERAL_RULES = 0x8011368C       # General_Rules[]: LOCATION [25], TABLESIZE_TYPE [27], GAME_TYPE [28]
POCKETS = 0x80105E88             # struct pockets_st { double x, y, radius; } [6]
CUSH = {                         # struct cush_st { double x, y, cush_ang; }
    'cushions_y1': (0x8009FE40, 1000),
    'cushions_x2': (0x800A5C00, 2000),
    'cushions_x3': (0x800B1780, 2000),
    'cushions_y4': (0x800BD300, 1000),
}
# ballstruct offsets (GLOBALS.H)
B_LONGS = ['graphic_type', 'obj_type', 'ball_colour', 'foul_flag', 'hitcush',
           'numstruct', 'respot_flag', 'num', 'pocket_ball_went_down']
B_DOUBLES = ['vx1', 'vy1', 'vz1', 'x', 'y', 'z', 'dist_x', 'dist_y', 'dist_z',
             'x_prev', 'y_prev', 'z_prev', 'vx', 'vy', 'vz', 'ax', 'ay', 'az',
             'old_ax', 'old_ay', 'old_az', 'avx', 'avy', 'avz',
             'diameter_squared', 'diameter', 'radius', 'mass', 'mi']
OFF = {n: 40 + 8 * i for i, n in enumerate(B_DOUBLES)}
OFF_MOVING = 272
WATCH = ['x', 'y', 'vx', 'vy']   # doubles watched per ball, plus the moving/active word


def watch_addrs():
    out = []
    for b in range(NBALLS):
        base = BALL + BALL_SIZE * b
        for n in WATCH:
            out += [base + OFF[n], base + OFF[n] + 4]
        out.append(base + OFF_MOVING)
    return out


def u2d(lo, hi):
    return struct.unpack('<d', struct.pack('<II', lo, hi))[0]


def run_emu(args, log):
    with open(log, 'w') as f:
        subprocess.run([DEVKIT_EMU, 'launch', '--path', os.path.join(POOL2, 'POOL.CPE'),
                        '--digital-pad'] + args, stdout=f, stderr=subprocess.STDOUT, check=True)


def read_route(path):
    rows = []
    with open(path) as f:
        r = csv.reader(f)
        hdr = next(r)
        first = hdr.index('ram_%08x' % BALL) if ('ram_%08x' % BALL) in hdr else None
        wa = watch_addrs()
        first = hdr.index('ram_%08x' % wa[0])
        for row in r:
            rows.append(row)
    return hdr, first, rows


def decode_row(row, first):
    vals = [int(v) for v in row[first:first + len(watch_addrs())]]
    balls = []
    k = 0
    for b in range(NBALLS):
        rec = {}
        for n in WATCH:
            rec[n] = u2d(vals[k], vals[k + 1]); k += 2
        w = vals[k]; k += 1
        rec['moving'] = w & 0xff
        rec['active'] = (w >> 8) & 0xff
        balls.append(rec)
    return vals, balls


def extract_state(dump):
    m = open(dump, 'rb').read()
    def at(a):
        return a - 0x80000000
    def rd(a, t):
        if t == 'd':
            return struct.unpack_from('<d', m, at(a))[0]
        if t == 'l':
            return struct.unpack_from('<i', m, at(a))[0]
        if t == 'h':
            return struct.unpack_from('<h', m, at(a))[0]
    def dbits(a):
        return '%016x' % struct.unpack_from('<Q', m, at(a))[0]
    st = {'globals': {}, 'globals_bits': {}}
    for k, (a, t) in SYMS.items():
        st['globals'][k] = rd(a, t)
        if t == 'd':
            st['globals_bits'][k] = dbits(a)
    st['friction'] = [[dbits(FRICTION + 16 * i), dbits(FRICTION + 16 * i + 8)] for i in range(6)]
    st['angle_z_friction'] = [dbits(ANGLE_Z_FRICTION + 8 * i) for i in range(8)]
    gr = [rd(GENERAL_RULES + 4 * i, 'l') for i in range(40)]
    st['globals']['LOCATION'] = gr[25]
    st['globals']['TABLESIZE_TYPE'] = gr[27]
    st['globals']['GAME_TYPE'] = gr[28]
    st['pockets'] = []
    for p in range(6):
        a = POCKETS + 24 * p
        st['pockets'].append({'x': dbits(a), 'y': dbits(a + 8), 'radius': dbits(a + 16)})
    st['cushions'] = {}
    for name, (a, n) in CUSH.items():
        st['cushions'][name] = [[dbits(a + 24 * i), dbits(a + 24 * i + 8), dbits(a + 24 * i + 16)]
                                for i in range(n)]
    st['balls'] = []
    for b in range(NBALLS):
        base = BALL + BALL_SIZE * b
        rec = {n: rd(base + 4 * i, 'l') for i, n in enumerate(B_LONGS)}
        for n in B_DOUBLES:
            rec[n] = dbits(base + OFF[n])
        rec['moving'] = m[at(base + OFF_MOVING)]
        rec['active'] = m[at(base + OFF_MOVING + 1)]
        st['balls'].append(rec)
    return st


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('name')
    ap.add_argument('--press', required=True)
    ap.add_argument('--steps', type=int, default=9000000000)
    ap.add_argument('--out', default=os.path.join(ROOT, 'build/oracle'))
    a = ap.parse_args()
    od = os.path.join(a.out, a.name)
    os.makedirs(od, exist_ok=True)
    route = os.path.join(od, 'route.csv')
    wargs = []
    for w in watch_addrs():
        wargs += ['--route-watch-u32', '0x%08x' % w]
    run_emu(['--steps', str(a.steps), '--press', a.press, '--route-log', route] + wargs,
            os.path.join(od, 'route-run.log'))
    hdr, first, rows = read_route(route)
    ti = hdr.index('route_tick'); pi = hdr.index('port1_polls')
    # stroke: first tick where the cue ball is moving
    start = None
    for i, row in enumerate(rows):
        _, balls = decode_row(row, first)
        if balls[0]['moving'] and balls[0]['active'] == 1 and (balls[0]['vx'] or balls[0]['vy']):
            start = i
            break
    if start is None:
        sys.exit('no stroke found')
    # first physics change after the stroke
    v0, _ = decode_row(rows[start], first)
    phys = start
    for i in range(start + 1, len(rows)):
        v, _ = decode_row(rows[i], first)
        if v != v0:
            phys = i
            break
    poll = int(rows[start][pi]) + 1
    print('stroke tick', rows[start][ti], 'first physics tick', rows[phys][ti], 'stop poll', poll)
    dump = os.path.join(od, 'start.ram')
    run_emu(['--steps', str(a.steps), '--press', a.press, '--stop-at-poll', str(poll),
             '--dump-ram', dump], os.path.join(od, 'dump-run.log'))
    st = extract_state(dump)
    os.remove(dump)
    # the dump must hold the stroke state, before any physics step
    vs, bs = decode_row(rows[start], first)
    for b in range(NBALLS):
        for n in WATCH:
            if '%016x' % struct.unpack('<Q', struct.pack('<d', bs[b][n]))[0] != st['balls'][b][n]:
                sys.exit('dump does not match stroke state (ball %d %s)' % (b, n))
    st['stroke_tick'] = int(rows[start][ti])
    st['first_physics_tick'] = int(rows[phys][ti])
    st['press'] = a.press
    with open(os.path.join(od, 'state.json'), 'w') as f:
        json.dump(st, f)
    # frames: ticks where anything watched changed, from the stroke on
    with open(os.path.join(od, 'frames.csv'), 'w', newline='') as f:
        w = csv.writer(f)
        h = ['tick', 'cpu_tick_delta', 'flip']
        for b in range(NBALLS):
            h += ['b%d_%s' % (b, n) for n in WATCH + ['moving', 'active']]
        w.writerow(h)
        prev = None
        ci = hdr.index('cpu_tick_delta'); fi = hdr.index('display_start_changed')
        last_moving = start
        for i in range(start, len(rows)):
            v, bs = decode_row(rows[i], first)
            if any(b['moving'] and b['active'] for b in bs):
                last_moving = i
            if v != prev:
                out = [rows[i][ti], rows[i][ci], rows[i][fi]]
                for b in bs:
                    out += [repr(b[n]) for n in WATCH] + [b['moving'], b['active']]
                w.writerow(out)
                prev = v
            if i > last_moving + 200:
                break
    # keep the raw route log compressed, rows from 100 ticks before the stroke
    with open(route) as f, gzip.open(route + '.gz', 'wt') as g:
        for k, line in enumerate(f):
            if k == 0 or k - 1 >= start - 100:
                g.write(line)
    os.remove(route)
    print('wrote', od)


if __name__ == '__main__':
    main()
