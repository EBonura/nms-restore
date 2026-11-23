//! The extended graphics library (libgs), as 360X uses it.
//!
//! Written from NMS's calls and from what NMS's 1996 build of 360X does,
//! read from its MAIN.CPE with MAIN.SYM (field offsets, packet words, the
//! order of operations and the roundings). Nothing here is Sony's code.
//!
//! # Buffers and origin
//!
//! `GsDefDispBuff` names two buffers in VRAM. `GsSwapDispBuff` shows the
//! active one, makes the other active and sets the drawing area to it. In
//! the offset mode 360X uses (GTE offset, `GsInitGraph` intmode bit 2
//! clear) the GPU's drawing offset stays 0 and every packet carries absolute
//! VRAM coordinates: a packet's position is the game's x, y plus an origin
//! (`ORGOFS`) that is the screen centre (`GsInit3D`) plus the corner of the
//! buffer the ordering table being built will be drawn into, which is the
//! one that is not active while it is built (the swap comes before the
//! draw). The GTE's screen offset is the same origin, so models line up
//! with sprites.
//!
//! # Ordering tables
//!
//! `GsClearOt` links a table backwards (`ClearOTagR`), so the GPU walks from
//! the last entry to the first and a smaller priority is drawn later, in
//! front. Packets come from the work area `GsSetWorkBase` names and go on
//! the table at `priority - offset`.
//!
//! # Models
//!
//! `GsLinkObject4` and `GsSortObject4` draw TMD objects. Like the 1996
//! library, `GsLinkObject4` writes into the TMD: the first primitive of each
//! run of primitives of one type gets the run's length in its first
//! half-word (where the TMD has its olen and ilen bytes), and
//! `GsSortObject4` walks runs. Only the primitive types 360X draws are
//! implemented: lit flat textured triangles (TMD mode 24h, the planets),
//! with and without fog. Any other type is skipped with one message.

use crate::gpu::{DispEnv, DrawEnv, Rect};
use crate::gte::{self, gte_cmd, Matrix, SVector, Vector};

// ------------------------------------------------------------ structures

/// GsOT: `length` bits of priority, entries at `org`, walked from `tag`.
#[repr(C)]
pub struct GsOt {
    pub length: u32,
    pub org: *mut u32,
    pub offset: u32,
    pub point: u32,
    pub tag: *mut u32,
}

#[repr(C)]
pub struct GsSprite {
    pub attribute: u32,
    pub x: i16,
    pub y: i16,
    pub w: u16,
    pub h: u16,
    pub tpage: u16,
    pub u: u8,
    pub v: u8,
    pub cx: i16,
    pub cy: i16,
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub mx: i16,
    pub my: i16,
    pub scalex: i16,
    pub scaley: i16,
    pub rotate: i32,
}

#[repr(C)]
pub struct GsLine {
    pub attribute: u32,
    pub x0: i16,
    pub y0: i16,
    pub x1: i16,
    pub y1: i16,
    pub r: u8,
    pub g: u8,
    pub b: u8,
}

#[repr(C)]
pub struct GsBoxF {
    pub attribute: u32,
    pub x: i16,
    pub y: i16,
    pub w: u16,
    pub h: u16,
    pub r: u8,
    pub g: u8,
    pub b: u8,
}

#[repr(C)]
pub struct GsGLine {
    pub attribute: u32,
    pub x0: i16,
    pub y0: i16,
    pub x1: i16,
    pub y1: i16,
    pub r0: u8,
    pub g0: u8,
    pub b0: u8,
    pub r1: u8,
    pub g1: u8,
    pub b1: u8,
}

#[repr(C)]
pub struct GsImage {
    pub pmode: u32,
    pub px: i16,
    pub py: i16,
    pub pw: u16,
    pub ph: u16,
    pub pixel: *mut u32,
    pub cx: i16,
    pub cy: i16,
    pub cw: u16,
    pub ch: u16,
    pub clut: *mut u32,
}

#[repr(C)]
pub struct GsCoordinate2 {
    pub flg: u32,
    pub coord: Matrix,
    pub workm: Matrix,
    pub sup: *mut GsCoordinate2,
    pub sub: *mut GsCoordinate2,
}

#[repr(C)]
pub struct GsRview2 {
    pub vpx: i32,
    pub vpy: i32,
    pub vpz: i32,
    pub vrx: i32,
    pub vry: i32,
    pub vrz: i32,
    pub rz: i32,
    pub sup: *mut GsCoordinate2,
}

#[repr(C)]
pub struct GsFLight {
    pub vx: i32,
    pub vy: i32,
    pub vz: i32,
    pub r: u8,
    pub g: u8,
    pub b: u8,
}

#[repr(C)]
pub struct GsFogParam {
    pub dqa: i16,
    pub dqb: i32,
    pub rfc: u8,
    pub gfc: u8,
    pub bfc: u8,
}

#[repr(C)]
pub struct GsDobj2 {
    pub attribute: u32,
    pub coord2: *mut GsCoordinate2,
    pub tmd: *mut u32,
    pub id: u32,
}

// --------------------------------------------------------------- state

const EMPTY_RECT: Rect = Rect { x: 0, y: 0, w: 0, h: 0 };

#[no_mangle]
pub static mut GsDRAWENV: DrawEnv = DrawEnv {
    clip: EMPTY_RECT,
    ofs: [0; 2],
    tw: EMPTY_RECT,
    tpage: 0,
    dtd: 0,
    dfe: 0,
    isbg: 0,
    r0: 0,
    g0: 0,
    b0: 0,
    dr_env: [0; 16],
};

#[no_mangle]
pub static mut GsDISPENV: DispEnv =
    DispEnv { disp: EMPTY_RECT, screen: EMPTY_RECT, isinter: 0, isrgb24: 0, pad0: 0, pad1: 0 };

#[no_mangle]
pub static mut GsIDMATRIX: Matrix = gte::IDENTITY;
#[no_mangle]
pub static mut GsWSMATRIX: Matrix = gte::IDENTITY;

/// Everything else the library keeps, in one place.
struct Gs {
    /// active buffer (0 or 1) and the frame counter the swap advances
    idx: usize,
    count: u32,
    /// buffer corners (GsDefDispBuff)
    ofs: [[i16; 2]; 2],
    /// GPU offset mode (GsInitGraph intmode bit 2)
    gpu_offset: bool,
    /// screen size, the origin inside the screen, the clip inside it
    width: i32,
    height: i32,
    position: [i16; 2],
    clip: Rect,
    /// added to every packet's x, y
    org: [i16; 2],
    /// next free byte of the packet area
    packet: *mut u32,
    /// identity with the screen's aspect in m[1][1] (GsSetRefView2's start)
    id2: Matrix,
    /// directions of the three flat lights (rows), and their colours
    light_ws: Matrix,
    light_colour: Matrix,
    light_mode: i32,
    near_clip: i32,
    far_clip: i32,
}

static mut GS: Gs = Gs {
    idx: 0,
    count: 1,
    ofs: [[0; 2]; 2],
    gpu_offset: false,
    width: 320,
    height: 240,
    position: [0; 2],
    clip: EMPTY_RECT,
    org: [0; 2],
    packet: core::ptr::null_mut(),
    id2: gte::IDENTITY,
    light_ws: gte::ZERO,
    light_colour: gte::ZERO,
    light_mode: 0,
    near_clip: 10,
    far_clip: 0x3FFF,
};

#[allow(static_mut_refs)]
fn gs() -> &'static mut Gs {
    // SAFETY: the library runs in the foreground only (never from the
    // VBlank handler), one call at a time.
    unsafe { &mut GS }
}

#[allow(static_mut_refs)]
fn drawenv() -> &'static mut DrawEnv {
    // SAFETY: as gs().
    unsafe { &mut GsDRAWENV }
}

#[allow(static_mut_refs)]
fn dispenv() -> &'static mut DispEnv {
    // SAFETY: as gs().
    unsafe { &mut GsDISPENV }
}

// -------------------------------------------------- screen and buffers

fn set_draw_clip() {
    let s = gs();
    let o = s.ofs[s.idx];
    let e = drawenv();
    e.clip = Rect { x: s.clip.x + o[0], y: s.clip.y + o[1], w: s.clip.w, h: s.clip.h };
    // SAFETY: the library's own DRAWENV.
    unsafe { crate::gpu::PutDrawEnv(e) };
}

fn set_draw_offset() {
    let s = gs();
    if s.gpu_offset {
        let o = s.ofs[s.idx];
        let e = drawenv();
        e.ofs = [s.position[0] + o[0], s.position[1] + o[1]];
        s.org = [0, 0];
        // SAFETY: the library's own DRAWENV.
        unsafe { crate::gpu::PutDrawEnv(e) };
    } else {
        // the buffer the table now being built is drawn into: the other one
        let o = s.ofs[s.idx ^ 1];
        s.org = [s.position[0] + o[0], s.position[1] + o[1]];
        gte::SetGeomOffset(s.org[0] as i32, s.org[1] as i32);
    }
}

#[no_mangle]
pub extern "C" fn GsInitGraph(w: u32, h: u32, intmode: u32, dith: u32, vrammode: u32) {
    let (w, h) = (w as u16 as i16, h as u16 as i16);
    crate::gpu::ResetGraph(0);
    let e = drawenv();
    e.clip = EMPTY_RECT;
    e.ofs = [0, 0];
    e.tw = EMPTY_RECT;
    e.tpage = 0;
    e.dtd = dith as u8;
    e.dfe = 0;
    e.isbg = 0;
    // SAFETY: the library's own DRAWENV.
    unsafe { crate::gpu::PutDrawEnv(e) };
    // The display: the whole screen; the TV area at its default except 24
    // lines down on PAL, as the 1996 build sets it (whatever the game had
    // put in GsDISPENV before is overwritten).
    let pal = crate::gpu::video_mode() == 1;
    let d = dispenv();
    d.disp = Rect { x: 0, y: 0, w, h };
    d.screen = Rect { x: 0, y: if pal { 24 } else { 0 }, w: 0, h: 0 };
    d.isinter = (intmode & 1) as u8;
    d.isrgb24 = vrammode as u8;
    d.pad0 = u8::from(pal);
    let s = gs();
    s.gpu_offset = intmode & 4 != 0;
    // SAFETY: the library's own DISPENV.
    unsafe { crate::gpu::PutDispEnv(d) };
    // the GTE
    gte::InitGeom();
    gte::SetFarColor(0, 0, 0);
    gte::SetGeomOffset(0, 0);
    s.org = [0, 0];
    s.idx = 0;
    // the library's variables
    s.width = w as i32;
    s.height = h as i32;
    // SAFETY: foreground-only statics.
    unsafe {
        GsIDMATRIX = gte::IDENTITY;
    }
    s.id2 = gte::IDENTITY;
    s.id2.m[1][1] = if w != 0 { (((h as i32) << 14) / w as i32 / 3) as i16 } else { 4096 };
    s.light_ws = gte::ZERO;
    s.position = [0, 0];
    s.clip = Rect { x: 0, y: 0, w, h };
    s.count = 1;
    set_draw_clip();
    set_draw_offset();
}

#[no_mangle]
pub extern "C" fn GsDefDispBuff(x0: u32, y0: u32, x1: u32, y1: u32) {
    let s = gs();
    s.ofs = [[x0 as i16, y0 as i16], [x1 as i16, y1 as i16]];
    set_draw_clip();
    set_draw_offset();
}

#[no_mangle]
pub extern "C" fn GsSwapDispBuff() {
    let s = gs();
    let o = s.ofs[s.idx];
    let d = dispenv();
    d.disp.x = o[0];
    d.disp.y = o[1];
    // SAFETY: the library's own DISPENV.
    unsafe { crate::gpu::PutDispEnv(d) };
    crate::gpu::SetDispMask(1);
    s.idx ^= 1;
    set_draw_clip();
    set_draw_offset();
    s.count = s.count.wrapping_add(1).max(1);
}

#[no_mangle]
pub extern "C" fn GsGetActiveBuff() -> i32 {
    gs().idx as i32
}

#[no_mangle]
#[link_section = ".text.layer_hot"]
pub extern "C" fn GsSetWorkBase(base: *mut u32) {
    gs().packet = base;
}

/// The origin in the middle of the screen; default near and far clip.
#[no_mangle]
pub extern "C" fn GsInit3D() {
    let s = gs();
    s.position = [(s.width / 2) as i16, (s.height / 2) as i16];
    set_draw_offset();
    s.light_mode = 0;
    s.near_clip = 10;
    s.far_clip = 0x3FFF;
}

/// Clear a table for building. First waits until the previous table's
/// drawing has ended and its DrawSync callback has run: in NMS's 1996 build
/// that callback (Interupt_GPU_Stuff, which frees the last sprite image it
/// sent to VRAM) has always run by the time Start_Loop clears the next
/// table and resets its `lastfree`, and without the wait the rebuild's
/// slower drawing let the reset come first and leak one block per frame.
#[no_mangle]
#[link_section = ".text.layer_warm"]
pub unsafe extern "C" fn GsClearOt(offset: u32, point: u32, ot: *mut GsOt) {
    crate::gpu::wait_walk_handled();
    // SAFETY: the game's GsOT and its table.
    unsafe {
        let ot = &mut *ot;
        ot.offset = offset & 0xFFFF;
        ot.point = point & 0xFFFF;
        let n = 1u32 << ot.length;
        ot.tag = ot.org.add(n as usize - 1);
        crate::gpu::ClearOTagR(ot.org, n as i32);
    }
}

#[no_mangle]
pub unsafe extern "C" fn GsDrawOt(ot: *mut GsOt) {
    // SAFETY: the game's GsOT.
    unsafe { crate::gpu::DrawOTag((*ot).tag) }
}

// -------------------------------------------------------------- packets

/// Put `len` words at the packet pointer on the table at `pri`, and move the
/// packet pointer past them.
unsafe fn add_packet(ot: *mut GsOt, pri: u32, len: u32) {
    let s = gs();
    let p = s.packet;
    // SAFETY: the game's GsOT; the packet area GsSetWorkBase named.
    unsafe {
        let ot = &*ot;
        let index = (pri & 0xFFFF).wrapping_sub(ot.offset);
        if (index as i32) < 0 {
            crate::console::puts("psyq: GsSort: priority below the table's offset\n");
        }
        let entry = ot.org.add(index as usize);
        *p = (*entry & 0x00FF_FFFF) | (len << 24);
        *entry = p as u32 & 0x00FF_FFFF;
        s.packet = p.add(len as usize + 1);
    }
}

#[inline(always)]
fn xy(x: i32, y: i32) -> u32 {
    (x as u32 & 0xFFFF) | ((y as u32) << 16)
}

/// The packet code word's colour, code and the flags shared by every
/// textured packet: semi-transparent (attribute bit 30) and texture not
/// shaded (attribute bit 6).
#[inline(always)]
fn tex_code(attr: u32, code: u32, r: u8, g: u8, b: u8) -> u32 {
    ((attr >> 5) & 0x0200_0000) | ((attr << 18) & 0x0100_0000) | (code << 24) | ((b as u32) << 16) | ((g as u32) << 8) | r as u32
}

/// Draw mode (E1h) for a sprite: its page, the colour mode in attribute
/// bits 24-25, the blend in bits 28-29, dithering on.
#[inline(always)]
fn sprite_draw_mode(attr: u32, tpage: u16) -> u32 {
    0xE100_0200 | ((attr >> 17) & 0x180) | ((attr >> 23) & 0x60) | (tpage as u32 & 0x1F)
}

#[inline(always)]
fn clut_word(cx: i16, cy: i16) -> u32 {
    ((cy as u32) << 22) | (((cx as u32) << 12) & 0x003F_0000)
}

/// A sprite as the GPU's sprite primitive, neither rotated nor scaled.
#[no_mangle]
#[link_section = ".text.layer_hot"]
pub unsafe extern "C" fn GsSortFastSprite(sp: *const GsSprite, ot: *mut GsOt, pri: u32) {
    // SAFETY: the game's GsSPRITE.
    let sp = unsafe { &*sp };
    unsafe { sort_sprt(sp, ot, pri, 0, 0) }
}

#[link_section = ".text.layer_hot"]
unsafe fn sort_sprt(sp: &GsSprite, ot: *mut GsOt, pri: u32, mx: i16, my: i16) {
    if sp.attribute & 0x8000_0000 != 0 || sp.w == 0 || sp.h == 0 {
        return;
    }
    let s = gs();
    let p = s.packet;
    // SAFETY: the packet area GsSetWorkBase named.
    unsafe {
        *p.add(1) = sprite_draw_mode(sp.attribute, sp.tpage);
        *p.add(2) = tex_code(sp.attribute, 0x64, sp.r, sp.g, sp.b);
        *p.add(3) = xy(sp.x as i32 + s.org[0] as i32 - mx as i32, sp.y as i32 + s.org[1] as i32 - my as i32);
        *p.add(4) = sp.u as u32 | ((sp.v as u32) << 8) | clut_word(sp.cx, sp.cy);
        *p.add(5) = sp.w as u32 | ((sp.h as u32) << 16);
        add_packet(ot, pri, 5);
    }
}

/// A sprite that may be rotated and scaled about (mx, my), as a textured
/// quadrangle; unrotated and unscaled it is a sprite primitive, moved by
/// (-mx, -my).
#[no_mangle]
pub unsafe extern "C" fn GsSortSprite(sp: *const GsSprite, ot: *mut GsOt, pri: u32) {
    // SAFETY: the game's GsSPRITE.
    let sp = unsafe { &*sp };
    let attr = sp.attribute;
    if attr & 0x8000_0000 != 0 || sp.w == 0 || sp.h == 0 {
        return;
    }
    let plain = sp.scalex == 4096 && sp.scaley == 4096 && sp.rotate == 0 && attr & 0x00C0_0000 == 0;
    if attr & (1 << 27) != 0 || plain {
        // SAFETY: as GsSortFastSprite.
        unsafe { sort_sprt(sp, ot, pri, sp.mx, sp.my) };
        return;
    }
    // rotation (4096 to a degree, so /360 for 4096 to a turn), then scale
    let mut m = gte::IDENTITY;
    if sp.rotate != 0 {
        let r = SVector { vx: 0, vy: 0, vz: (sp.rotate / 360) as i16, pad: 0 };
        // SAFETY: local values.
        unsafe { gte::RotMatrix(&r, &mut m) };
    }
    if !(sp.scalex == 4096 && sp.scaley == 4096) {
        let v = Vector { vx: sp.scalex as i32, vy: sp.scaley as i32, vz: 0, pad: 0 };
        // SAFETY: local values.
        unsafe { gte::ScaleMatrix(&mut m, &v) };
    }
    m.t = [sp.x as i32, sp.y as i32, gte::read_geom_screen()];
    gte::set_rot_matrix(&m);
    gte::set_trans_matrix(&m);
    let (x0, y0) = (-(sp.mx as i32), -(sp.my as i32));
    let (w, h) = (sp.w as i32, sp.h as i32);
    let corners = [(x0, y0), (x0 + w, y0), (x0, y0 + h), (x0 + w, y0 + h)];
    let mut sxy = [0u32; 4];
    for (i, &(cx, cy)) in corners.iter().enumerate() {
        sxy[i] = project(cx, cy, 0);
    }
    let (ul, ur) = if attr & 0x0080_0000 != 0 {
        (sp.u.wrapping_add(sp.w as u8).wrapping_sub(1), sp.u)
    } else {
        (sp.u, sp.u.wrapping_add(sp.w as u8).wrapping_sub(1))
    };
    let (vt, vb) = if attr & 0x0040_0000 != 0 {
        (sp.v.wrapping_add(sp.h as u8).wrapping_sub(1), sp.v)
    } else {
        (sp.v, sp.v.wrapping_add(sp.h as u8).wrapping_sub(1))
    };
    let tpage = (sp.tpage as u32 & 0x1F) | ((attr >> 17) & 0x180) | ((attr >> 23) & 0x60);
    let s = gs();
    let p = s.packet;
    // SAFETY: the packet area GsSetWorkBase named.
    unsafe {
        *p.add(1) = tex_code(attr, 0x2C, sp.r, sp.g, sp.b);
        *p.add(2) = sxy[0];
        *p.add(3) = ul as u32 | ((vt as u32) << 8) | clut_word(sp.cx, sp.cy);
        *p.add(4) = sxy[1];
        *p.add(5) = ur as u32 | ((vt as u32) << 8) | (tpage << 16);
        *p.add(6) = sxy[2];
        *p.add(7) = ul as u32 | ((vb as u32) << 8);
        *p.add(8) = sxy[3];
        *p.add(9) = ur as u32 | ((vb as u32) << 8);
        add_packet(ot, pri, 9);
    }
}

/// One point through the GTE (rotation, translation, perspective): its
/// screen position as a packed x, y word.
fn project(x: i32, y: i32, z: i32) -> u32 {
    psx_gte::write_data!(0, (x as u32 & 0xFFFF) | ((y as u32) << 16));
    psx_gte::write_data!(1, z as u32 & 0xFFFF);
    gte_cmd!(gte::RTPS);
    psx_gte::read_data!(14)
}

#[no_mangle]
pub unsafe extern "C" fn GsSortLine(lp: *const GsLine, ot: *mut GsOt, pri: u32) {
    // SAFETY: the game's GsLINE.
    let l = unsafe { &*lp };
    if l.attribute & 0x8000_0000 != 0 {
        return;
    }
    let s = gs();
    let p = s.packet;
    let code = 0x40 | ((l.attribute >> 29) & 2);
    // SAFETY: the packet area GsSetWorkBase named.
    unsafe {
        *p.add(1) = (code << 24) | ((l.b as u32) << 16) | ((l.g as u32) << 8) | l.r as u32;
        *p.add(2) = xy(l.x0 as i32 + s.org[0] as i32, l.y0 as i32 + s.org[1] as i32);
        *p.add(3) = xy(l.x1 as i32 + s.org[0] as i32, l.y1 as i32 + s.org[1] as i32);
        add_packet(ot, pri, 3);
    }
}

/// A flat-coloured rectangle (GP0 60h, 62h when semi-transparent). A
/// draw-mode word goes first so the blend in attribute bits 28-29 is the one
/// used, not whatever the packet drawn before it left; page 0, dithering on.
#[no_mangle]
pub unsafe extern "C" fn GsSortBoxFill(bp: *const GsBoxF, ot: *mut GsOt, pri: u32) {
    // SAFETY: the game's GsBOXF.
    let b = unsafe { &*bp };
    if b.attribute & 0x8000_0000 != 0 || b.w == 0 || b.h == 0 {
        return;
    }
    let s = gs();
    let p = s.packet;
    let code = 0x60 | ((b.attribute >> 29) & 2);
    // SAFETY: the packet area GsSetWorkBase named.
    unsafe {
        *p.add(1) = 0xE100_0200 | ((b.attribute >> 23) & 0x60);
        *p.add(2) = (code << 24) | ((b.b as u32) << 16) | ((b.g as u32) << 8) | b.r as u32;
        *p.add(3) = xy(b.x as i32 + s.org[0] as i32, b.y as i32 + s.org[1] as i32);
        *p.add(4) = b.w as u32 | ((b.h as u32) << 16);
        add_packet(ot, pri, 4);
    }
}

#[no_mangle]
pub unsafe extern "C" fn GsSortGLine(lp: *const GsGLine, ot: *mut GsOt, pri: u32) {
    // SAFETY: the game's GsGLINE.
    let l = unsafe { &*lp };
    if l.attribute & 0x8000_0000 != 0 {
        return;
    }
    let s = gs();
    let p = s.packet;
    let code = 0x50 | ((l.attribute >> 29) & 2);
    // SAFETY: the packet area GsSetWorkBase named.
    unsafe {
        *p.add(1) = (code << 24) | ((l.b0 as u32) << 16) | ((l.g0 as u32) << 8) | l.r0 as u32;
        *p.add(2) = xy(l.x0 as i32 + s.org[0] as i32, l.y0 as i32 + s.org[1] as i32);
        *p.add(3) = ((l.b1 as u32) << 16) | ((l.g1 as u32) << 8) | l.r1 as u32;
        *p.add(4) = xy(l.x1 as i32 + s.org[0] as i32, l.y1 as i32 + s.org[1] as i32);
        add_packet(ot, pri, 4);
    }
}

/// Where a TIM's image (and CLUT, when its flags say it has one) are. `tim`
/// points at the flags word, after the ID.
#[no_mangle]
pub unsafe extern "C" fn GsGetTimInfo(tim: *const u32, im: *mut GsImage) {
    // SAFETY: the game's TIM in RAM and its GsIMAGE.
    unsafe {
        let im = &mut *im;
        let flags = *tim;
        im.pmode = flags;
        let mut p = tim.add(1);
        if flags & 8 != 0 {
            let bytes = *p & !3;
            let c = p as *const u16;
            im.cx = *c.add(2) as i16;
            im.cy = *c.add(3) as i16;
            im.cw = *c.add(4);
            im.ch = *c.add(5);
            im.clut = p.add(3) as *mut u32;
            p = (p as *const u8).add(bytes as usize) as *const u32;
        }
        let c = p as *const u16;
        im.px = *c.add(2) as i16;
        im.py = *c.add(3) as i16;
        im.pw = *c.add(4);
        im.ph = *c.add(5);
        im.pixel = p.add(3) as *mut u32;
    }
}

// ------------------------------------------------------- coordinates

#[no_mangle]
pub unsafe extern "C" fn GsInitCoordinate2(sup: *mut GsCoordinate2, base: *mut GsCoordinate2) {
    // SAFETY: the game's coordinate systems.
    unsafe {
        let b = &mut *base;
        b.coord = GsIDMATRIX;
        b.sup = sup;
        b.flg = 0;
        if !sup.is_null() && sup as usize != 1 {
            (*sup).sub = base;
        }
    }
}

/// Local to world: the chain of coordinate systems from the world down,
/// composed parent first. Worked out every time (the 1996 library kept the
/// result in `workm` per frame; the game's coordinates do not change
/// between its calls within a frame, so the result is the same).
unsafe fn local_world(coord: *mut GsCoordinate2) -> Matrix {
    let mut chain: [*mut GsCoordinate2; 100] = [core::ptr::null_mut(); 100];
    let mut n = 0;
    let mut c = coord;
    while !c.is_null() && n < chain.len() {
        chain[n] = c;
        n += 1;
        // SAFETY: the game's coordinate systems.
        c = unsafe { (*c).sup };
    }
    // SAFETY: as above.
    let mut m = unsafe { (*chain[n - 1]).coord };
    unsafe { (*chain[n - 1]).workm = m };
    for i in (0..n - 1).rev() {
        // SAFETY: as above.
        unsafe {
            m = gte::compose(&m, &(*chain[i]).coord);
            (*chain[i]).workm = m;
        }
    }
    let count = gs().count;
    for &c in &chain[..n] {
        // SAFETY: as above.
        unsafe { (*c).flg = count };
    }
    m
}

#[no_mangle]
pub unsafe extern "C" fn GsGetLw(coord: *mut GsCoordinate2, m: *mut Matrix) {
    // SAFETY: the game's coordinate system and matrix.
    unsafe { *m = local_world(coord) }
}

/// Local to screen: the world-to-screen matrix composed with local to world.
#[no_mangle]
pub unsafe extern "C" fn GsGetLs(coord: *mut GsCoordinate2, m: *mut Matrix) {
    // SAFETY: the game's coordinate system and matrix.
    unsafe {
        let lw = local_world(coord);
        *m = gte::compose(&*(&raw const GsWSMATRIX), &lw);
    }
}

#[no_mangle]
pub unsafe extern "C" fn GsSetLsMatrix(m: *const Matrix) {
    // SAFETY: the game's matrix.
    unsafe {
        gte::set_rot_matrix(&*m);
        gte::set_trans_matrix(&*m);
    }
}

/// The lights' directions in the object's coordinates: the world light
/// matrix times the given (local to world) rotation.
#[no_mangle]
pub unsafe extern "C" fn GsSetLightMatrix(m: *const Matrix) {
    let s = gs();
    let mut l = s.light_ws;
    // SAFETY: the game's matrix.
    l.m = gte::mul_rot(&s.light_ws, unsafe { &*m });
    gte::set_light_matrix(&l);
}

/// Rotation about one axis from a sine and cosine, on the identity.
fn axis_matrix(sin: i16, cos: i16, axis: u8) -> Matrix {
    let mut m = gte::IDENTITY;
    match axis {
        b'x' => {
            m.m[1][1] = cos;
            m.m[2][2] = cos;
            m.m[1][2] = sin.wrapping_neg();
            m.m[2][1] = sin;
        }
        b'y' => {
            m.m[0][0] = cos;
            m.m[2][2] = cos;
            m.m[0][2] = sin;
            m.m[2][0] = sin.wrapping_neg();
        }
        _ => {
            m.m[0][0] = cos;
            m.m[1][1] = cos;
            m.m[0][1] = sin.wrapping_neg();
            m.m[1][0] = sin;
        }
    }
    m
}

/// World to screen from a viewpoint, a point looked at and a twist, as the
/// 1996 build works it out: the aspect matrix twisted by -rz, turned so the
/// line of sight is the Z axis (pitch about X, then heading about Y, sines
/// and cosines from lengths), translated by -vp.
#[no_mangle]
pub unsafe extern "C" fn GsSetRefView2(v: *const GsRview2) -> i32 {
    // SAFETY: the game's view.
    let v = unsafe { &*v };
    let s = gs();
    let mut w = s.id2;
    if v.rz != 0 {
        let a = v.rz / 360;
        let (sn, cs) = (gte::rsin(a) as i16, gte::rcos(a) as i16);
        let r = Matrix { m: [[cs, sn.wrapping_neg(), 0], [sn, cs, 0], [0, 0, 4096]], t: [0; 3] };
        w.m = gte::mul_rot(&w, &r);
    }
    // the six coordinates brought under 16 bits together
    let mut p = [v.vpx, v.vpy, v.vpz, v.vrx, v.vry, v.vrz];
    let max = p.iter().map(|x| x.unsigned_abs()).max().unwrap_or(0);
    let bits = 32 - max.leading_zeros() as i32;
    if bits >= 16 {
        for x in p.iter_mut() {
            *x >>= bits - 15;
        }
    }
    let (dx, dy, dz) = (p[3] - p[0], p[4] - p[1], p[5] - p[2]);
    let len = gte::square_root0(dx.wrapping_mul(dx).wrapping_add(dy.wrapping_mul(dy)).wrapping_add(dz.wrapping_mul(dz)));
    if len == 0 {
        return 1;
    }
    let sin = -(((p[1] - p[4]) << 12) / len);
    let flat = gte::square_root0(dx.wrapping_mul(dx).wrapping_add(dz.wrapping_mul(dz)));
    let cos = (flat << 12) / len;
    w.m = gte::mul_rot(&w, &axis_matrix(sin as i16, cos as i16, b'x'));
    if flat != 0 {
        let sin = (dx << 12) / flat;
        let cos = (dz << 12) / flat;
        w.m = gte::mul_rot(&w, &axis_matrix((-sin) as i16, cos as i16, b'y'));
    }
    w.t = gte::apply_lv(&w, [v.vpx.wrapping_neg(), v.vpy.wrapping_neg(), v.vpz.wrapping_neg()]);
    if !v.sup.is_null() {
        // a view in another coordinate system: through that system's inverse
        // SAFETY: the game's coordinate system.
        let lw = unsafe { local_world(v.sup) };
        let mut t = gte::IDENTITY;
        // SAFETY: local matrices.
        unsafe { gte::TransposeMatrix(&lw, &mut t) };
        let back = gte::apply_lv(&t, lw.t);
        t.t = [back[0].wrapping_neg(), back[1].wrapping_neg(), back[2].wrapping_neg()];
        w = gte::compose(&w, &t);
    }
    // SAFETY: foreground-only static.
    unsafe { GsWSMATRIX = w };
    0
}

/// 360X calls this for the title's cube demo view; worked out
/// as GsSetRefView2.
#[no_mangle]
pub unsafe extern "C" fn GsSetRefView(v: *const GsRview2) -> i32 {
    // SAFETY: as GsSetRefView2.
    unsafe { GsSetRefView2(v) }
}

#[no_mangle]
pub extern "C" fn GsSetProjection(h: i32) {
    gte::SetGeomScreen(h);
}

#[no_mangle]
pub extern "C" fn GsSetNearClip(near: i32) {
    gs().near_clip = near;
}

/// Flat light `id` (0-2): the direction it shines along, as a unit vector
/// (4.12) pointing back at the light, in a row of the world light matrix;
/// its colour (0-255 to 0-4096) in a column of the light colour matrix.
#[no_mangle]
pub unsafe extern "C" fn GsSetFlatLight(id: i32, l: *const GsFLight) -> i32 {
    // SAFETY: the game's light.
    let l = unsafe { &*l };
    let len = gte::square_root0(
        l.vx.wrapping_mul(l.vx).wrapping_add(l.vy.wrapping_mul(l.vy)).wrapping_add(l.vz.wrapping_mul(l.vz)),
    );
    if len == 0 {
        return -1;
    }
    if !(0..3).contains(&id) {
        crate::console::puts("psyq: GsSetFlatLight: light number out of range\n");
        return 0;
    }
    let s = gs();
    let i = id as usize;
    s.light_ws.m[i] = [
        ((-l.vx << 12) / len) as i16,
        ((-l.vy << 12) / len) as i16,
        ((-l.vz << 12) / len) as i16,
    ];
    let c = |v: u8| ((v as i32) << 12) / 255;
    s.light_colour.m[0][i] = c(l.r) as i16;
    s.light_colour.m[1][i] = c(l.g) as i16;
    s.light_colour.m[2][i] = c(l.b) as i16;
    gte::set_color_matrix(&s.light_colour);
    0
}

/// Ambient light, 0-4096 per channel: the GTE's background colour.
#[no_mangle]
pub extern "C" fn GsSetAmbient(r: i32, g: i32, b: i32) {
    gte::SetBackColor(r >> 4, g >> 4, b >> 4);
}

/// 0: lighting; 1: lighting and fog (2 and 3 are accepted as in 1996).
#[no_mangle]
pub extern "C" fn GsSetLightMode(mode: i32) {
    match mode {
        0..=3 => gs().light_mode = mode,
        _ => crate::console::puts("psyq: GsSetLightMode: unknown mode\n"),
    }
}

#[no_mangle]
pub unsafe extern "C" fn GsSetFogParam(f: *const GsFogParam) {
    // SAFETY: the game's fog parameters.
    let f = unsafe { &*f };
    gte::SetDQA(f.dqa as i32);
    gte::SetDQB(f.dqb);
    gte::SetFarColor(f.rfc as i32, f.gfc as i32, f.bfc as i32);
}

// ------------------------------------------------------------- models

/// TMD object table entry (7 words): vertices, normals, primitives, scale.
#[repr(C)]
struct TmdObject {
    vert_top: u32,
    n_vert: u32,
    normal_top: u32,
    n_normal: u32,
    primitive_top: u32,
    n_primitive: u32,
    scale: i32,
}

/// Turn the TMD's object table offsets into addresses, once (flag bit 0).
/// `p` points at the flags word, after the ID.
#[no_mangle]
pub unsafe extern "C" fn GsMapModelingData(p: *mut u32) {
    // SAFETY: the game's TMD in RAM.
    unsafe {
        if *p & 1 != 0 {
            return;
        }
        *p |= 1;
        let n = *p.add(1) as i32;
        let table = p.add(2);
        let base = table as u32;
        let objs = table as *mut TmdObject;
        for i in 0..n.max(0) as usize {
            let o = &mut *objs.add(i);
            o.vert_top = o.vert_top.wrapping_add(base);
            o.normal_top = o.normal_top.wrapping_add(base);
            o.primitive_top = o.primitive_top.wrapping_add(base);
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn GsLinkObject2(tmd: u32, obj: *mut GsDobj2, n: i32) {
    // SAFETY: the game's object and mapped TMD object table.
    unsafe { (*obj).tmd = (tmd as *mut u32).add(7 * n as usize) };
}

/// Size in bytes of one TMD primitive of a mode and flag: the header word
/// and the words of the public TMD primitive layouts (light source on or
/// off, flat or gouraud, textured or not, triangle or quadrangle,
/// gradation). None for a mode outside polygons.
fn tmd_packet_bytes(mode: u8, flag: u8) -> Option<usize> {
    if mode & 0xE0 != 0x20 {
        return None;
    }
    let iip = mode & 0x10 != 0; // gouraud
    let quad = mode & 0x08 != 0;
    let tme = mode & 0x04 != 0;
    let lit = flag & 1 == 0;
    let grd = flag & 4 != 0; // a colour per vertex, unlit only
    let n = if quad { 4 } else { 3 };
    // words after the header
    let uv = if tme { n } else { 0 }; // u,v + clut/tpage/pad half-words: one word per vertex
    let colour = if tme && lit { 0 } else if !lit && (iip || grd) { n } else { 1 };
    let normals = if lit { if iip { n } else { 1 } } else { 0 };
    let index_words = (normals + n + 1) / 2;
    Some(4 * (1 + uv + colour + index_words))
}

/// Link object `n` of a mapped TMD to `obj`, and mark the primitive runs
/// (see the module comment).
#[no_mangle]
pub unsafe extern "C" fn GsLinkObject4(tmd: u32, obj: *mut GsDobj2, n: i32) {
    // SAFETY: the game's object and mapped TMD.
    unsafe {
        let o = (tmd as *mut u32).add(7 * n as usize);
        (*obj).tmd = o;
        let o = &*(o as *const TmdObject);
        let mut p = o.primitive_top as *mut u8;
        let mut run_start = p;
        let mut run = 0u16;
        let mut mode = 0u8;
        for i in 0..o.n_primitive {
            let m = *p.add(3);
            if i != 0 && m != mode {
                *(run_start as *mut u16) = run;
                run = 0;
                run_start = p;
            }
            mode = m;
            match tmd_packet_bytes(m, *p.add(2)) {
                Some(b) => p = p.add(b),
                None => {
                    crate::console::puts("psyq: GsLinkObject4: unknown TMD primitive\n");
                    break;
                }
            }
            run += 1;
        }
        *(run_start as *mut u16) = run;
    }
}

/// Draw an object linked with `GsLinkObject2` (the TMD is not marked into
/// runs, so each primitive is looked at in turn): the title's cubes, which
/// the TMD (INTRO.TMD, object 0) builds from lit flat untextured triangles
/// (mode 20h, 22h semi-transparent). Same steps as `GsSortObject4`. Any
/// other primitive type is skipped with one message.
#[no_mangle]
pub unsafe extern "C" fn GsSortObject2(obj: *mut GsDobj2, ot: *mut GsOt, shift: i32) {
    // SAFETY: the game's object, mapped TMD and ordering table.
    unsafe {
        let ob = &*obj;
        if ob.attribute & 0x8000_0000 != 0 {
            return;
        }
        let semi = (ob.attribute >> 30) & 1;
        let light_off = (ob.attribute >> 6) & 1 != 0;
        let ignore = (ob.attribute >> 5) & 1 != 0;
        let lmode = (ob.attribute >> 3) & 3;
        let fog = !light_off && if ignore { lmode == 2 } else { gs().light_mode == 1 };
        let o = &*(ob.tmd as *const TmdObject);
        let mut p = o.primitive_top as *const u8;
        for _ in 0..o.n_primitive {
            let mode = *p.add(3);
            let flag = *p.add(2);
            let Some(bytes) = tmd_packet_bytes(mode & 0xFD, flag) else {
                crate::console::puts("psyq: GsSortObject2: unknown TMD primitive\n");
                return;
            };
            if mode & 0xFD == 0x20 && flag & 5 == 0 {
                f3_lit(p, o, ot, shift as u32, semi, fog);
            } else {
                stub!("GsSortObject2 (TMD primitives other than lit flat triangles)");
            }
            p = p.add(bytes);
        }
    }
}

/// One lit flat untextured triangle (TMD mode 20h): header, r g b mode,
/// then normal and vertex indices. Projected, culled, sorted and lit as
/// `tf3_lit` does; the packet is a POLY_F3 (5 words).
unsafe fn f3_lit(p: *const u8, o: &TmdObject, ot: *mut GsOt, shift: u32, semi: u32, fog: bool) {
    let verts = o.vert_top as *const u32;
    let normals = o.normal_top as *const u32;
    let s = gs();
    // SAFETY: the TMD (indices inside its tables) and the game's table.
    unsafe {
        let ot = &*ot;
        let w = p as *const u32;
        let code = (((*p.add(3) & 0xFD) as u32) | (semi << 1)) << 24;
        let i0 = *w.add(2);
        let i1 = *w.add(3);
        let (n0, v0) = (i0 & 0xFFFF, i0 >> 16);
        let (v1, v2) = (i1 & 0xFFFF, i1 >> 16);
        let pk = s.packet;
        psx_gte::write_data!(0, *verts.add(2 * v0 as usize));
        psx_gte::write_data!(1, *verts.add(2 * v0 as usize + 1));
        psx_gte::write_data!(2, *verts.add(2 * v1 as usize));
        psx_gte::write_data!(3, *verts.add(2 * v1 as usize + 1));
        psx_gte::write_data!(4, *verts.add(2 * v2 as usize));
        psx_gte::write_data!(5, *verts.add(2 * v2 as usize + 1));
        gte_cmd!(gte::RTPT);
        psx_gte::write_data!(6, (*w.add(1) & 0x00FF_FFFF) | code);
        let flags = psx_gte::read_control!(31);
        if flags & 0x8000_0000 != 0 {
            return;
        }
        gte_cmd!(gte::NCLIP);
        if (psx_gte::read_data!(24) as i32) <= 0 {
            return;
        }
        gte_cmd!(gte::AVSZ3);
        psx_gte::write_data!(0, *normals.add(2 * n0 as usize));
        psx_gte::write_data!(1, *normals.add(2 * n0 as usize + 1));
        let otz = psx_gte::read_data!(7);
        if fog {
            gte_cmd!(gte::NCDS);
        } else {
            gte_cmd!(gte::NCCS);
        }
        let index = (otz.wrapping_sub(ot.offset) >> shift) & 0xFFFF;
        let entry = ot.org.add(index as usize);
        *pk = (*entry & 0x00FF_FFFF) | (4 << 24);
        *entry = pk as u32 & 0x00FF_FFFF;
        *pk.add(2) = psx_gte::read_data!(12);
        *pk.add(3) = psx_gte::read_data!(13);
        *pk.add(4) = psx_gte::read_data!(14);
        *pk.add(1) = psx_gte::read_data!(22);
        s.packet = pk.add(5);
    }
}

/// Draw a linked TMD object with the current light-screen and light
/// matrices: each primitive is projected (RTPT), dropped when the GTE flags
/// an error or it faces away (NCLIP <= 0), put on the table at its average
/// Z (AVSZ3) shifted down by `shift`, and lit from its normal (NCDS with
/// fog, as light mode 1 asks, NCCS without).
#[no_mangle]
pub unsafe extern "C" fn GsSortObject4(obj: *mut GsDobj2, ot: *mut GsOt, shift: i32, _scratch: *mut u32) {
    // SAFETY: the game's object, linked TMD and ordering table.
    unsafe {
        let ob = &*obj;
        if ob.attribute & 0x8000_0000 != 0 {
            return;
        }
        let semi = (ob.attribute >> 30) & 1;
        let light_off = (ob.attribute >> 6) & 1 != 0;
        let ignore = (ob.attribute >> 5) & 1 != 0;
        let lmode = (ob.attribute >> 3) & 3;
        let fog = !light_off && if ignore { lmode == 2 } else { gs().light_mode == 1 };
        let o = &*(ob.tmd as *const TmdObject);
        let mut p = o.primitive_top as *const u8;
        let mut left = o.n_primitive;
        while left != 0 {
            let mode = *p.add(3) & 0xFD;
            let flag = *p.add(2);
            let run = *(p as *const u16) as u32;
            let Some(bytes) = tmd_packet_bytes(mode, flag) else {
                crate::console::puts("psyq: GsSortObject4: unknown TMD primitive\n");
                return;
            };
            if run == 0 || run > left {
                crate::console::puts("psyq: GsSortObject4: TMD not linked with GsLinkObject4\n");
                return;
            }
            if mode == 0x24 && flag & 5 == 0 {
                tf3_lit(p, run, o, ot, shift as u32, semi, fog);
            } else {
                stub!("GsSortObject4 (TMD primitives other than lit flat textured triangles)");
            }
            p = p.add(bytes * run as usize);
            left -= run;
        }
    }
}

/// A run of lit flat textured triangles (TMD mode 24h): header, then
/// u0 v0 CBA, u1 v1 TSB, u2 v2, then normal and vertex indices.
unsafe fn tf3_lit(mut p: *const u8, count: u32, o: &TmdObject, ot: *mut GsOt, shift: u32, semi: u32, fog: bool) {
    let verts = o.vert_top as *const u32;
    let normals = o.normal_top as *const u32;
    let s = gs();
    // SAFETY: the TMD (indices inside its tables) and the game's table.
    unsafe {
        let ot = &*ot;
        let code = ((*p.add(3) as u32) | (semi << 1)) << 24;
        for _ in 0..count {
            let w = p as *const u32;
            let i0 = *w.add(4);
            let i1 = *w.add(5);
            let (n0, v0) = (i0 & 0xFFFF, i0 >> 16);
            let (v1, v2) = (i1 & 0xFFFF, i1 >> 16);
            let pk = s.packet;
            psx_gte::write_data!(0, *verts.add(2 * v0 as usize));
            psx_gte::write_data!(1, *verts.add(2 * v0 as usize + 1));
            psx_gte::write_data!(2, *verts.add(2 * v1 as usize));
            psx_gte::write_data!(3, *verts.add(2 * v1 as usize + 1));
            psx_gte::write_data!(4, *verts.add(2 * v2 as usize));
            psx_gte::write_data!(5, *verts.add(2 * v2 as usize + 1));
            gte_cmd!(gte::RTPT);
            psx_gte::write_data!(6, 0x0080_8080 | code);
            *pk.add(3) = *w.add(1);
            *pk.add(5) = *w.add(2);
            *pk.add(7) = *w.add(3);
            p = p.add(24);
            let flags = psx_gte::read_control!(31);
            if flags & 0x8000_0000 != 0 {
                continue;
            }
            gte_cmd!(gte::NCLIP);
            if (psx_gte::read_data!(24) as i32) <= 0 {
                continue;
            }
            gte_cmd!(gte::AVSZ3);
            psx_gte::write_data!(0, *normals.add(2 * n0 as usize));
            psx_gte::write_data!(1, *normals.add(2 * n0 as usize + 1));
            let otz = psx_gte::read_data!(7);
            if fog {
                gte_cmd!(gte::NCDS);
            } else {
                gte_cmd!(gte::NCCS);
            }
            let index = (otz.wrapping_sub(ot.offset) >> shift) & 0xFFFF;
            let entry = ot.org.add(index as usize);
            *pk = (*entry & 0x00FF_FFFF) | (7 << 24);
            *entry = pk as u32 & 0x00FF_FFFF;
            *pk.add(2) = psx_gte::read_data!(12);
            *pk.add(4) = psx_gte::read_data!(13);
            *pk.add(6) = psx_gte::read_data!(14);
            *pk.add(1) = psx_gte::read_data!(22);
            s.packet = pk.add(8);
        }
    }
}

// ------------------------------------------- libgpu calls 360X adds

/// The texture page word for a page at (x, y): colour mode tp, blend abr.
#[no_mangle]
pub extern "C" fn GetTPage(tp: i32, abr: i32, x: i32, y: i32) -> u16 {
    (((tp & 3) << 7) | ((abr & 3) << 5) | ((y & 0x100) >> 4) | ((x & 0x3FF) >> 6)) as u16
}

/// Load a texture page image (w in pixels of mode tp) and return its page
/// word. As in 1996 the image is loaded whatever `pix` is (360X passes 1
/// to only get the word; the rectangle is overwritten later).
#[no_mangle]
pub unsafe extern "C" fn LoadTPage(pix: *const u8, tp: i32, abr: i32, x: i32, y: i32, w: i32, h: i32) -> u16 {
    let width = match tp {
        0 => w / 4,
        1 => w / 2,
        _ => w,
    };
    let r = Rect { x: x as i16, y: y as i16, w: width as i16, h: h as i16 };
    // SAFETY: the game's image (or, for pix 1, low RAM read as the 1996
    // build's DMA read it).
    unsafe { crate::gpu::LoadImage(&r, pix) };
    GetTPage(tp, abr, x, y)
}
