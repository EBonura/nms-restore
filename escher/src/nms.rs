//! NMS's data: the structures ESCHER.H declares and ESCHER.S's loader
//! fills, and the symbols the renderer shares with the game.
//!
//! The layouts are ESCHER.H's (and the `rs` offsets in ESCHER.S, which agree
//! with it: OBJECT is 132 bytes, PLANE 48, LINE 56).

/// ESCHER.H `VERTEX`.
#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct Vertex {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

/// ESCHER.H `LINE`: an edge between two vertices, with the planes on its
/// two sides (-1: none) and the texture coordinates on each.
#[repr(C)]
pub struct Line {
    pub vertex: [i32; 2],
    pub plane: [i32; 2],
    pub orientation: [i32; 2],
    pub uv: [i32; 8],
}

/// ESCHER.H `PLANE`: a convex or concave polygon of the mesh. `axis` is the
/// plane's dominant axis (0 x, 1 y, 2 z) and `visible_side` the side it is
/// seen from along that axis (2: from +, 1: from -).
#[repr(C)]
pub struct Plane {
    pub axis: u8,
    pub plane_type: u8,
    pub visible_side: u8,
    pub current_side: u8,
    pub colour: i32,
    pub brightness: i32,
    pub tile_info: i32,
    pub gradient: [i32; 2],
    pub offset: i32,
    pub radius: i32,
    pub centre_vertex: i32,
    pub priority: i32,
    pub vertex_start: i32,
    pub num_vertices: i32,
}

/// ESCHER.H `OBJECT`.
#[repr(C)]
pub struct Object {
    pub num_vertices: i32,
    pub first_extra_vertex: i32,
    pub num_lines: i32,
    pub num_planes: i32,
    pub radius: i32,
    pub priority: i32,
    pub num_plane_vertices: i32,
    /// Escher's scratch; this renderer keeps the object's first entry in
    /// its plane table here (see `mesh`).
    pub first_sorted_plane: i32,
    pub last_sorted_plane: i32,
    pub source_pos: Vertex,
    pub modified_pos: Vertex,
    pub world_pos: Vertex,
    pub rotation: Vertex,
    pub source_vertex: *mut Vertex,
    pub modified_vertex: *mut Vertex,
    pub world_vertex: *mut Vertex,
    pub lines: *mut Line,
    pub planes: *mut Plane,
    pub plane_vertex: *mut i16,
    pub object_name: [u8; 16],
    pub colour: u8,
    pub visible: u8,
    pub selected: u8,
    pub sort_type: u8,
    pub enabled: u8,
    pub object_type: u8,
}

const _: () = assert!(core::mem::size_of::<Object>() == 132);
const _: () = assert!(core::mem::size_of::<Plane>() == 48);
const _: () = assert!(core::mem::size_of::<Line>() == 56);

pub const OBJECT_MOBILE: u8 = 1;
pub const OBJECT_DUMMY: u8 = 2;
pub const OBJECT_MOBILE_COMPOUND: u8 = 4;

/// PSX.H `RECT`.
#[repr(C)]
pub struct Rect {
    pub x: i16,
    pub y: i16,
    pub w: i16,
    pub h: i16,
}

extern "C" {
    // ESCHER.S (NMS's loader), exported for this renderer
    pub static mut objects: *mut Object;
    pub static mut num_objects: i32;
    pub static mut mesh_names: [u8; 1024];
    pub static mut mesh_name_ptr: *mut u8;
    pub static mut edge_mem: *mut u8;
    pub static mut edge_start_mem: *mut u8;
    /// COSINE.TAB, loaded by E_Init: cos(i * 2pi / 4096) * 65536.
    pub static cosine: [i32; 4096];

    /// GLOBALS.H: the game's VGA palette (6-bit components), what the PC
    /// version programmed the DAC with; the 3D build loads the room's here.
    pub static palette: [u8; 768];

    // FILE_IO.C: the game's own file access (PC link or CD)
    pub fn FileOpen(name: *const u8, mode: i32, attr: i32) -> i32;
    pub fn FileRead(handle: i32, dest: *mut u8, length: i32) -> i32;
    pub fn FileClose(handle: i32) -> i32;

    // the Psy-Q layer
    pub fn LoadImage(rect: *const Rect, p: *const u8) -> i32;
}

/// NMS's object list, as E_Load left it.
pub fn object_list() -> &'static mut [Object] {
    // SAFETY: ESCHER.S's `objects` holds `num_objects` OBJECTs once E_Load has
    // run; before that the count is 0. The game is single-threaded and the
    // renderer is only called from it.
    unsafe {
        let n = num_objects;
        if objects.is_null() || n <= 0 {
            return &mut [];
        }
        core::slice::from_raw_parts_mut(objects, n as usize)
    }
}

impl Object {
    pub fn vertices(&self) -> &'static mut [Vertex] {
        // SAFETY: E_Load allocated num_vertices (+3) of each list.
        unsafe { core::slice::from_raw_parts_mut(self.modified_vertex, self.num_vertices.max(0) as usize) }
    }
    pub fn sources(&self) -> &'static mut [Vertex] {
        // SAFETY: as vertices.
        unsafe { core::slice::from_raw_parts_mut(self.source_vertex, self.num_vertices.max(0) as usize) }
    }
    pub fn planes(&self) -> &'static mut [Plane] {
        // SAFETY: E_Load read num_planes PLANEs here.
        unsafe { core::slice::from_raw_parts_mut(self.planes, self.num_planes.max(0) as usize) }
    }
    pub fn lines(&self) -> &'static [Line] {
        // SAFETY: E_Load read num_lines LINEs here.
        unsafe { core::slice::from_raw_parts(self.lines, self.num_lines.max(0) as usize) }
    }
    pub fn drawn(&self) -> bool {
        self.enabled != 0 && self.visible != 0 && self.object_type != OBJECT_DUMMY
    }
}

/// `name` matches `pattern` as Escher's StringCompare (ESCHER.S) does: `*`
/// ends the pattern with a match, `?` matches any one character.
pub fn name_matches(pattern: *const u8, name: &[u8; 16]) -> bool {
    // SAFETY: as below; most names differ at the first character
    let p0 = unsafe { *pattern };
    if p0 != name[0] && p0 != b'*' && p0 != b'?' {
        return false;
    }
    let mut i = 0;
    loop {
        // SAFETY: a NUL-terminated C string from the game.
        let p = unsafe { *pattern.add(i) };
        if p == b'*' {
            return true;
        }
        let n = if i < 16 { name[i] } else { 0 };
        if p != b'?' && p != n {
            return false;
        }
        if p == 0 {
            return true;
        }
        i += 1;
    }
}
