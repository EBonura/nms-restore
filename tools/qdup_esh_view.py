#!/usr/bin/env python3
"""Render a still of QD Up's 3D table from NMS's Escher data, as a reference
for the PS1 3D renderer (escher/).  Not NMS code: NMS's Escher renderer
does not survive (on the PS1 every render call is a stub in ENGINE.C, and
PINDOS.EXE in the archive is a DOS pinball game, not QD Up), so this is a
plain z-buffer rasteriser over the data NMS's loader reads, with the game's
own set-up applied.

    qdup_esh_view.py OUT.png [--data DIR] [--detail low|high] [--scale N]
                     [--rot X Y Z] [--target X Y Z] [--camz N] [--cue]

DIR is the 1996 POOL2 folder (default: the unpacked P_200996 copy).
The default camera and balls are the game's state at the start of a frame
on the table (read from a RAM dump of the disc build at the table): camera
rotation (256, 1025, 0), target (-217601, 7295, 0), camz 3504 (distance
camz*256), the UK-coloured 8-ball rack and the cue ball on its spot.

Formats (from ESCHER.S's loader E_Load, ESCHER.H and the files):

ESH (mesh), little-endian 32-bit ints:
  int num_objects
  per object:
    char name[16]; int radius; VERTEX source_pos (x, y, z)
    int num_vertices; VERTEX vertex[num_vertices]        (object-local)
    int first_extra_vertex
    int num_lines;  LINE  line[num_lines]                (56 bytes each)
    int num_planes; PLANE plane[num_planes]              (48 bytes each)
    int num_plane_vertices; short plane_vertex[n]        (vertex indices)
  LINE  = vertex[2], plane[2] (-1 none), orientation[2], start_u[2],
          start_v[2], end_u[2], end_v[2]
  PLANE = char axis (0 x, 1 y, 2 z: the plane's dominant axis),
          char plane_type (0 plain, 1 texture, 2 chrome, 3 tiled),
          char visible_side (2: seen from the + side of `axis`, 1: from
          the - side), char current_side; int colour (palette index,
          repeated in all four bytes), brightness (0..31), tile_info,
          gradient[2], offset (the plane equation along `axis`), radius,
          centre_vertex, priority, vertex_start (into plane_vertex),
          num_vertices.  A plane is a convex polygon.
  World units: 256000 to the metre (the game's FIX24_TO_WORLD); y is up.

EMP (map data, E_LoadMapData; one per ESH, same base name):
  int num_objects
  per object: char name[16]; int num_lines, num_planes; int priority;
              int colour[num_planes]; int brightness[num_planes];
              int tile_info[num_planes]; int uv[num_lines][8];
              char plane_type[num_planes]
  Applied to the ESH object of the same name when the line and plane counts
  agree.  POTTERUS.EMP matches POTTERUS.ESH object for object (all planes
  plain, baked brightness).  BALLSCUE.EMP (May) is older than BALLSCUE.ESH
  (July) and holds running debug colours; the game sets the ball colours
  itself (GetBallPtrs), and the cue's colours are not in the data.

Colour: palette[BRIGHT.TAB[brightness*256 + colour]], palette POTTBLUE.PAL
(VGA 6-bit), the table LEVELDAT.C pairs with Potters; ball colours from the
ramps at POOLPAL (BLUEPAL.TAB) + 2560, as STV3D.C's GetBallPtrs.

Rotation (my reading of STV3D.C's PutCamera and PositionCue, 4096 to a
turn): v' = Ry(y) Rx(x) Rz(z) v, Rx raising -z toward +y.  The camera sits
at target + R (0, 0, -distance) and looks along R (0, 0, 1).  Projection:
the game's own set-up (STV3D.C setup_resolution, low resolution):
E_SetScreenSize(320, 200), E_SetScreenCentre(160, 145),
E_SetPixelAspect(54613), E_SetZoom(zoom * 65536 / 320) with zoom 1128, read
as a focal length of zoom/65536 * 160 = 564 pixels (half the screen width).
That reading makes the game's fixed cameras frame what they are for: the
overhead camera (camz 4974) puts the 2.6 m table across 295 of the 320
pixels, the corner cameras (camz 6000) show all of it; with the full width
(1128 pixels) they would each show part of the table.  On the PS1's 320x256
PAL screen the same 4:3 picture has the centre at y 145*256/200 and a
vertical scale of 54613/65536 * 256/200 (16/15).  It is a reading: Escher's
projection code is lost.
"""
import argparse
import math
import os
import struct

import numpy as np
from PIL import Image

DATA = os.environ.get('QDUP_DATA', os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'build/originals/qdup/WORK/SONY/POOL2'))

W, H = 320, 256
CX, CY = 160, 145 * 256 / 200
FOCAL = 564
ASPECT = 54613 / 65536 * 256 / 200

# the opening rack, ball[i] (x, y) in metres from the disc build at the table;
# colours: 0 white, 1 yellow, 3 red, 8 black
RACK = [(-0.8500, 0.0, 0), (0.6200, 0.0, 1), (0.6713, 0.0296, 3), (0.7227, 0.0593, 1),
        (0.7740, 0.0889, 3), (0.8253, 0.1186, 1), (0.8253, 0.0593, 1), (0.8253, 0.0, 3),
        (0.7227, 0.0, 8), (0.8253, -0.1186, 3), (0.7740, -0.0889, 1), (0.7227, -0.0593, 3),
        (0.6713, -0.0296, 1), (0.8253, -0.0593, 1), (0.7740, 0.0296, 1), (0.7740, -0.0296, 3)]
BALL_R = 0.0285
COL16 = {0: 128, 1: 7 * 16, 2: 16, 3: 4 * 16, 4: 5 * 16, 5: 3 * 16, 6: 2 * 16, 7: 6 * 16, 8: 0}


def read_esh(path):
    b = open(path, 'rb').read()
    o = 0

    def rd(fmt):
        nonlocal o
        v = struct.unpack_from(fmt, b, o)
        o += struct.calcsize(fmt)
        return v
    n, = rd('<i')
    objs = []
    for _ in range(n):
        name = b[o:o + 16].split(b'\0')[0].decode('latin1')
        o += 16
        radius, = rd('<i')
        pos = rd('<3i')
        nv, = rd('<i')
        verts = [rd('<3i') for _ in range(nv)]
        rd('<i')
        nl, = rd('<i')
        lines = [rd('<14i')[:4] for _ in range(nl)]
        npl, = rd('<i')
        planes = []
        for _ in range(npl):
            p = rd('<4B11i')
            planes.append(dict(axis=p[0], type=p[1], side=p[2], colour=p[4] & 255, bright=p[5],
                               start=p[13], count=p[14]))
        npv, = rd('<i')
        pv = rd('<%dh' % npv)
        for k, pl in enumerate(planes):
            pl['loop'] = boundary(lines, k)
        objs.append(dict(name=name, radius=radius, pos=pos, verts=np.array(verts, float),
                         nl=nl, planes=planes, pv=pv, pri=0x10000000, enabled=False))
    assert o == len(b), path
    return objs


def boundary(lines, k):
    """A plane's outline in order: plane_vertex lists the plane's vertices but
    not always around the edge (160 of the 702 Potters planes are out of
    order), so the outline is chained from the LINEs that border the plane."""
    adj = {}
    for a, b, p0, p1 in lines:
        if k in (p0, p1):
            adj.setdefault(a, []).append(b)
            adj.setdefault(b, []).append(a)
    if not adj:
        return []
    start = next(iter(adj))
    loop, prev, cur = [start], None, start
    while True:
        nxt = [v for v in adj[cur] if v != prev]
        if not nxt or nxt[0] == start or len(loop) > len(adj):
            break
        loop.append(nxt[0])
        prev, cur = cur, nxt[0]
    return loop


def ear_clip(pts):
    """Triangles (index triples) of a simple polygon given in 2D; the
    number glyphs and a few cabinet sides are concave."""
    n = len(pts)
    area = sum(pts[i][0] * pts[(i + 1) % n][1] - pts[(i + 1) % n][0] * pts[i][1] for i in range(n))
    idx = list(range(n)) if area > 0 else list(range(n))[::-1]
    tris = []

    def cross(a, b, c):
        return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])
    guard = 0
    while len(idx) > 3 and guard < 4 * n:
        guard += 1
        for k in range(len(idx)):
            i0, i1, i2 = idx[k - 1], idx[k], idx[(k + 1) % len(idx)]
            a, b, c = pts[i0], pts[i1], pts[i2]
            if cross(a, b, c) <= 0:
                continue
            if any(cross(a, b, pts[j]) > 0 and cross(b, c, pts[j]) > 0 and cross(c, a, pts[j]) > 0
                   for j in idx if j not in (i0, i1, i2)):
                continue
            tris.append((i0, i1, i2))
            idx.pop(k)
            break
        else:
            break
    if len(idx) >= 3:
        tris += [(idx[0], idx[k], idx[k + 1]) for k in range(1, len(idx) - 1)]
    return tris


def apply_emp(path, objs):
    b = open(path, 'rb').read()
    n, = struct.unpack_from('<i', b, 0)
    o = 4
    by = {ob['name']: ob for ob in objs}
    for _ in range(n):
        name = b[o:o + 16].split(b'\0')[0].decode('latin1')
        nl, npl, pri = struct.unpack_from('<3i', b, o + 16)
        p = o + 28
        col = struct.unpack_from('<%di' % npl, b, p)
        bri = struct.unpack_from('<%di' % npl, b, p + 4 * npl)
        typ = b[p + 12 * npl + 32 * nl:p + 12 * npl + 32 * nl + npl]
        o = p + 13 * npl + 32 * nl
        ob = by.get(name)
        if ob and ob['nl'] == nl and len(ob['planes']) == npl:
            ob['pri'] = pri
            for pl, c, br, t in zip(ob['planes'], col, bri, typ):
                pl.update(colour=c & 255, bright=br, type=t)


def rotation(x, y, z):
    a, b, c = (v * 2 * math.pi / 4096 for v in (x, y, z))
    rx = np.array([[1, 0, 0], [0, math.cos(a), -math.sin(a)], [0, math.sin(a), math.cos(a)]])
    ry = np.array([[math.cos(b), 0, math.sin(b)], [0, 1, 0], [-math.sin(b), 0, math.cos(b)]])
    rz = np.array([[math.cos(c), -math.sin(c), 0], [math.sin(c), math.cos(c), 0], [0, 0, 1]])
    return ry @ rx @ rz


def game_setup(objs, pal, bright, pool, detail, cue):
    """init_3d / GetBallPtrs / GetBumperPtrs / setup_detail_level_of_room for
    the Potters US table, 8-ball with UK colours (the game's start)."""
    by = {ob['name']: ob for ob in objs}

    def colour(name, c, pri=None):
        ob = by.get(name)
        if ob:
            for pl in ob['planes']:
                pl.update(colour=c, type=0)
            if pri is not None:
                ob['pri'] = pri
    colour('outerbox', pool[2560], 96 << 24)
    for name in [str(i) for i in range(1, 16)]:
        colour(name, pool[2560])
    colour('cuespot', pool[2560 + 16])
    for i in range(15):
        colour('spot%02d' % i, pool[2560 + 15])
    by['surface']['pri'] = 15 << 24
    for i in range(1, 7):
        by['pocket%d' % i]['pri'] = 16 << 24
        by['bumper%d' % i]['pri'] = 14 << 24
        by['corner%d' % i]['pri'] = 15 << 24
    for n in ('endleft', 'endbottom', 'endtop', 'endright'):
        by[n]['pri'] = 13 << 24
    for i in range(1, 19):
        by['diamond%d' % i]['pri'] = 4 << 24
    by['cue']['pri'] = 1 << 24
    # the cue: BALLSCUE.EMP has only debug colours for it; wood from the
    # table's own palette (the leg colours), by facing
    for k, pl in enumerate(by['cue']['planes']):
        pl.update(colour=232, bright=31, type=0)
    # ball meshes are scaled to the US ball (init_3d, E_ScaleObject 73534)
    for ob in objs:
        if ob['name'].startswith(('spot', 'stripe', 'cuespot')) or ob['name'].isdigit():
            ob['verts'] *= 73534 / 65536
    # which objects show
    if detail == 'high':
        for ob in objs:
            ob['enabled'] = not ob['name'].startswith('outer')
    else:
        for ob in objs:
            ob['enabled'] = ob['name'].startswith(('outer', 'top', 'bott', 'leg0', 'surface', 'corner'))
    for ob in objs:
        n = ob['name']
        if n.startswith(('pocket', 'surface', 'diamond', 'bumper', 'end')):
            ob['enabled'] = True
        if n.startswith(('stripe', 'spot', 'dummy', 'ldummy', 'cross', 'cuespot')) or n.isdigit() or n == 'cue':
            ob['enabled'] = False
    by['cue']['enabled'] = cue
    by['cuespot']['enabled'] = cue


def place(ob, pos, rot=(0, 0, 0)):
    ob['world'] = np.asarray(pos, float)
    ob['rot'] = rotation(*rot)


class Raster:
    def __init__(self, scale):
        self.s = scale
        self.img = np.zeros((H * scale, W * scale, 3), np.uint8)
        self.z = np.full((H * scale, W * scale), np.inf)

    def tri(self, p, rgb, bias=0.0):
        """p: 3 x (sx, sy, z) in screen pixels."""
        s = self.s
        x = p[:, 0] * s
        y = p[:, 1] * s
        x0, x1 = max(int(math.floor(x.min())), 0), min(int(math.ceil(x.max())), W * s - 1)
        y0, y1 = max(int(math.floor(y.min())), 0), min(int(math.ceil(y.max())), H * s - 1)
        if x0 > x1 or y0 > y1:
            return
        den = (y[1] - y[2]) * (x[0] - x[2]) + (x[2] - x[1]) * (y[0] - y[2])
        if abs(den) < 1e-9:
            return
        gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        l0 = ((y[1] - y[2]) * (gx - x[2]) + (x[2] - x[1]) * (gy - y[2])) / den
        l1 = ((y[2] - y[0]) * (gx - x[2]) + (x[0] - x[2]) * (gy - y[2])) / den
        l2 = 1 - l0 - l1
        inside = (l0 >= 0) & (l1 >= 0) & (l2 >= 0)
        iz = l0 / p[0, 2] + l1 / p[1, 2] + l2 / p[2, 2]
        z = 1 / np.maximum(iz, 1e-12) - bias
        zb = self.z[y0:y1 + 1, x0:x1 + 1]
        m = inside & (z < zb)
        zb[m] = z[m]
        self.img[y0:y1 + 1, x0:x1 + 1][m] = rgb

    def disc(self, cx, cy, r, z, colour_fn, squish=1.0):
        s = self.s
        x0, x1 = max(int((cx - r) * s), 0), min(int((cx + r) * s) + 1, W * s - 1)
        y0, y1 = max(int((cy - r * squish) * s), 0), min(int((cy + r * squish) * s) + 1, H * s - 1)
        if x0 > x1 or y0 > y1:
            return
        gx, gy = np.meshgrid((np.arange(x0, x1 + 1) + 0.5) / s, (np.arange(y0, y1 + 1) + 0.5) / s)
        dx, dy = (gx - cx) / r, (gy - cy) / (r * squish)
        d2 = dx * dx + dy * dy
        m = (d2 <= 1) & (z < self.z[y0:y1 + 1, x0:x1 + 1])
        if m.any():
            self.img[y0:y1 + 1, x0:x1 + 1][m] = colour_fn(dx[m], dy[m])
            self.z[y0:y1 + 1, x0:x1 + 1][m] = z


def render(objs, cam_rot, target, dist, pal, bright, pool, scale, balls):
    R = rotation(*cam_rot)
    cam = np.asarray(target, float) + R @ np.array([0, 0, -dist])
    view = R.T
    ras = Raster(scale)
    near = 2048.0

    def project(pc):
        return np.array([CX + FOCAL * pc[0] / pc[2], CY - FOCAL * ASPECT * pc[1] / pc[2], pc[2]])

    for ob in objs:
        if not ob['enabled']:
            continue
        rot = ob.get('rot', np.eye(3))
        world = ob.get('world', np.asarray(ob['pos'], float))
        pw = (rot @ ob['verts'].T).T + world
        pc = (view @ (pw - cam).T).T
        for pl in ob['planes']:
            idx = pl['loop']
            if len(idx) < 3:
                continue
            poly_w = pw[list(idx)]
            # facing: the plane's visible side along its axis, against the camera
            nrm = np.zeros(3)
            for i in range(len(poly_w)):       # Newell's normal
                a, b = poly_w[i], poly_w[(i + 1) % len(poly_w)]
                nrm += np.array([(a[1] - b[1]) * (a[2] + b[2]), (a[2] - b[2]) * (a[0] + b[0]),
                                 (a[0] - b[0]) * (a[1] + b[1])])
            want = 1 if pl['side'] == 2 else -1
            if nrm[pl['axis']] * want < 0:
                nrm = -nrm
            if np.dot(nrm, cam - poly_w.mean(0)) <= 0:
                continue
            if 'tris' not in pl:
                u, w = [i for i in range(3) if i != pl['axis']]
                pl['tris'] = ear_clip([(ob['verts'][i][u], ob['verts'][i][w]) for i in idx])
            rgb = pal[bright[pl['bright'] * 256 + pl['colour']]]
            for tri in pl['tris']:
                poly = [pc[idx[i]] for i in tri]
                # clip at the near plane
                out = []
                for i in range(3):
                    a, b = poly[i], poly[(i + 1) % 3]
                    if a[2] >= near:
                        out.append(a)
                    if (a[2] >= near) != (b[2] >= near):
                        t = (near - a[2]) / (b[2] - a[2])
                        out.append(a + (b - a) * t)
                sp = [project(v) for v in out]
                for i in range(1, len(sp) - 1):
                    ras.tri(np.array([sp[0], sp[i], sp[i + 1]]), rgb)

    # balls: a shaded disc and its shadow, as STV3D.C's DrawAllBalls draws
    # them with E_DrawBall (ramp POOLPAL+2560+col: 8..15 base to white)
    pitch = cam_rot[0] * 2 * math.pi / 4096
    for i, (bx, by, col) in enumerate(balls):
        w = np.array([bx * 256000, BALL_R * 256000, by * 256000])
        c = view @ (w - cam)
        if c[2] < near:
            continue
        sx, sy = CX + FOCAL * c[0] / c[2], CY - FOCAL * ASPECT * c[1] / c[2]
        r = FOCAL * BALL_R * 256000 / c[2]
        base = COL16[col]
        ramp = [pal[pool[2560 + base + 8 + k]] for k in range(8)]
        shadow = pal[pool[2560 + 144]]
        ras.disc(sx, sy + r * math.cos(pitch), r * 0.75, c[2] + BALL_R * 256000,
                 lambda dx, dy: shadow, squish=math.sin(pitch))

        def shade(dx, dy, ramp=ramp):
            # light from the upper left: ramp index by distance from the highlight
            d = np.sqrt((dx + 0.35) ** 2 + (dy + 0.4) ** 2)
            k = np.clip(7 - (d * 7.5).astype(int), 0, 7)
            return np.array(ramp)[k]
        ras.disc(sx, sy, r, c[2] - BALL_R * 256000 * 0.5, shade)
    return ras.img


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('out')
    ap.add_argument('--data', default=DATA)
    ap.add_argument('--detail', choices=['low', 'high'], default='low')
    ap.add_argument('--scale', type=int, default=1)
    ap.add_argument('--rot', type=int, nargs=3, default=[256, 1025, 0])
    ap.add_argument('--target', type=int, nargs=3, default=[-217601, 7295, 0])
    ap.add_argument('--camz', type=int, default=3504)
    ap.add_argument('--cue', action='store_true', help='draw the cue behind the cue ball')
    a = ap.parse_args()
    t = os.path.join(a.data, 'TABLES', 'POTTERS')
    raw = open(os.path.join(t, 'POTTBLUE.PAL'), 'rb').read()
    pal = np.array([[min(255, raw[i * 3 + k] * 4 + raw[i * 3 + k] // 16) for k in range(3)] for i in range(256)], np.uint8)
    bright = open(os.path.join(t, 'BRIGHT.TAB'), 'rb').read()
    pool = open(os.path.join(t, 'BLUEPAL.TAB'), 'rb').read()
    objs = []
    for mesh in ('POTTERUS', 'BALLSCUE'):
        o = read_esh(os.path.join(t, mesh + '.ESH'))
        apply_emp(os.path.join(t, mesh + '.EMP'), o)
        objs += o
    game_setup(objs, pal, bright, pool, a.detail, a.cue)
    by = {ob['name']: ob for ob in objs}
    if a.cue:
        # PositionCue: the cue's tip a little behind the cue ball, along the view
        cb = np.array([RACK[0][0] * 256000, BALL_R * 256000, RACK[0][1] * 256000])
        butt = 33
        rot = (butt, a.rot[1], 0)
        place(by['cue'], cb + rotation(*rot) @ np.array([0, 0, -0.016 * 256000]), rot)
        place(by['cuespot'], cb, rot)
    img = render(objs, a.rot, a.target, a.camz * 256, pal, bright, pool, a.scale, RACK)
    Image.fromarray(img).save(a.out)
    print('wrote', a.out)


if __name__ == '__main__':
    main()
