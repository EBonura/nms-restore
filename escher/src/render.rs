//! Drawing: NMS's objects through the GTE into the game's ordering table.
//!
//! Escher on the PC was a painter: each E_RenderToBuffer call drew the
//! enabled objects over what the buffer held, objects by their priority
//! (higher first) and planes by depth, and STV3D.C's draw_3d orders its
//! calls so that the picture comes out right (room and far cushions, then
//! each ball far to near, then the near cushions and the cue). Here each
//! call takes the next band of ordering-table slots below the last one, so
//! the GPU draws the calls in the order the game made them; inside a band
//! the slots run by priority, then by depth.
//!
//! World units are Escher's (256000 to the metre). The GTE works in units
//! of 128 of them (2000 to the metre): a room 12 m long stays inside its
//! 16-bit inputs and 16-bit depth.

use crate::camera::camera;
use crate::fixed::{mul12, rotation, transpose, Mat};
use crate::mesh::{plane_entry, PLANE_CONCAVE, PLANE_SKIP};
use crate::nms::{object_list, palette, Object, Vertex};
use crate::pool::{self, Screen};
use psx_gte::{read_control, read_data, write_control, write_data};

/// World units per GTE unit, as a shift.
const GTE_SHIFT: i32 = 7;
/// Ordering-table slots kept for the game's own 2D (DrawScreen's sprites,
/// AddPrim(o_t++) from slot 0 up).
const HUD_SLOTS: usize = 64;
/// Camera-space depth (GTE units) below which a polygon is clipped.
const NEAR: i32 = 32;
/// The PS1 picture: 320x256 PAL.
const PSX_W: i32 = 320;
const PSX_H: i32 = 256;
/// Guard band, PS1 pixels: polygons are clipped to it so that what reaches
/// the GPU is inside its coordinate range and size limits (1023 x 511).
const GUARD_L: i32 = -200;
const GUARD_R: i32 = 520;
const GUARD_T: i32 = -120;
const GUARD_B: i32 = 380;

/// Escher's screen set-up (E_SetScreenSize, E_SetScreenCentre,
/// E_SetPixelAspect, E_SetZoom): the PC picture the game thinks in.
pub struct ScreenSetup {
    pub w: i32,
    pub h: i32,
    pub cx: i32,
    pub cy: i32,
    pub aspect: i32,
    pub zoom: i32,
}

pub static mut SCREEN: ScreenSetup = ScreenSetup { w: 320, h: 200, cx: 160, cy: 145, aspect: 54613, zoom: 231014 };

pub fn screen() -> &'static mut ScreenSetup {
    // SAFETY: foreground-only static.
    unsafe { &mut *(&raw mut SCREEN) }
}

impl ScreenSetup {
    /// Focal length in PC pixels: E_SetZoom's value is zoom/320 in 16.16
    /// (STV3D.C setup_resolution), read here as times half the screen
    /// width: 564 pixels at 320 wide for the game's zoom of 1128. That is the
    /// reading under which the game's fixed cameras frame what they are for:
    /// the overhead camera (4974*256 units up) puts the 2.6 m table across
    /// 295 of the 320 pixels, the corner cameras (6 m) show the whole table,
    /// the computer's camera the whole length of it. (Twice that, the zoom
    /// value times the full width, would leave each of them showing part of
    /// the table.) Escher's own projection code is lost.
    pub fn focal(&self) -> i32 {
        ((self.zoom >> 4) * (self.w / 2)) >> 12
    }
}

/// Per-frame state: the ordering table the game gave (E_SetOrderingTable),
/// the next free slot from the top, and this page's primitive buffer.
pub struct Frame {
    ot: *mut u32,
    size: usize,
    cursor: usize,
    page: usize,
    last_ot: *mut u32,
    prim: *mut u32,
    prim_end: *mut u32,
    pub active: bool,
}

pub static mut FRAME: Frame = Frame {
    ot: core::ptr::null_mut(),
    size: 0,
    cursor: 0,
    page: 0,
    last_ot: core::ptr::null_mut(),
    prim: core::ptr::null_mut(),
    prim_end: core::ptr::null_mut(),
    active: false,
};

fn frame() -> &'static mut Frame {
    // SAFETY: foreground-only static.
    unsafe { &mut *(&raw mut FRAME) }
}

/// Counters for runs and route logs: frames begun, then for the last
/// finished frame: primitives, polygons that took the clipping path,
/// primitives dropped (all frames), ordering-table slots used, objects
/// drawn, vertices transformed, primitive bytes.
#[no_mangle]
pub static mut escher_stats: [u32; 8] = [0; 8];

fn stat(i: usize) -> &'static mut u32 {
    // SAFETY: foreground-only static.
    unsafe { &mut (*(&raw mut escher_stats))[i] }
}

/// Brightness table (E_UseBrightnessTable): 32 rows of 256 palette indices.
pub static mut BRIGHT: *const u8 = core::ptr::null();

pub fn begin_frame(ot: *mut u32, size: i32) {
    let f = frame();
    if ot.is_null() || size as usize <= HUD_SLOTS + 16 || !pool::ready() {
        f.active = false;
        return;
    }
    if ot != f.last_ot {
        f.page ^= 1;
    }
    f.last_ot = ot;
    f.ot = ot;
    f.size = size as usize;
    f.cursor = size as usize - 1;
    f.prim = pool::prims(f.page);
    // SAFETY: the page's buffer is PRIM_BYTES long.
    f.prim_end = unsafe { f.prim.add(pool::PRIM_BYTES / 4) };
    f.active = true;
    *stat(0) += 1;
    crate::balls::upload_if_needed(f.prim.cast());
}

/// The finished frame's counts into escher_stats (1..7); the running ones
/// start again.
pub fn end_stats() {
    let f = frame();
    // SAFETY: foreground-only statics.
    unsafe {
        let run = &mut *(&raw mut RUN);
        if f.active {
            *stat(1) = run[0];
            *stat(2) = run[1];
            *stat(3) += run[2];
            *stat(4) = (f.size - 1 - f.cursor) as u32;
            *stat(5) = run[3];
            *stat(6) = run[4];
            // SAFETY: both inside the page buffer.
            *stat(7) = f.prim.offset_from(pool::prims(f.page)) as u32 * 4;
        }
        *run = [0; 5];
    }
}

/// Running counts of the frame being drawn: primitives, clipped polygons,
/// dropped primitives, objects, vertices.
static mut RUN: [u32; 5] = [0; 5];

fn run(i: usize) -> &'static mut u32 {
    // SAFETY: foreground-only static.
    unsafe { &mut (*(&raw mut RUN))[i] }
}

impl Frame {
    /// Take `n` slots below the last band; returns the band's top slot.
    fn band(&mut self, n: usize) -> Option<usize> {
        if self.cursor < HUD_SLOTS + n {
            return None;
        }
        let top = self.cursor;
        self.cursor -= n;
        Some(top)
    }

    pub fn avail(&self) -> usize {
        self.cursor.saturating_sub(HUD_SLOTS)
    }

    /// Room for a primitive of `words` payload words, linked into `slot`.
    fn alloc(&mut self, slot: usize, words: usize) -> Option<*mut u32> {
        // SAFETY: inside the page buffer, checked against its end.
        unsafe {
            if self.prim.add(words + 1) > self.prim_end {
                *run(2) += 1;
                return None;
            }
            let p = self.prim;
            self.prim = p.add(words + 1);
            let e = self.ot.add(slot);
            *p = ((words as u32) << 24) | (*e & 0x00FF_FFFF);
            *e = (*e & 0xFF00_0000) | (p as u32 & 0x00FF_FFFF);
            *run(0) += 1;
            Some(p.add(1))
        }
    }
}

fn xy(x: i32, y: i32) -> u32 {
    (x as u16 as u32) | ((y as u16 as u32) << 16)
}

/// A flat polygon of n points (got from `pt`), as one F3/F4 or a fan of
/// them.
fn emit_flat(slot: usize, n: usize, pt: impl Fn(usize) -> (i32, i32), rgb: u32) {
    let f = frame();
    let p0 = pt(0);
    let mut i = 1;
    while i + 1 < n {
        let quad = i + 2 < n;
        let words = if quad { 5 } else { 4 };
        let Some(p) = f.alloc(slot, words) else { return };
        // SAFETY: `words` payload words just allocated.
        unsafe {
            *p.add(1) = xy(p0.0, p0.1);
            let a = pt(i);
            *p.add(2) = xy(a.0, a.1);
            if quad {
                // GPU quad order: v0 v1 v3 v2 (two triangles 012, 123)
                *p = psx_hw::gpu::packet::FLAT_QUAD | rgb;
                let (b, c) = (pt(i + 1), pt(i + 2));
                *p.add(3) = xy(c.0, c.1);
                *p.add(4) = xy(b.0, b.1);
            } else {
                *p = psx_hw::gpu::packet::FLAT_TRIANGLE | rgb;
                let b = pt(i + 1);
                *p.add(3) = xy(b.0, b.1);
            }
        }
        i += if quad { 2 } else { 1 };
    }
}

pub fn emit_line(slot: usize, x0: i32, y0: i32, x1: i32, y1: i32, rgb: u32) {
    let Some(p) = frame().alloc(slot, 3) else { return };
    // SAFETY: 3 payload words.
    unsafe {
        *p = psx_hw::gpu::packet::FLAT_LINE | rgb;
        *p.add(1) = xy(x0, y0);
        *p.add(2) = xy(x1, y1);
    }
}

/// A textured sprite quad (raw texture, 4-bit page) for the balls.
pub fn emit_sprite(slot: usize, x0: i32, y0: i32, x1: i32, y1: i32, clut: u32, tpage: u32, uv_max: u32) {
    let Some(p) = frame().alloc(slot, 9) else { return };
    // SAFETY: 9 payload words.
    unsafe {
        *p = 0x2D80_8080;
        *p.add(1) = xy(x0, y0);
        *p.add(2) = clut << 16;
        *p.add(3) = xy(x1, y0);
        *p.add(4) = (tpage << 16) | uv_max;
        *p.add(5) = xy(x0, y1);
        *p.add(6) = uv_max << 8;
        *p.add(7) = xy(x1, y1);
        *p.add(8) = uv_max | (uv_max << 8);
    }
}

/// One slot of its own, below everything so far (E_DrawBall).
pub fn take_slot() -> Option<usize> {
    let f = frame();
    if !f.active {
        return None;
    }
    f.band(1)
}

/// A palette index as a GPU colour word (24-bit BGR).
pub fn rgb_of(index: i32) -> u32 {
    let i = (index & 255) as usize * 3;
    // SAFETY: the game's 768-byte palette.
    let (r, g, b) = unsafe { (palette[i] as u32 & 63, palette[i + 1] as u32 & 63, palette[i + 2] as u32 & 63) };
    let s = |c: u32| (c << 2) | (c >> 4);
    s(r) | (s(g) << 8) | (s(b) << 16)
}

/// A plane's colour: palette[BRIGHT.TAB[brightness][colour]], flat as the
/// PC drew it (the brightness is baked into the EMP per plane).
fn plane_rgb(colour: i32, brightness: i32) -> u32 {
    let c = colour & 255;
    // SAFETY: the 32 x 256 table the game handed over, or none.
    let idx = unsafe {
        if BRIGHT.is_null() {
            c
        } else {
            *BRIGHT.add((brightness.clamp(0, 31) * 256 + c) as usize) as i32
        }
    };
    rgb_of(idx)
}

/// The view: camera space is x right, y up, z ahead, world units.
pub struct View {
    /// world to camera (R^T), Q12
    pub m: Mat,
    /// the GTE's rotation: m with the y row flipped and scaled to PS1 pixels
    g: Mat,
    pos: Vertex,
    /// PS1 projection: focal length, centre (pixels)
    h: i32,
    ofx: i32,
    ofy: i32,
}

static mut VIEW: Option<(Vertex, Vertex, [i32; 6], View)> = None;

/// The view for the camera as it is now (kept while camera and screen stay).
pub fn view() -> &'static View {
    let cam = camera();
    let s = screen();
    let key = [s.w, s.h, s.cx, s.cy, s.aspect, s.zoom];
    // SAFETY: foreground-only static.
    let cache = unsafe { &mut *(&raw mut VIEW) };
    let same = matches!(cache, Some((p, r, k, _)) if *k == key
        && (p.x, p.y, p.z) == (cam.pos.x, cam.pos.y, cam.pos.z)
        && (r.x, r.y, r.z) == (cam.rot.x, cam.rot.y, cam.rot.z));
    if !same {
        *cache = Some((cam.pos, cam.rot, key, make_view()));
    }
    &cache.as_ref().unwrap().3
}

fn make_view() -> View {
    let cam = camera();
    let m = transpose(&rotation(cam.rot.x, cam.rot.y, cam.rot.z));
    let s = screen();
    // PC pixels to PS1 pixels: x * 320 / w, y * 256 / h; the vertical
    // focal length is the horizontal one times the PC's pixel aspect.
    let my = ((s.aspect * PSX_H) / s.h.max(1)) * s.w / PSX_W; // 16.16
    let mut g = m;
    for j in 0..3 {
        g[1][j] = -((m[1][j] * my) >> 16);
    }
    View {
        m,
        g,
        pos: cam.pos,
        h: s.focal() * PSX_W / s.w.max(1),
        ofx: s.cx * PSX_W / s.w.max(1),
        ofy: s.cy * PSX_H / s.h.max(1),
    }
}

impl View {
    /// The bounding sphere (camera-space centre in world units, radius r in
    /// GTE units) is wholly off one side of the screen; conservative (the
    /// sphere's far depth stands for all of it).
    fn outside(&self, c: Vertex, r: i32) -> bool {
        let (x, y, z) = (c.x >> GTE_SHIFT, c.y >> GTE_SHIFT, (c.z >> GTE_SHIFT) + r);
        if x.abs() >= 1 << 16 || y.abs() >= 1 << 16 || z >= 1 << 16 || r >= 1 << 15 {
            return false;
        }
        if z <= 0 {
            return true;
        }
        let h = self.h;
        // vertical scale 17/16 (the PS1 aspect, 1.0667) rounded down
        let up = |v: i32| ((v * h) >> 4) * 17;
        (x - r) * h > (PSX_W - self.ofx) * z
            || (x + r) * h < -self.ofx * z
            || up(y - r) > self.ofy * z
            || up(y + r) < -(PSX_H - self.ofy) * z
    }

    /// Camera-space position of a world point (world units); None when it is
    /// too far to compute (pocketed balls sit 1000 m below the table).
    pub fn to_camera(&self, p: &Vertex) -> Option<Vertex> {
        let d = Vertex { x: p.x - self.pos.x, y: p.y - self.pos.y, z: p.z - self.pos.z };
        if d.x.abs() >= 1 << 26 || d.y.abs() >= 1 << 26 || d.z.abs() >= 1 << 26 {
            return None;
        }
        Some(crate::fixed::apply(&self.m, &d))
    }

    fn load_gte(&self) {
        write_control!(24, (self.ofx << 16) as u32);
        write_control!(25, (self.ofy << 16) as u32);
        write_control!(26, self.h.clamp(1, 65535) as u32);
        let g = &self.g;
        let p = |a: i32, b: i32| (a as u16 as u32) | ((b as u16 as u32) << 16);
        write_control!(0, p(g[0][0], g[0][1]));
        write_control!(1, p(g[0][2], g[1][0]));
        write_control!(2, p(g[1][1], g[1][2]));
        write_control!(3, p(g[2][0], g[2][1]));
        write_control!(4, g[2][2] as u32);
    }

    /// Put the object's origin in the GTE's translation; false when it is
    /// out of range.
    fn load_object(&self, obj: &Object) -> bool {
        let p = &obj.modified_pos;
        let d = [p.x - self.pos.x, p.y - self.pos.y, p.z - self.pos.z];
        if d.iter().any(|v| v.abs() >= 1 << 26) {
            return false;
        }
        let d = [d[0] >> GTE_SHIFT, d[1] >> GTE_SHIFT, d[2] >> GTE_SHIFT];
        let g = &self.g;
        for (i, reg) in [5u32, 6, 7].iter().enumerate() {
            let t = mul12(g[i][0], d[0]) + mul12(g[i][1], d[1]) + mul12(g[i][2], d[2]);
            match reg {
                5 => write_control!(5, t as u32),
                6 => write_control!(6, t as u32),
                _ => write_control!(7, t as u32),
            }
        }
        true
    }
}

/// Transform an object's vertices to the screen (scratch list), three at a
/// time (RTPT). A triple the GTE flags (behind the camera, saturated) is
/// marked for the clipping path, which asks the GTE for camera space then.
fn transform(obj: &Object) -> usize {
    let verts = obj.vertices();
    let n = verts.len().min(pool::MAX_VERTS);
    let scr = pool::scratch();
    if obj.radius >= (32767 << GTE_SHIFT) / 2 && verts.iter().take(n).any(|v| !fits(v)) {
        for s in scr.iter_mut().take(n) {
            *s = Screen { x: 0, y: 0, z: 0, ok: 0 };
        }
        return 0;
    }
    let pk = |v: &Vertex| ((v.x >> GTE_SHIFT) as u16 as u32) | (((v.y >> GTE_SHIFT) as u16 as u32) << 16);
    let pz = |v: &Vertex| (v.z >> GTE_SHIFT) as u32;
    let mut i = 0;
    while i + 3 <= n {
        let (a, b, c) = (&verts[i], &verts[i + 1], &verts[i + 2]);
        write_data!(0, pk(a));
        write_data!(1, pz(a));
        write_data!(2, pk(b));
        write_data!(3, pz(b));
        write_data!(4, pk(c));
        write_data!(5, pz(c));
        // SAFETY: V0..V2 loaded; RT/TR/OFX/OFY/H set by the pass.
        unsafe { psx_gte::ops::project_triple() };
        let (s0, s1, s2) = (read_data!(12), read_data!(13), read_data!(14));
        let (z0, z1, z2) = (read_data!(17), read_data!(18), read_data!(19));
        let ok = u16::from(read_control!(31) & 0x8000_0000 == 0);
        scr[i] = Screen { x: s0 as i16, y: (s0 >> 16) as i16, z: z0 as u16, ok };
        scr[i + 1] = Screen { x: s1 as i16, y: (s1 >> 16) as i16, z: z1 as u16, ok };
        scr[i + 2] = Screen { x: s2 as i16, y: (s2 >> 16) as i16, z: z2 as u16, ok };
        i += 3;
    }
    while i < n {
        let v = &verts[i];
        write_data!(0, pk(v));
        write_data!(1, pz(v));
        // SAFETY: as above.
        unsafe { psx_gte::ops::project_single() };
        let s = read_data!(14);
        let z = read_data!(19);
        let ok = u16::from(read_control!(31) & 0x8000_0000 == 0);
        scr[i] = Screen { x: s as i16, y: (s >> 16) as i16, z: z as u16, ok };
        i += 1;
    }
    *run(4) += n as u32;
    n
}

fn fits(v: &Vertex) -> bool {
    let lim = 32767 << GTE_SHIFT;
    v.x.abs() < lim && v.y.abs() < lim && v.z.abs() < lim
}

/// A vertex in camera space (GTE units, y as the GTE's), from the object's
/// rotation and translation still loaded in the GTE (MVMVA).
fn camera_space(v: &Vertex) -> [i32; 3] {
    write_data!(0, ((v.x >> GTE_SHIFT) as u16 as u32) | (((v.y >> GTE_SHIFT) as u16 as u32) << 16));
    write_data!(1, (v.z >> GTE_SHIFT) as u32);
    // SAFETY: V0 loaded; RT and TR are the object's.
    unsafe { psx_gte::ops::rotate_translate_v0() };
    [read_data!(25) as i32, read_data!(26) as i32, read_data!(27) as i32]
}

/// Where a pass puts things: the top slot of each priority rank, slots per
/// rank, and the depth range mapped onto them.
struct Bands {
    top: usize,
    per: usize,
    zmin: i32,
    shift: i32,
    prio: [i32; 48],
    ranks: usize,
}

impl Bands {
    /// The top slot of a priority's rank.
    fn rank_top(&self, priority: i32) -> usize {
        let r = self.prio[..self.ranks].iter().position(|&p| p == priority).unwrap_or(self.ranks - 1);
        self.top - r * self.per
    }

    /// The slot for a depth (GTE units) in the rank whose top is `top`.
    fn slot(&self, top: usize, depth: i32) -> usize {
        let b = (((depth - self.zmin) >> self.shift).clamp(0, self.per as i32 - 1)) as usize;
        top - (self.per - 1 - b)
    }
}

/// E_RenderToBuffer / E_RenderToScreen: draw every enabled object (and the
/// queued 2D lines) as the next band.
pub fn render_pass() {
    if !frame().active {
        return;
    }
    let v = view();
    let objs = object_list();
    let mut prio = [0i32; 48];
    let mut ranks = 0;
    let add = |p: i32, prio: &mut [i32; 48], ranks: &mut usize| {
        if !prio[..*ranks].contains(&p) && *ranks < 48 {
            prio[*ranks] = p;
            *ranks += 1;
        }
    };
    let (mut zmin, mut zmax) = (i32::MAX, i32::MIN);
    let mut count = 0;
    let mut list = [0u16; 256];
    for (i, obj) in objs.iter_mut().enumerate() {
        if obj.enabled == 0 || !obj.drawn() || obj.first_sorted_plane < 0 {
            continue;
        }
        let Some(c) = v.to_camera(&obj.modified_pos) else { continue };
        obj.world_pos = c;
        let (z, r) = (c.z >> GTE_SHIFT, (obj.radius >> GTE_SHIFT) + 1);
        if z + r < NEAR || v.outside(c, r) {
            continue;
        }
        zmin = zmin.min(z - r);
        zmax = zmax.max(z + r);
        add(obj.priority, &mut prio, &mut ranks);
        if count < list.len() {
            list[count] = i as u16;
            count += 1;
        }
    }
    let lines = crate::lines::queued();
    for l in lines.iter() {
        add(l.sort, &mut prio, &mut ranks);
    }
    if ranks == 0 {
        return;
    }
    // higher priority values are drawn first (the room before the table)
    for i in 1..ranks {
        let mut j = i;
        while j > 0 && prio[j - 1] < prio[j] {
            prio.swap(j - 1, j);
            j -= 1;
        }
    }
    let f = frame();
    let per = if count > 4 { (f.avail() / (ranks * 2)).min(128) } else { (f.avail() / ranks).min(8) }.max(1);
    let Some(top) = f.band(per * ranks) else { return };
    zmin = zmin.max(0);
    if zmax < zmin {
        zmax = zmin;
    }
    let mut shift = 0;
    while ((zmax - zmin) >> shift) >= per as i32 {
        shift += 1;
    }
    let bands = Bands { top, per, zmin, shift, prio, ranks };

    v.load_gte();
    for &i in list[..count].iter() {
        let obj = &objs[i as usize];
        if !v.load_object(obj) {
            continue;
        }
        draw_object(obj, &bands, v);
        *run(3) += 1;
    }
    for l in lines.iter() {
        let slot = bands.slot(bands.rank_top(l.sort), i32::MIN / 2);
        emit_line(slot, l.x0 as i32, l.y0 as i32, l.x1 as i32, l.y1 as i32, rgb_of(l.colour));
    }
}

fn draw_object(obj: &Object, bands: &Bands, v: &View) {
    let n = transform(obj);
    let scr = pool::scratch();
    let idx = pool::indices();
    let top = bands.rank_top(obj.priority);
    let verts = obj.vertices();
    for (k, plane) in obj.planes().iter().enumerate() {
        let e = plane_entry(obj, k);
        if e & PLANE_SKIP != 0 {
            continue;
        }
        let off = (e & 0xFFFF) as usize;
        let len = ((e >> 16) & 0xFF) as usize;
        let lp = &idx[off..off + len];
        if len > n {
            continue;
        }
        let mut fast = true;
        let (mut x0, mut x1, mut y0, mut y1) = (i32::MAX, i32::MIN, i32::MAX, i32::MIN);
        let (mut zsum, mut area) = (0i32, 0i32);
        let mut a = scr[lp[len - 1] as usize];
        for &i in lp {
            let s = scr[i as usize];
            if s.ok == 0 {
                fast = false;
                break;
            }
            let (x, y) = (s.x as i32, s.y as i32);
            x0 = x0.min(x);
            x1 = x1.max(x);
            y0 = y0.min(y);
            y1 = y1.max(y);
            zsum += s.z as i32;
            area += a.x as i32 * y - x * a.y as i32;
            a = s;
        }
        if fast {
            if x1 < 0 || x0 >= PSX_W || y1 < 0 || y0 >= PSX_H {
                continue;
            }
            if x1 - x0 > 1000 || y1 - y0 > 500 {
                fast = false;
            }
        }
        let concave = e & PLANE_CONCAVE != 0;
        let rgb = plane_rgb(plane.colour, plane.brightness);
        if fast {
            if area <= 0 {
                continue;
            }
            let slot = bands.slot(top, zsum / len as i32);
            let pt = |i: u8| {
                let s = scr[lp[i as usize] as usize];
                (s.x as i32, s.y as i32)
            };
            if concave {
                let tris = &idx[off + len..off + len + 3 * (len - 2)];
                for t in tris.chunks(3) {
                    emit_flat(slot, 3, |j| pt(t[j]), rgb);
                }
            } else {
                emit_flat(slot, len, |j| pt(j as u8), rgb);
            }
            continue;
        }
        // the slow path: clip in camera space, one convex piece at a time
        let piece = |a: usize, b: usize, c: usize, d: Option<usize>| {
            let mut poly = [[0i32; 4]; 4];
            let mut m = 0;
            for &i in [Some(a), Some(b), Some(c), d].iter().flatten() {
                let p = &verts[lp[i] as usize];
                if !fits(p) {
                    return;
                }
                let c = camera_space(p);
                poly[m] = [c[0], c[1], c[2], lp[i] as i32];
                m += 1;
            }
            clipped(&poly[..m], v, bands, top, rgb);
        };
        if concave {
            let tris = &idx[off + len..off + len + 3 * (len - 2)];
            for t in tris.chunks(3) {
                piece(t[0] as usize, t[1] as usize, t[2] as usize, None);
            }
        } else {
            let mut i = 1;
            while i + 1 < len {
                if i + 2 < len {
                    piece(0, i, i + 1, Some(i + 2));
                    i += 2;
                } else {
                    piece(0, i, i + 1, None);
                    i += 1;
                }
            }
        }
    }
}

/// Clip a convex camera-space polygon (GTE units, y as the GTE's; the
/// fourth value the vertex's scratch index, or -1) to the near plane and the
/// guard band, project it and draw it. Vertices the clipping keeps use the
/// GTE's projection when it had one, and every crossing is computed from the
/// edge's two ends in one order, so neighbouring polygons meet without
/// cracks.
fn clipped(poly: &[[i32; 4]], v: &View, bands: &Bands, top: usize, rgb: u32) {
    *run(1) += 1;
    let mut a = [[0i32; 4]; 16];
    let mut b = [[0i32; 4]; 16];
    let mut n = poly.len();
    a[..n].copy_from_slice(poly);
    let h = v.h;
    // plane distances, positive inside: z - NEAR; x*h - (L-ofx)*z; ...
    let planes: [fn(&[i32; 4], i32, i32, i32) -> i32; 5] = [
        |p, _, _, _| p[2] - NEAR,
        |p, h, ofx, _| p[0] * h - (GUARD_L - ofx) * p[2],
        |p, h, ofx, _| (GUARD_R - ofx) * p[2] - p[0] * h,
        |p, h, _, ofy| p[1] * h - (GUARD_T - ofy) * p[2],
        |p, h, _, ofy| (GUARD_B - ofy) * p[2] - p[1] * h,
    ];
    for (k, dist) in planes.iter().enumerate() {
        let (src, dst) = if k % 2 == 0 { (&a, &mut b) } else { (&b, &mut a) };
        let mut m = 0;
        for i in 0..n {
            let (p, q) = (&src[i], &src[(i + 1) % n]);
            let (dp, dq) = (dist(p, h, v.ofx, v.ofy), dist(q, h, v.ofx, v.ofy));
            if dp >= 0 && m < 16 {
                dst[m] = *p;
                m += 1;
            }
            if (dp >= 0) != (dq >= 0) && m < 16 {
                dst[m] = if p[..3] <= q[..3] { lerp(p, q, dp, dq) } else { lerp(q, p, dq, dp) };
                m += 1;
            }
        }
        n = m;
        if n < 3 {
            return;
        }
    }
    // after five planes the result is in b
    let scr = pool::scratch();
    let mut pts = [(0i32, 0i32); 16];
    let mut zsum = 0;
    for i in 0..n {
        let p = &b[i];
        pts[i] = match scr.get(p[3] as usize) {
            Some(s) if p[3] >= 0 && s.ok != 0 => (s.x as i32, s.y as i32),
            _ => {
                let z = p[2].max(1);
                (v.ofx + p[0] * h / z, v.ofy + p[1] * h / z)
            }
        };
        zsum += p[2];
    }
    let mut area = 0;
    for i in 0..n {
        let (p, q) = (pts[i], pts[(i + 1) % n]);
        area += p.0 * q.1 - q.0 * p.1;
    }
    if area <= 0 {
        return;
    }
    emit_flat(bands.slot(top, zsum / n as i32), n, |i| pts[i], rgb);
}

/// The point where p..q crosses a plane, from their distances to it.
fn lerp(p: &[i32; 4], q: &[i32; 4], dp: i32, dq: i32) -> [i32; 4] {
    // t = dp / (dp - dq) in Q12, with both scaled down so dp << 12 fits
    let (mut num, mut den) = (dp, dp - dq);
    while num.abs() >= 1 << 18 || den.abs() >= 1 << 18 {
        num >>= 1;
        den >>= 1;
    }
    let t = if den == 0 { 0 } else { ((num << 12) / den).clamp(0, 4096) };
    [p[0] + mul12(t, q[0] - p[0]), p[1] + mul12(t, q[1] - p[1]), p[2] + mul12(t, q[2] - p[2]), -1]
}
