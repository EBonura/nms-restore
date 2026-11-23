//! The renderer's memory: the edge buffers E_Init allocates for Escher's
//! PC span renderer (2000 EDGEs, 152000 bytes, and 2000 EDGE_STARTs, 16000
//! bytes, in the game's model memory). Nothing else uses them on the PS1,
//! so this renderer takes them over and needs no RAM of its own.
//!
//! edge_mem:        plane table (1024 x u32), index pool (16 KB), the
//!                  pool palette table (E_PoolTabPtr, 2816 bytes), vertex
//!                  scratch (256 x 8 bytes), and two primitive buffers,
//!                  one per display page (about 62 KB each)
//! edge_start_mem:  the queue of 2D lines (E_Draw2DLine)

use crate::nms::{edge_mem, edge_start_mem};

pub const MAX_PLANES: usize = 1024;
const INDEX_BYTES: usize = 16384;
pub const POOLTAB_BYTES: usize = 2816;
pub const MAX_VERTS: usize = 256;
const EDGE_MEM_BYTES: usize = 152000;
const EDGE_START_BYTES: usize = 16000;

const PLANES_AT: usize = 0;
const INDEX_AT: usize = PLANES_AT + 4 * MAX_PLANES;
const POOLTAB_AT: usize = INDEX_AT + INDEX_BYTES;
const SCRATCH_AT: usize = POOLTAB_AT + POOLTAB_BYTES;
const PRIMS_AT: usize = SCRATCH_AT + 8 * MAX_VERTS;
pub const PRIM_BYTES: usize = ((EDGE_MEM_BYTES - PRIMS_AT) / 2) & !3;

/// One vertex after the GTE: screen x, y (PS1 pixels) and depth (GTE SZ3),
/// `ok` when the GTE raised no error for it (in front, no saturation).
#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct Screen {
    pub x: i16,
    pub y: i16,
    pub z: u16,
    pub ok: u16,
}

/// A queued E_Draw2DLine: PS1 pixels, palette index, sort value.
#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct Line2D {
    pub x0: i16,
    pub y0: i16,
    pub x1: i16,
    pub y1: i16,
    pub colour: i32,
    pub sort: i32,
}
pub const MAX_LINES: usize = EDGE_START_BYTES / core::mem::size_of::<Line2D>();

static mut INDEX_USED: usize = 0;

fn base() -> *mut u8 {
    // SAFETY: E_Init stored the edge buffer's address; it is word aligned.
    unsafe { edge_mem }
}

pub fn ready() -> bool {
    !base().is_null()
}

/// Forget the plane table and index pool (E_LoadMapData rebuilds them).
pub fn reset() {
    // SAFETY: foreground-only static.
    unsafe { INDEX_USED = 0 };
    for e in plane_table().iter_mut() {
        *e = 0;
    }
}

pub fn plane_table() -> &'static mut [u32] {
    if !ready() {
        return &mut [];
    }
    // SAFETY: inside edge_mem (see the layout above).
    unsafe { core::slice::from_raw_parts_mut(base().add(PLANES_AT).cast(), MAX_PLANES) }
}

pub fn indices() -> &'static mut [u8] {
    if !ready() {
        return &mut [];
    }
    // SAFETY: inside edge_mem.
    unsafe { core::slice::from_raw_parts_mut(base().add(INDEX_AT), INDEX_BYTES) }
}

pub fn alloc_indices(n: usize) -> Option<usize> {
    // SAFETY: foreground-only static.
    unsafe {
        if !ready() || INDEX_USED + n > INDEX_BYTES {
            return None;
        }
        let off = INDEX_USED;
        INDEX_USED += n;
        Some(off)
    }
}

pub fn pooltab() -> *mut u8 {
    if !ready() {
        return core::ptr::null_mut();
    }
    // SAFETY: inside edge_mem.
    unsafe { base().add(POOLTAB_AT) }
}

pub fn scratch() -> &'static mut [Screen] {
    if !ready() {
        return &mut [];
    }
    // SAFETY: inside edge_mem, 8-byte entries, word aligned.
    unsafe { core::slice::from_raw_parts_mut(base().add(SCRATCH_AT).cast(), MAX_VERTS) }
}

/// The primitive buffer of display page `page` (0 or 1), as words.
pub fn prims(page: usize) -> *mut u32 {
    if !ready() {
        return core::ptr::null_mut();
    }
    // SAFETY: inside edge_mem.
    unsafe { base().add(PRIMS_AT + (page & 1) * PRIM_BYTES).cast() }
}

pub fn lines() -> &'static mut [Line2D] {
    // SAFETY: E_Init's EDGE_START buffer.
    unsafe {
        if edge_start_mem.is_null() {
            return &mut [];
        }
        core::slice::from_raw_parts_mut(edge_start_mem.cast(), MAX_LINES)
    }
}
