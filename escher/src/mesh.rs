//! Load-time work on NMS's meshes: the map data (.EMP) E_LoadMapData reads,
//! and the outline and triangles of every plane, which the renderer draws.
//!
//! EMP layout (one per mesh, the .ESH name with .EMP; written by NMS's
//! tools, read here from the files and ESCHER.S's naming):
//!   int num_objects
//!   per object: char name[16]; int num_lines, num_planes, priority;
//!               int colour[num_planes], brightness[num_planes],
//!               tile_info[num_planes]; int uv[num_lines][8];
//!               char plane_type[num_planes]
//! A record is applied to the object of that name when its line and plane
//! counts agree with the mesh.
//!
//! Plane outlines: plane_vertex lists a plane's vertices, but not always in
//! order around it (160 of the Potters table's 702 planes are not), so the
//! outline is chained from the LINEs bordering the plane, oriented so that
//! its normal (right-hand rule) points to the plane's visible side, and
//! triangulated by ear clipping when it is concave (the number glyphs, a
//! few cabinet sides).

use crate::nms::{self, object_list, FileClose, FileOpen, FileRead, Object};
use crate::pool;

/// A plane's entry in the plane table (one u32 per plane, objects' runs
/// start at `Object::first_sorted_plane`): byte offset of the outline in the
/// index pool (bits 0..15), outline length (16..23), flags (24..31).
pub const PLANE_CONCAVE: u32 = 1 << 24;
pub const PLANE_SKIP: u32 = 1 << 25;

pub fn plane_entry(obj: &Object, k: usize) -> u32 {
    let t = pool::plane_table();
    let i = obj.first_sorted_plane as usize + k;
    if obj.first_sorted_plane < 0 || i >= t.len() {
        PLANE_SKIP
    } else {
        t[i]
    }
}

/// E_LoadMapData: apply each mesh's EMP, then build the plane table.
pub fn load_map_data() {
    pool::reset();
    // SAFETY: ESCHER.S's mesh list, 64 bytes a name, up to mesh_name_ptr.
    let names = unsafe {
        let start = (&raw const nms::mesh_names) as *const u8;
        let end = nms::mesh_name_ptr as *const u8;
        let n = if end.is_null() || end < start { 0 } else { (end.offset_from(start) as usize / 64).min(16) };
        core::slice::from_raw_parts(start, n * 64)
    };
    for mesh in names.chunks(64) {
        load_emp(mesh);
    }
    build_plane_table();
}

fn load_emp(mesh: &[u8]) {
    let mut name = [0u8; 80];
    let mut n = 0;
    while n < 63 && mesh[n] != 0 {
        name[n] = mesh[n];
        n += 1;
    }
    // .esh -> .emp (keep the case of the rest)
    if n < 4 || name[n - 4] != b'.' {
        return;
    }
    let upper = name[n - 3].is_ascii_uppercase();
    for (i, c) in b"emp".iter().enumerate() {
        name[n - 3 + i] = if upper { c.to_ascii_uppercase() } else { *c };
    }
    // SAFETY: a NUL-terminated name; FILE_IO.C reads one file at a time and
    // nothing else is open while the table is set up.
    unsafe {
        let h = FileOpen(name.as_ptr(), 0, 0);
        if h < 0 {
            return;
        }
        let mut count = 0i32;
        if FileRead(h, (&raw mut count).cast(), 4) == 4 {
            for _ in 0..count.clamp(0, 1024) {
                if !emp_record(h) {
                    break;
                }
            }
        }
        FileClose(h);
    }
}

unsafe fn read_exact(h: i32, dest: *mut u8, len: usize) -> bool {
    // SAFETY: the caller's buffer of len bytes.
    len == 0 || unsafe { FileRead(h, dest, len as i32) } == len as i32
}

unsafe fn skip(h: i32, mut len: usize) -> bool {
    let mut buf = [0u8; 256];
    while len > 0 {
        let n = len.min(256);
        // SAFETY: a local buffer.
        if !unsafe { read_exact(h, buf.as_mut_ptr(), n) } {
            return false;
        }
        len -= n;
    }
    true
}

unsafe fn emp_record(h: i32) -> bool {
    #[repr(C)]
    struct Head {
        name: [u8; 16],
        num_lines: i32,
        num_planes: i32,
        priority: i32,
    }
    let mut head = Head { name: [0; 16], num_lines: 0, num_planes: 0, priority: 0 };
    // SAFETY: reads into local storage.
    unsafe {
        if !read_exact(h, (&raw mut head).cast(), 28) {
            return false;
        }
        let (nl, np) = (head.num_lines, head.num_planes);
        if !(0..=4096).contains(&nl) || !(0..=256).contains(&np) {
            return false;
        }
        let np = np as usize;
        let mut cols = [0i32; 256];
        let mut bris = [0i32; 256];
        let mut tiles = [0i32; 256];
        let mut types = [0u8; 256];
        if !read_exact(h, cols.as_mut_ptr().cast(), 4 * np)
            || !read_exact(h, bris.as_mut_ptr().cast(), 4 * np)
            || !read_exact(h, tiles.as_mut_ptr().cast(), 4 * np)
            || !skip(h, 32 * nl as usize)
            || !read_exact(h, types.as_mut_ptr(), np)
        {
            return false;
        }
        head.name[15] = 0;
        let Some(obj) = object_list().iter_mut().find(|o| cstr_eq(&o.object_name, &head.name)) else {
            return true;
        };
        if obj.num_lines != nl || obj.num_planes as usize != np {
            return true;
        }
        obj.priority = head.priority;
        for (k, p) in obj.planes().iter_mut().enumerate() {
            p.colour = cols[k];
            p.brightness = bris[k];
            p.tile_info = tiles[k];
            p.plane_type = types[k];
        }
    }
    true
}

fn cstr_eq(a: &[u8; 16], b: &[u8; 16]) -> bool {
    for i in 0..16 {
        if a[i] != b[i] {
            return false;
        }
        if a[i] == 0 {
            return true;
        }
    }
    true
}

/// 2D coordinates of a vertex seen along the plane's axis, in cyclic order
/// (x: y,z; y: z,x; z: x,y), so a positive area means a normal toward +axis.
fn flat(v: &nms::Vertex, axis: u8) -> (i32, i32) {
    match axis {
        0 => (v.y, v.z),
        1 => (v.z, v.x),
        _ => (v.x, v.y),
    }
}

fn build_plane_table() {
    let mut next_plane = 0usize;
    for obj in object_list().iter_mut() {
        let np = obj.num_planes.max(0) as usize;
        if next_plane + np > pool::MAX_PLANES {
            obj.first_sorted_plane = -1;
            continue;
        }
        obj.first_sorted_plane = next_plane as i32;
        for k in 0..np {
            pool::plane_table()[next_plane + k] = outline(obj, k);
        }
        next_plane += np;
    }
}

/// Chain plane k's outline from its bordering lines, orient it, and store it
/// (and its triangles when concave) in the index pool.
fn outline(obj: &Object, k: usize) -> u32 {
    let lines = obj.lines();
    let verts = obj.sources();
    let mut edges = [(0u8, 0u8); 128];
    let mut ne = 0;
    for l in lines {
        if (l.plane[0] == k as i32 || l.plane[1] == k as i32) && ne < 128 {
            let (a, b) = (l.vertex[0], l.vertex[1]);
            if a < 0 || b < 0 || a as usize >= verts.len() || b as usize >= verts.len() || a > 255 || b > 255 {
                return PLANE_SKIP;
            }
            edges[ne] = (a as u8, b as u8);
            ne += 1;
        }
    }
    if ne < 3 {
        return PLANE_SKIP;
    }
    let mut lp = [0u8; 128];
    let mut used = [false; 128];
    lp[0] = edges[0].0;
    lp[1] = edges[0].1;
    used[0] = true;
    let mut n = 2;
    while n < ne {
        let cur = lp[n - 1];
        let Some(e) = (0..ne).find(|&e| !used[e] && (edges[e].0 == cur || edges[e].1 == cur)) else {
            return PLANE_SKIP;
        };
        used[e] = true;
        lp[n] = if edges[e].0 == cur { edges[e].1 } else { edges[e].0 };
        n += 1;
    }
    let plane = &obj.planes()[k];
    let axis = plane.axis.min(2);
    // scale the 2D outline into 14 bits so the cross products fit 32 bits
    let mut pts = [(0i32, 0i32); 128];
    let (mut lo, mut hi) = ((i32::MAX, i32::MAX), (i32::MIN, i32::MIN));
    for i in 0..n {
        let p = flat(&verts[lp[i] as usize], axis);
        pts[i] = p;
        lo = (lo.0.min(p.0), lo.1.min(p.1));
        hi = (hi.0.max(p.0), hi.1.max(p.1));
    }
    let mut shift = 0;
    while ((hi.0 - lo.0) >> shift) >= (1 << 13) || ((hi.1 - lo.1) >> shift) >= (1 << 13) {
        shift += 1;
    }
    for p in pts.iter_mut().take(n) {
        *p = ((p.0 - lo.0) >> shift, (p.1 - lo.1) >> shift);
    }
    let mut area = 0i32;
    for i in 0..n {
        let (a, b) = (pts[i], pts[(i + 1) % n]);
        area += a.0 * b.1 - b.0 * a.1;
    }
    // normal toward the visible side (2: +axis, 1: -axis)
    let want_positive = plane.visible_side != 1;
    if area != 0 && (area > 0) != want_positive {
        lp[..n].reverse();
        pts[..n].reverse();
    }
    if !want_positive {
        // seen from -axis: mirror so the 2D tests below see it counter-clockwise
        for p in pts.iter_mut().take(n) {
            p.0 = -p.0;
        }
    }
    let convex = (0..n).all(|i| cross(pts[i], pts[(i + 1) % n], pts[(i + 2) % n]) >= 0);
    let tris = if convex { 0 } else { n - 2 };
    let Some(off) = pool::alloc_indices(n + 3 * tris) else {
        return PLANE_SKIP;
    };
    let dst = pool::indices();
    dst[off..off + n].copy_from_slice(&lp[..n]);
    if !convex {
        ear_clip(&pts[..n], &mut dst[off + n..off + n + 3 * tris]);
    }
    off as u32 | ((n as u32) << 16) | if convex { 0 } else { PLANE_CONCAVE }
}

fn cross(a: (i32, i32), b: (i32, i32), c: (i32, i32)) -> i32 {
    (b.0 - a.0) * (c.1 - a.1) - (b.1 - a.1) * (c.0 - a.0)
}

/// Triangles of a counter-clockwise simple polygon, as outline positions
/// (0..n) in `out`; any rest that clipping cannot resolve becomes a fan.
fn ear_clip(pts: &[(i32, i32)], out: &mut [u8]) {
    let n = pts.len();
    let mut idx = [0u8; 128];
    for (i, v) in idx.iter_mut().enumerate().take(n) {
        *v = i as u8;
    }
    let mut m = n;
    let mut t = 0;
    let mut guard = 0;
    while m > 3 && guard < 4 * n {
        guard += 1;
        let mut found = false;
        for k in 0..m {
            let (i0, i1, i2) = (idx[(k + m - 1) % m], idx[k], idx[(k + 1) % m]);
            let (a, b, c) = (pts[i0 as usize], pts[i1 as usize], pts[i2 as usize]);
            if cross(a, b, c) <= 0 {
                continue;
            }
            let inside = (0..m).any(|j| {
                let q = idx[j];
                q != i0 && q != i1 && q != i2 && {
                    let p = pts[q as usize];
                    cross(a, b, p) > 0 && cross(b, c, p) > 0 && cross(c, a, p) > 0
                }
            });
            if inside {
                continue;
            }
            out[3 * t..3 * t + 3].copy_from_slice(&[i0, i1, i2]);
            t += 1;
            for j in k..m - 1 {
                idx[j] = idx[j + 1];
            }
            m -= 1;
            found = true;
            break;
        }
        if !found {
            break;
        }
    }
    for k in 1..m - 1 {
        out[3 * t..3 * t + 3].copy_from_slice(&[idx[0], idx[k], idx[k + 1]]);
        t += 1;
    }
}
