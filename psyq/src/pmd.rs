//! PMD models (libgs `RotPMD_*`): QD Up's option screen draws its table,
//! ball-rack and player models with these (MODELS.C `RotPMD`, called from
//! PSX.C `DrawInsideBall`).
//!
//! The layout below was read from the model files in the 1996 data folder
//! (`MODELS/PMD/*.PMD`) and from MODELS.C, which walks them: a header (ID 42h,
//! byte offsets of the primitive section and the shared vertex section, the
//! object count), per object a count of primitive groups and their byte
//! offsets, and per group a header word, type in the high half and packet
//! count in the low half. A packet is two copies of a GPU primitive, already
//! filled in (tag length, command, colours, UVs): one for each display
//! buffer, picked by `id`. They are followed by the packet's vertices: for
//! the shared-vertex types (type bit 3) one word per vertex, a byte offset
//! into the shared section of SVECTORs; for the independent types the
//! SVECTORs themselves. Type bit 0 makes a quad, bit 1 Gouraud, bit 2
//! untextured (MODELS.C lists the functions in that order), bit 5 asks for
//! back faces to be skipped (MODELS.C passes it as `backc`).
//!
//! Every file in the data folder uses the shared-vertex flat and Gouraud
//! triangles (types 0Ch and 0Eh, 3Ch and 3Eh with the flags), and their
//! packet sizes add up to the section offsets exactly; the other fourteen
//! functions follow the same rules and are not exercised by the data.
//!
//! A drawn primitive gets its screen corners from the GTE (rotation and
//! translation as set up by the game, `SetCameraPMD`) and is linked into the
//! ordering table at the average depth: the GTE's OTZ (14 bits with the
//! default ZSF3/ZSF4) cut to `otlen` bits. MODELS.C passes 11 for its 4096
//! entry table, so the model sorts into its first 2048 entries.

use crate::gte::{self, gte_cmd};

const AVSZ4: u32 = 0x4B68_002E;
const RTPS: u32 = gte::RTPS;

/// Words in one GPU primitive of a PMD type (low three bits), and the word
/// offsets of its vertex coordinates.
fn layout(t: u32) -> (usize, &'static [usize]) {
    match t & 7 {
        0 => (8, &[2, 4, 6]),         // POLY_FT3
        1 => (10, &[2, 4, 6, 8]),     // POLY_FT4
        2 => (10, &[2, 5, 8]),        // POLY_GT3
        3 => (13, &[2, 5, 8, 11]),    // POLY_GT4
        4 => (5, &[2, 3, 4]),         // POLY_F3
        5 => (6, &[2, 3, 4, 5]),      // POLY_F4
        6 => (7, &[2, 4, 6]),         // POLY_G3
        _ => (9, &[2, 4, 6, 8]),      // POLY_G4
    }
}

/// Load one SVECTOR into GTE vertex register `n` (0..2).
#[inline(always)]
unsafe fn load_vertex(n: u32, v: *const u32) {
    // SAFETY: the caller's vertex, inside the model.
    let (a, b) = unsafe { (*v, *v.add(1)) };
    match n {
        0 => {
            psx_gte::write_data!(0, a);
            psx_gte::write_data!(1, b);
        }
        1 => {
            psx_gte::write_data!(2, a);
            psx_gte::write_data!(3, b);
        }
        _ => {
            psx_gte::write_data!(4, a);
            psx_gte::write_data!(5, b);
        }
    }
}

/// Draw one primitive group. `group` is the group header, `sv` the shared
/// vertex section (null for the independent types).
unsafe fn rot_pmd(t: u32, group: *mut u32, sv: *const u8, ot: *mut u32, otlen: i32, id: i32, backc: i32) {
    let (words, xy) = layout(t);
    let nv = xy.len();
    let shared = !sv.is_null();
    let vert_words = if shared { nv } else { 2 * nv };
    let shift = (14 - otlen.clamp(1, 14)) as u32;
    let limit = 1u32 << otlen.clamp(1, 14);
    // SAFETY: the game's model, loaded whole, and its ordering table.
    unsafe {
        let count = *group & 0xFFFF;
        let mut pk = group.add(1);
        for _ in 0..count {
            let prim = pk.add(words * (id as usize & 1));
            let verts = pk.add(2 * words);
            let vertex = |i: usize| -> *const u32 {
                if shared {
                    sv.add(*verts.add(i) as usize) as *const u32
                } else {
                    verts.add(2 * i)
                }
            };
            pk = pk.add(2 * words + vert_words);

            load_vertex(0, vertex(0));
            load_vertex(1, vertex(1));
            load_vertex(2, vertex(2));
            gte_cmd!(gte::RTPT);
            if psx_gte::read_control!(31) & 0x8000_0000 != 0 {
                continue;
            }
            if backc != 0 {
                gte_cmd!(gte::NCLIP);
                if (psx_gte::read_data!(24) as i32) <= 0 {
                    continue;
                }
            }
            let otz;
            if nv == 4 {
                *prim.add(xy[0]) = psx_gte::read_data!(12);
                load_vertex(0, vertex(3));
                gte_cmd!(RTPS);
                if psx_gte::read_control!(31) & 0x8000_0000 != 0 {
                    continue;
                }
                gte_cmd!(AVSZ4);
                otz = psx_gte::read_data!(7);
                *prim.add(xy[1]) = psx_gte::read_data!(12);
                *prim.add(xy[2]) = psx_gte::read_data!(13);
                *prim.add(xy[3]) = psx_gte::read_data!(14);
            } else {
                gte_cmd!(gte::AVSZ3);
                otz = psx_gte::read_data!(7);
                *prim.add(xy[0]) = psx_gte::read_data!(12);
                *prim.add(xy[1]) = psx_gte::read_data!(13);
                *prim.add(xy[2]) = psx_gte::read_data!(14);
            }
            let index = otz >> shift;
            if otz == 0 || index >= limit {
                continue;
            }
            let entry = ot.add(index as usize);
            *prim = (*prim & 0xFF00_0000) | (*entry & 0x00FF_FFFF);
            *entry = (*entry & 0xFF00_0000) | (prim as u32 & 0x00FF_FFFF);
        }
    }
}

macro_rules! rot_pmd_fn {
    ($name:ident, $t:expr) => {
        #[no_mangle]
        pub unsafe extern "C" fn $name(pa: *mut u32, ot: *mut u32, otlen: i32, id: i32, backc: i32) {
            // SAFETY: as rot_pmd.
            unsafe { rot_pmd($t, pa, core::ptr::null(), ot, otlen, id, backc) }
        }
    };
}

macro_rules! rot_pmd_sv_fn {
    ($name:ident, $t:expr) => {
        #[no_mangle]
        pub unsafe extern "C" fn $name(pa: *mut u32, va: *const u8, ot: *mut u32, otlen: i32, id: i32, backc: i32) {
            // SAFETY: as rot_pmd.
            unsafe { rot_pmd($t, pa, va, ot, otlen, id, backc) }
        }
    };
}

rot_pmd_fn!(RotPMD_FT3, 0);
rot_pmd_fn!(RotPMD_FT4, 1);
rot_pmd_fn!(RotPMD_GT3, 2);
rot_pmd_fn!(RotPMD_GT4, 3);
rot_pmd_fn!(RotPMD_F3, 4);
rot_pmd_fn!(RotPMD_F4, 5);
rot_pmd_fn!(RotPMD_G3, 6);
rot_pmd_fn!(RotPMD_G4, 7);
rot_pmd_sv_fn!(RotPMD_SV_FT3, 8);
rot_pmd_sv_fn!(RotPMD_SV_FT4, 9);
rot_pmd_sv_fn!(RotPMD_SV_GT3, 10);
rot_pmd_sv_fn!(RotPMD_SV_GT4, 11);
rot_pmd_sv_fn!(RotPMD_SV_F3, 12);
rot_pmd_sv_fn!(RotPMD_SV_F4, 13);
rot_pmd_sv_fn!(RotPMD_SV_G3, 14);
rot_pmd_sv_fn!(RotPMD_SV_G4, 15);
