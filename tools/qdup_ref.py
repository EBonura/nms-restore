#!/usr/bin/env python3
"""Host reference of QD Up's 1996 ball physics (BALL.C, doubles).

A line-by-line transcription of update_all_balls, get_max_time_step,
update_ball_movement, check_cushions_*, get_cush_*, cush_rebound_*,
check_all_pockets_*, pocketed_ball (the physics part) and ball_collision2,
run on an oracle start state (state.json from qdup_oracle.py).  Python floats
are IEEE doubles; the 1996 build used SN soft-float and Sony libmath, so
sin/cos/atan2/sqrt may differ in the last bits.

Used to check that the oracle is understood (which samples are call
boundaries) and as a double reference for experiments (--clamp).

  qdup_ref.py STATE.json [--calls N] [--clamp] [--tables text] [--substeps]
prints one line per update_all_balls call (or per sub-step).
"""
import argparse, json, math, os, struct, sys

PI = 3.1415927                      # POOL.C: PI=3.1415927
PIx = 3.1415927
NUM_CUSH_POINTS_X = 2000
NUM_CUSH_POINTS_Y = 1000
MINIMUM_SPEED_SQUARED = 0.0004
FRIC_SPEED_SCALE = 10000.0
NUM_FRIC_VALS = 6
NUM_FRICTION_VALS_Z = 8
FRIC_SPEED_SCALE_Z = 1
CUSH_SPIN_SCALE = 0.005
cush_loss = 0.95
cush_loss_perp = 0.95
ang_cush_loss = 0.1
LOCATION_LT_DIFFERENT = True        # LOCATION (0) < DIFFERENT_SHAPED_TABLES_INDEX (4)


def h2d(h):
    return struct.unpack('<d', struct.pack('<Q', int(h, 16)))[0]


def ddiv(a, b):
    """IEEE division (inf/nan on zero divisor) like the soft-float library."""
    try:
        return a / b
    except ZeroDivisionError:
        if a == 0 or a != a:
            return float('nan')
        return math.copysign(float('inf'), a) * math.copysign(1.0, b)


def trunc_int(v):
    """C double -> int conversion (truncate toward zero)."""
    if v != v or v in (float('inf'), float('-inf')):
        return -2**31
    return int(v)


class Ball:
    pass


def text_tables(path):
    """Parse a table's cushion text file (the .bmp named one) correctly."""
    names = {1: 'cushions_y1', 2: 'cushions_x2', 3: 'cushions_x3', 4: 'cushions_y4'}
    t = {n: None for n in names.values()}
    cur = None
    with open(path, 'rb') as f:
        toks = f.read().decode('latin-1').split()
    i = 0
    sizes = {'cushions_y1': 1000, 'cushions_x2': 2000, 'cushions_x3': 2000, 'cushions_y4': 1000}
    for n in sizes:
        t[n] = [[0.0, 0.0, 0.0] for _ in range(sizes[n])]
    while i < len(toks):
        if toks[i] == 'bumper:':
            cur = names[int(toks[i + 1])]; i += 2; continue
        if toks[i] == 'point:':
            p = int(toks[i + 1])
            t[cur][p] = [float(toks[i + 2]), float(toks[i + 3]), float(toks[i + 4])]
            i += 5; continue
        i += 1
    return t


class Sim:
    def __init__(self, st, clamp=False, tables=None):
        g = st['globals']
        self.LAST_BALL = g['LAST_BALL']
        self.bumper_scale = g['bumper_scale']
        self.min_cush_x1 = g['min_cush_x1']
        self.max_cush_y2 = g['max_cush_y2']
        self.min_cush_y3 = g['min_cush_y3']
        self.max_cush_x4 = g['max_cush_x4']
        self.frame_rate = g['frame_rate']
        self.friction = [[h2d(a), h2d(b)] for a, b in st['friction']]
        self.angle_z_friction = [h2d(a) for a in st['angle_z_friction']]
        self.pockets = [{k: h2d(v) for k, v in p.items()} for p in st['pockets']]
        if tables is None:
            self.cush = {n: [[h2d(v) for v in e] for e in tab] for n, tab in st['cushions'].items()}
        else:
            self.cush = tables
        self.balls = []
        for rec in st['balls']:
            b = Ball()
            for k, v in rec.items():
                setattr(b, k, h2d(v) if isinstance(v, str) else v)
            self.balls.append(b)
        self.clamp = clamp
        self.prev_time_step = 0.0
        self.ball_hits_pos = 0
        self.ball_hits = []
        self.check_angle_count = 0
        self.substep_log = None
        self.events = []

    # ---------------------------------------------------------------- BALL.C
    def update_ball_movement(self, time_step, n):
        b = self.balls[n]
        if b.active == 2:
            b.z -= 0.000005
            b.moving = 1
            if b.z < -b.diameter:
                b.active = 0
                b.moving = 0
            return
        if (not b.moving) or (not b.active) or b.active == 2:
            return
        b.x_prev = b.x; b.y_prev = b.y; b.z_prev = b.z
        total_velocity = b.vx * b.vx + b.vy * b.vy
        if total_velocity < MINIMUM_SPEED_SQUARED:
            if abs(b.avz) < 0.03:
                b.moving = 0
                b.vx = b.vy = b.vz = 0.0
                b.avx = b.avy = b.avz = 0.0
                return
        fric_pos = trunc_int(total_velocity * FRIC_SPEED_SCALE)
        if fric_pos >= NUM_FRIC_VALS:
            fric_pos = NUM_FRIC_VALS - 1
        f = self.friction[fric_pos]
        qx = -(f[0] * (b.vx - (b.avx * b.radius)))
        qy = -(f[0] * (b.vy - (b.avy * b.radius)))
        rx = -(f[1] * b.vx)
        ry = -(f[1] * b.vy)
        a_x = ddiv(qx + rx, b.mass)
        a_y = ddiv(qy + ry, b.mass)
        alpha_x = ddiv(b.radius * (2 * rx - 10 * qx), 10 * b.mi)
        alpha_y = ddiv(b.radius * (2 * ry - 10 * qy), 10 * b.mi)
        b.avx = (alpha_x * time_step) + b.avx
        b.avy = (alpha_y * time_step) + b.avy
        b.vx += a_x * time_step
        b.vy += a_y * time_step
        b.dist_x += abs(time_step * b.vx)
        b.dist_y += abs(time_step * b.vy)
        b.x += time_step * b.vx
        b.ax += time_step * b.avx
        b.y += time_step * b.vy
        b.ay += time_step * b.avy
        fric_pos = trunc_int(abs(b.avz) * FRIC_SPEED_SCALE_Z)
        if fric_pos >= NUM_FRICTION_VALS_Z:
            fric_pos = NUM_FRICTION_VALS_Z - 1
        b.avz -= time_step * self.angle_z_friction[fric_pos] * b.avz
        b.az += time_step * b.avz
        self.check_angle_count += 1
        if self.check_angle_count >= 4:
            self.check_angle_count = 0   # angle wrap only affects ax/ay/az

    def pocketed_ball(self, n, pock):
        b = self.balls[n]
        if b.active == 0 or b.active == 2:
            return
        b.active = 0
        self.events.append(('pocket', n, pock, b.x, b.y))

    def array_pos(self, v, npts):
        p = trunc_int(v / self.bumper_scale) + npts // 2
        if p < 0 or p >= npts:
            p = 0
        return p

    def get_cush_ang(self, bump, x, y):
        if bump == 1:
            return self.cush['cushions_y1'][self.array_pos(y, NUM_CUSH_POINTS_Y)][2]
        if bump == 2:
            return self.cush['cushions_x2'][self.array_pos(x, NUM_CUSH_POINTS_X)][2]
        if bump == 3:
            return self.cush['cushions_x3'][self.array_pos(x, NUM_CUSH_POINTS_X)][2]
        return self.cush['cushions_y4'][self.array_pos(y, NUM_CUSH_POINTS_Y)][2]

    def get_cush_x(self, bump, x, y):
        if bump == 1:
            return self.cush['cushions_y1'][self.array_pos(y, NUM_CUSH_POINTS_Y)][0]
        if bump == 4:
            return self.cush['cushions_y4'][self.array_pos(y, NUM_CUSH_POINTS_Y)][0]
        return 10.0

    def get_cush_y(self, bump, x, y):
        if bump == 2:
            return self.cush['cushions_x2'][self.array_pos(x, NUM_CUSH_POINTS_X)][1]
        if bump == 3:
            return self.cush['cushions_x3'][self.array_pos(x, NUM_CUSH_POINTS_X)][1]
        return 10.0

    def extra(self, diff, v, cur):
        e = abs(ddiv(diff, v))
        if self.clamp and not (e <= cur):
            e = cur
        return e

    def hit_cushion(self, n, kind, cush, cur, vbefore):
        b = self.balls[n]
        self.events.append(('cush', n, kind, b.x, b.y, vbefore, math.hypot(b.vx, b.vy)))

    def check_cushions_float(self, n, cur):
        b = self.balls[n]
        if not b.moving or not b.active or b.active == 2:
            return
        self.check_cushions_x1(n, cur)
        self.check_cushions_x4(n, cur)
        self.check_cushions_y2(n, cur)
        self.check_cushions_y3(n, cur)

    def check_cushions_y2(self, n, cur):
        b = self.balls[n]
        if (self.max_cush_y2 - b.y) < 0.0:
            cush_y = self.get_cush_y(3, b.x, 0.0)
            self.check_all_pockets_y2(n)
            if b.y > cush_y:
                v0 = math.hypot(b.vx, b.vy)
                y_diff = cush_y - b.y
                e = self.extra(y_diff, b.vy, cur)
                b.y = cush_y
                self.cush_rebound_y(n, self.get_cush_ang(3, b.x, 0.0), 3)
                self.update_ball_movement(e, n)
                b.hitcush += 1
                self.hit_cushion(n, 'y2', cush_y, cur, v0)

    def check_cushions_x1(self, n, cur):
        b = self.balls[n]
        if (self.min_cush_x1 - b.x) > 0.0:
            cush_x = self.get_cush_x(1, 0.0, b.y)
            if cush_x == -10 or cush_x == 10:
                self.check_all_pockets_x1(n)
            if b.x < cush_x:
                v0 = math.hypot(b.vx, b.vy)
                x_diff = b.x - cush_x
                e = self.extra(x_diff, b.vx, cur)
                b.x = cush_x
                self.cush_rebound_x(n, self.get_cush_ang(1, 0.0, b.y), 1)
                self.update_ball_movement(e, n)
                b.hitcush += 1
                self.hit_cushion(n, 'x1', cush_x, cur, v0)

    def check_cushions_x4(self, n, cur):
        b = self.balls[n]
        if (self.max_cush_x4 - b.x) < 0.0:
            cush_x = self.get_cush_x(4, 0.0, b.y)
            if cush_x == -10 or cush_x == 10:
                self.check_all_pockets_x4(n)
            if b.x > cush_x:
                v0 = math.hypot(b.vx, b.vy)
                x_diff = b.x - cush_x
                e = self.extra(x_diff, b.vx, cur)
                b.x = cush_x
                self.cush_rebound_x(n, self.get_cush_ang(4, 0.0, b.y), 4)
                self.update_ball_movement(e, n)
                b.hitcush += 1
                self.hit_cushion(n, 'x4', cush_x, cur, v0)

    def check_cushions_y3(self, n, cur):
        b = self.balls[n]
        if (self.min_cush_y3 - b.y) > 0.0:
            cush_y = self.get_cush_y(2, b.x, 0.0)
            self.check_all_pockets_y3(n)
            if b.y < cush_y:
                v0 = math.hypot(b.vx, b.vy)
                y_diff = b.y - cush_y
                e = self.extra(y_diff, b.vy, cur)
                b.y = cush_y
                self.cush_rebound_y(n, self.get_cush_ang(2, b.x, 0.0), 2)
                self.update_ball_movement(e, n)
                b.hitcush += 1
                self.hit_cushion(n, 'y3', cush_y, cur, v0)

    def cush_rebound_x(self, n, angle, cush_num):
        b = self.balls[n]
        scale = CUSH_SPIN_SCALE
        angle = -angle
        angle /= 180
        angle *= PI
        if cush_num == 4:
            scale = -scale
        temp_vx = -((b.vx * math.cos(angle)) + (b.vy * math.sin(angle)))
        temp_vy = (-(b.vx * math.sin(angle)) + (b.vy * math.cos(angle)))
        b.vx = (temp_vx * math.cos(-angle)) + (temp_vy * math.sin(-angle))
        b.vy = (-temp_vx * math.sin(-angle)) + (temp_vy * math.cos(-angle))
        ang_vel = b.avz * scale
        spin_ang = angle + PI / 2
        spin_vx = ang_vel * math.cos(spin_ang)
        spin_vy = ang_vel * math.sin(spin_ang)
        b.vx = b.vx * cush_loss
        b.vy = b.vy * cush_loss_perp
        b.avx = b.avx * ang_cush_loss
        b.avy = b.avy * ang_cush_loss
        b.avz = 0.0
        b.vx += spin_vx
        b.vy += spin_vy

    def cush_rebound_y(self, n, angle, cush_num):
        b = self.balls[n]
        scale = CUSH_SPIN_SCALE
        angle -= 90
        angle /= 180
        angle *= PI
        if cush_num == 2:
            scale = -scale
        ang_vel = b.avz * scale
        spin_ang = angle + PI / 2
        spin_vx = ang_vel * math.cos(spin_ang)
        spin_vy = ang_vel * math.sin(spin_ang)
        temp_vx = ((b.vx * math.cos(angle)) + (b.vy * math.sin(angle)))
        temp_vy = -(-(b.vx * math.sin(angle)) + (b.vy * math.cos(angle)))
        b.vx = (temp_vx * math.cos(-angle)) + (temp_vy * math.sin(-angle))
        b.vy = (-temp_vx * math.sin(-angle)) + (temp_vy * math.cos(-angle))
        b.vx = b.vx * cush_loss
        b.vy = b.vy * cush_loss_perp
        b.avx = b.avx * ang_cush_loss
        b.avy = b.avy * ang_cush_loss
        b.avz = 0.0
        b.vx += spin_vx
        b.vy += spin_vy

    def hyp(self, n, p):
        b = self.balls[n]
        P = self.pockets[p]
        x1 = abs(P['x'] - b.x)
        y1 = abs(P['y'] - b.y)
        return math.sqrt(x1 * x1 + y1 * y1)

    def check_all_pockets_x1(self, n):
        b = self.balls[n]; P = self.pockets
        h = self.hyp(n, 0)
        if b.x < P[0]['x'] or b.y > P[0]['y']:
            self.pocketed_ball(n, 3)
        if h <= P[0]['radius']:
            self.pocketed_ball(n, 0); return
        h = self.hyp(n, 1)
        if b.x < P[1]['x'] or b.y < P[1]['y']:
            self.pocketed_ball(n, 3)
        if h <= P[1]['radius']:
            self.pocketed_ball(n, 1); return

    def check_all_pockets_x4(self, n):
        b = self.balls[n]; P = self.pockets
        if b.x > P[3]['x'] or b.y < P[3]['y']:
            self.pocketed_ball(n, 3)
        h = self.hyp(n, 4)
        if b.x > P[4]['x'] or b.y > P[4]['y']:
            self.pocketed_ball(n, 3)
        if h <= P[4]['radius']:
            self.pocketed_ball(n, 4); return
        h = self.hyp(n, 3)
        if h <= P[3]['radius']:
            self.pocketed_ball(n, 3); return

    def check_all_pockets_y2(self, n):
        b = self.balls[n]; P = self.pockets
        if LOCATION_LT_DIFFERENT:
            if b.y > P[5]['y']:
                self.pocketed_ball(n, 3)
            if b.x > P[4]['x'] or b.y > P[4]['y']:
                self.pocketed_ball(n, 3)
            if b.x < P[0]['x'] or b.y > P[0]['y']:
                self.pocketed_ball(n, 3)
        if self.hyp(n, 0) <= P[0]['radius'] / 4:
            self.pocketed_ball(n, 0); return
        if self.hyp(n, 5) <= P[5]['radius']:
            self.pocketed_ball(n, 5); return
        if self.hyp(n, 4) <= P[4]['radius']:
            self.pocketed_ball(n, 4); return

    def check_all_pockets_y3(self, n):
        b = self.balls[n]; P = self.pockets
        for p in range(1, 4):
            if LOCATION_LT_DIFFERENT:
                if p == 1 and (b.x < P[p]['x'] or b.y < P[p]['y']):
                    self.pocketed_ball(n, 3)
                if p == 2 and b.y < P[p]['y']:
                    self.pocketed_ball(n, 3)
                if p == 3 and (b.x > P[p]['x'] or b.y < P[p]['y']):
                    self.pocketed_ball(n, 3)
            if self.hyp(n, p) <= P[p]['radius']:
                self.pocketed_ball(n, p); return

    def get_max_time_step(self, time_step):
        max_time_step = time_step / 2
        min_time_step = time_step / 4
        L = self.LAST_BALL
        hyp = [[0.0] * L for _ in range(L)]
        self.ball_hits_pos = 0
        self.ball_hits = []
        min_ball_dist = 1e99
        num = 0
        B = self.balls
        for l1 in range(L):
            if B[l1].moving and B[l1].active:
                for l2 in range(L):
                    if l1 != l2 and hyp[l1][l2] == 0.0 and B[l2].active:
                        dx = B[l2].x - B[l1].x
                        dy = B[l2].y - B[l1].y
                        h = dx * dx + dy * dy
                        hyp[l1][l2] = h
                        hyp[l2][l1] = h
                        if h < min_ball_dist:
                            min_ball_dist = h
                            num = l1
                        if h < B[l1].diameter_squared:
                            self.ball_hits.append((l1, l2))
                            self.ball_hits_pos += 1
                            if self.ball_hits_pos == L:
                                self.ball_hits_pos -= 1
                                self.ball_hits.pop()
        vel = math.sqrt(B[num].vx * B[num].vx + B[num].vy * B[num].vy)
        new_time_step = ddiv(abs(math.sqrt(min_ball_dist) - B[num].diameter), vel * 2)
        while new_time_step < max_time_step and max_time_step > min_time_step:
            max_time_step /= 2
        return max_time_step

    def ball_collision2(self, b1, b2):
        B = self.balls
        for k in (b1, b2):
            if B[k].active == 2 or not B[k].active:
                return
        collision_ang = math.atan2(B[b2].x - B[b1].x, B[b2].y - B[b1].y)
        ball1_ang = math.atan2(B[b1].vx, B[b1].vy)
        s = math.sin(collision_ang); c = math.cos(collision_ang)
        vel1 = B[b1].vx * s + B[b1].vy * c
        vel2 = B[b2].vx * s + B[b2].vy * c
        diff = vel2 - vel1
        perp1 = -B[b1].vy * s + B[b1].vx * c
        perp2 = -B[b2].vy * s + B[b2].vx * c
        m1 = vel1 * B[b1].mass
        m2 = vel2 * B[b2].mass
        tot = m1 + m2
        new2 = ddiv(tot - (B[b2].mass * 0.90 * diff), B[b1].mass + B[b2].mass)
        new1 = (0.90 * diff) + new2
        z_spin = (collision_ang - ball1_ang) * diff * 2.0
        B[b1].vx = new1 * s + perp1 * c
        B[b1].vy = new1 * c - perp1 * s
        B[b1].avz += z_spin
        B[b1].moving = 1
        B[b2].vx = new2 * s + perp2 * c
        B[b2].vy = new2 * c - perp2 * s
        B[b2].avz -= z_spin
        B[b2].moving = 1
        self.events.append(('ballhit', b1, b2))

    def update_all_balls(self, time_step):
        time_elapsed = 0.0
        while (time_elapsed + 0.00000001) < time_step:
            cur = self.get_max_time_step(time_step)
            if self.ball_hits_pos != 0:
                for b in self.balls[:self.LAST_BALL]:
                    b.x = b.x_prev
                    b.y = b.y_prev
                time_elapsed -= self.prev_time_step
            for (b1, b2) in self.ball_hits:
                self.ball_collision2(b1, b2)
            for n in range(self.LAST_BALL):
                self.update_ball_movement(cur, n)
                self.check_cushions_float(n, cur)
            time_elapsed += cur
            self.prev_time_step = cur
            if self.substep_log is not None:
                self.substep_log(cur)

    def any_balls_moving(self):
        return any(b.moving and b.active > 0 for b in self.balls[:self.LAST_BALL])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('state')
    ap.add_argument('--calls', type=int, default=100000)
    ap.add_argument('--clamp', action='store_true', help='clamp extra_time_step to the sub-step')
    ap.add_argument('--tables', default='ram', help="'ram' (as the 1996 build had them) or a text cushion file")
    ap.add_argument('--substeps', action='store_true')
    a = ap.parse_args()
    st = json.load(open(a.state))
    tables = None if a.tables == 'ram' else text_tables(a.tables)
    sim = Sim(st, clamp=a.clamp, tables=tables)
    dt = 1.0 / sim.frame_rate
    out = sys.stdout
    def dump(tag):
        parts = [tag]
        for i, b in enumerate(sim.balls[:sim.LAST_BALL]):
            parts.append('%d:%r,%r,%r,%r,%d,%d' % (i, b.x, b.y, b.vx, b.vy, b.moving, b.active))
        out.write(' '.join(parts) + '\n')
    call = [0]; sub = [0]
    if a.substeps:
        sim.substep_log = lambda cur: (sub.__setitem__(0, sub[0] + 1), dump('S%d.%d' % (call[0], sub[0])))
    dump('C0')
    while call[0] < a.calls and sim.any_balls_moving():
        call[0] += 1; sub[0] = 0
        sim.update_all_balls(dt)
        dump('C%d' % call[0])
    for e in sim.events:
        out.write('E ' + ' '.join(str(v) for v in e) + '\n')


if __name__ == '__main__':
    main()
