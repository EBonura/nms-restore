//! NMS's Escher interface: every E_* function ENGINE.C stubs on the PS1,
//! with the C signature ESCHER.H/ENGINE.C give it. ESCHER.S keeps E_Init,
//! E_AddMesh, E_Load and E_GetObjectPtr (NMS's loader). The calls that only
//! made sense for the PC's software renderer (dirty zones, VGA copies, VESA
//! banks, colour remaps, the felt texture switch) do nothing here: the PS1
//! draws the whole scene into an ordering table every frame.

use crate::camera::camera;
use crate::fixed::{apply, cos16, mul12, rotation, sin16};
use crate::nms::{Object, Vertex, OBJECT_DUMMY, OBJECT_MOBILE, OBJECT_MOBILE_COMPOUND};
use crate::render::{self, screen, view};
use crate::{balls, lines, mesh};
use psx_gte::{read_data, write_control, write_data};

// ------------------------------------------------------------ the frame

/// New for the PS1 (not an Escher call): the ordering table this frame's
/// render calls draw into. MY3DS.C's Render (3D build) calls it after
/// DrawScreen has started the frame and before draw_3d.
#[no_mangle]
pub unsafe extern "C" fn E_SetOrderingTable(ot: *mut u32, size: i32) {
    render::end_stats();
    render::begin_frame(ot, size);
}

/// 1 = the 3D view, 0 = NMS's 2D view (set by `E_SetView3D`, which is new for
/// the PS1: not an Escher call). With the 2D view the game draws its own
/// frame, so the render calls draw nothing and the camera's easing counts as
/// finished at once (the 2D build had no camera). All the maths stays real, so
/// the game can switch views at any time.
static mut VIEW3D: i32 = 1;

/// Choose the view: 1 for 3D, 0 for the 2D view.
#[no_mangle]
pub extern "C" fn E_SetView3D(on: i32) {
    // SAFETY: the guest is single-threaded and nothing holds a reference to the static.
    unsafe { VIEW3D = on };
}

fn view3d() -> bool {
    // SAFETY: as in `E_SetView3D`.
    unsafe { VIEW3D != 0 }
}

// ------------------------------------------------------------ set-up

#[no_mangle]
pub extern "C" fn E_LoadMapData() {
    mesh::load_map_data();
    crate::select::forget();
    lines::reset();
    balls::mark_dirty();
}

#[no_mangle]
pub extern "C" fn E_SetVesaShift(_shift_amount: i32) {}

#[no_mangle]
pub extern "C" fn E_SetScreenSize(width: i32, height: i32) {
    let s = screen();
    s.w = width.max(1);
    s.h = height.max(1);
}

#[no_mangle]
pub extern "C" fn E_SetScanlineLength(_length: i32) {}

#[no_mangle]
pub extern "C" fn E_SetScreenCentre(centre_x: i32, centre_y: i32) {
    let s = screen();
    s.cx = centre_x;
    s.cy = centre_y;
}

#[no_mangle]
pub extern "C" fn E_SetPixelAspect(pixel_squish: i32) {
    screen().aspect = pixel_squish;
}

#[no_mangle]
pub extern "C" fn E_SetZoom(zoom_value: i32) {
    screen().zoom = zoom_value;
}

#[no_mangle]
pub unsafe extern "C" fn E_UseBrightnessTable(brightness_table: *mut u8) {
    // SAFETY: foreground-only static.
    unsafe { render::BRIGHT = brightness_table };
}

#[no_mangle]
pub extern "C" fn E_RemapColours(_graphics_ptr: *mut u8, _length: i32) {}

#[no_mangle]
pub extern "C" fn E_SetTexture(_texture_address: *mut u8) {}

#[no_mangle]
pub extern "C" fn E_EnableFelt(_on_off: i32) {}

/// The pool palette table (POOLPAL.TAB, 11 x 256 colour indices) the game
/// loads and reads ball colours from (`poolptr[2560 + c]`).
#[no_mangle]
pub extern "C" fn E_PoolTabPtr() -> *mut u8 {
    crate::pool::pooltab()
}

// ------------------------------------------------------------ camera

#[no_mangle]
pub extern "C" fn E_EaseCamera(on_off: i32) {
    camera().ease = on_off;
}

#[no_mangle]
pub extern "C" fn E_FinishedEase() -> i32 {
    i32::from(!camera().easing || !view3d())
}

#[no_mangle]
pub unsafe extern "C" fn E_PositionCamera(target: *mut Vertex, rot: *mut Vertex, distance: i32) {
    if target.is_null() || rot.is_null() {
        return;
    }
    // SAFETY: the game's VERTEXes.
    unsafe { camera().position(&*target, &*rot, distance) };
}

#[no_mangle]
pub extern "C" fn E_UpdateCamera() {
    camera().update();
}

#[no_mangle]
pub unsafe extern "C" fn E_GetCameraPos(cam_pos: *mut Vertex) {
    if !cam_pos.is_null() {
        // SAFETY: the game's VERTEX.
        unsafe { *cam_pos = camera().pos };
    }
}

#[no_mangle]
pub unsafe extern "C" fn E_GetCameraRot(cam_rot: *mut Vertex) {
    if !cam_rot.is_null() {
        // SAFETY: the game's VERTEX.
        unsafe { *cam_rot = camera().rot };
    }
}

// ------------------------------------------------------------ maths

#[no_mangle]
pub extern "C" fn E_Cos(angle: i32) -> i32 {
    cos16(angle)
}

#[no_mangle]
pub extern "C" fn E_Sin(angle: i32) -> i32 {
    sin16(angle)
}

#[no_mangle]
pub unsafe extern "C" fn E_RotateVertex(xrot: i32, yrot: i32, zrot: i32, v: *mut Vertex) {
    if v.is_null() {
        return;
    }
    // SAFETY: the game's VERTEX.
    unsafe { *v = apply(&rotation(xrot, yrot, zrot), &*v) };
}

// ------------------------------------------------------------ objects

fn for_selected(f: impl Fn(&mut Object)) {
    crate::select::each(f);
}

#[no_mangle]
pub unsafe extern "C" fn E_SelectObject(name: *mut u8) {
    if !name.is_null() {
        crate::select::select(name);
    }
}

#[no_mangle]
pub extern "C" fn E_EnableObjects(on_off: i32) {
    for_selected(|o| o.enabled = u8::from(on_off != 0));
}

#[no_mangle]
pub extern "C" fn E_MakeMobile() {
    for_selected(|o| o.object_type = OBJECT_MOBILE);
}

#[no_mangle]
pub extern "C" fn E_MakeMobileCompound() {
    for_selected(|o| o.object_type = OBJECT_MOBILE_COMPOUND);
}

#[no_mangle]
pub extern "C" fn E_MakeDummy() {
    for_selected(|o| o.object_type = OBJECT_DUMMY);
}

#[no_mangle]
pub unsafe extern "C" fn E_MoveObject(x: i32, y: i32, z: i32, object: *mut Object) {
    if let Some(o) = unsafe { object.as_mut() } {
        o.modified_pos = Vertex { x, y, z };
    }
}

/// Scale the mesh about its origin (16.16); the bounding radius stays as
/// the game set it (init_3d sets the ball radius, then scales the mesh to
/// match it).
#[no_mangle]
pub unsafe extern "C" fn E_ScaleObject(obj_ptr: *mut Object, scale_factor: i32) {
    let Some(o) = (unsafe { obj_ptr.as_mut() }) else { return };
    let s = scale_factor >> 4;
    for list in [o.sources(), o.vertices()] {
        for v in list.iter_mut() {
            *v = Vertex { x: mul12(s, v.x), y: mul12(s, v.y), z: mul12(s, v.z) };
        }
    }
}

/// modified_vertex = R(x, y, z) source_vertex, on the GTE (MVMVA).
#[no_mangle]
pub unsafe extern "C" fn E_RotateObject(xrot: i32, yrot: i32, zrot: i32, object: *mut Object) {
    let Some(o) = (unsafe { object.as_mut() }) else { return };
    o.rotation = Vertex { x: xrot, y: yrot, z: zrot };
    let src = o.sources();
    let dst = o.vertices();
    if (xrot | yrot | zrot) & 4095 == 0 {
        dst.copy_from_slice(src);
        return;
    }
    let m = rotation(xrot, yrot, zrot);
    let p = |a: i32, b: i32| (a as u16 as u32) | ((b as u16 as u32) << 16);
    write_control!(0, p(m[0][0], m[0][1]));
    write_control!(1, p(m[0][2], m[1][0]));
    write_control!(2, p(m[1][1], m[1][2]));
    write_control!(3, p(m[2][0], m[2][1]));
    write_control!(4, m[2][2] as u32);
    write_control!(5, 0);
    write_control!(6, 0);
    write_control!(7, 0);
    // inputs are 16-bit: shift long meshes (the cue is 270808 units long)
    let mut shift = 0;
    while (o.radius.max(1) * 2) >> shift >= 32767 && shift < 12 {
        shift += 1;
    }
    for (s, d) in src.iter().zip(dst.iter_mut()) {
        write_data!(0, p(s.x >> shift, s.y >> shift));
        write_data!(1, (s.z >> shift) as u32);
        // SAFETY: V0, RT and TR loaded.
        unsafe { psx_gte::ops::rotate_translate_v0() };
        *d = Vertex {
            x: (read_data!(25) as i32) << shift,
            y: (read_data!(26) as i32) << shift,
            z: (read_data!(27) as i32) << shift,
        };
    }
}

/// world_pos = the object's origin in camera space (x right, y up, z ahead).
#[no_mangle]
pub unsafe extern "C" fn E_WorldRotateObject(obj_ptr: *mut Object) {
    let Some(o) = (unsafe { obj_ptr.as_mut() }) else { return };
    o.world_pos = view().to_camera(&o.modified_pos).unwrap_or(Vertex { x: 0, y: 0, z: -(1 << 24) });
}

/// The object's world_pos on the PC screen the game thinks in: x and y in
/// sixteenths of a pixel, z the projected radius in sixteenths; 0 when it
/// is in front of the camera, 1 when not.
#[no_mangle]
pub unsafe extern "C" fn E_GetScreenPos(obj: *mut Object, pos: *mut Vertex) -> i32 {
    let (Some(o), Some(p)) = (unsafe { obj.as_ref() }, unsafe { pos.as_mut() }) else { return 1 };
    let c = o.world_pos;
    if c.z < 256 {
        return 1;
    }
    let s = screen();
    let f = s.focal();
    let fy = (f * s.aspect) >> 16;
    let (x, y, z) = (c.x >> 7, c.y >> 7, (c.z >> 7).max(1));
    if x.abs() >= 1 << 16 || y.abs() >= 1 << 16 {
        return 1;
    }
    p.x = s.cx * 16 + x * f * 16 / z;
    p.y = s.cy * 16 - y * fy * 16 / z;
    p.z = if o.radius < 1 << 16 { o.radius * f * 16 / c.z } else { (o.radius >> 7) * f * 16 / z };
    0
}

#[no_mangle]
pub unsafe extern "C" fn E_GetObjectExtents(obj_ptr: *mut Object, min: *mut Vertex, max: *mut Vertex) {
    let Some(o) = (unsafe { obj_ptr.as_ref() }) else { return };
    let mut lo = Vertex { x: i32::MAX, y: i32::MAX, z: i32::MAX };
    let mut hi = Vertex { x: i32::MIN, y: i32::MIN, z: i32::MIN };
    for v in o.vertices().iter() {
        lo = Vertex { x: lo.x.min(v.x), y: lo.y.min(v.y), z: lo.z.min(v.z) };
        hi = Vertex { x: hi.x.max(v.x), y: hi.y.max(v.y), z: hi.z.max(v.z) };
    }
    if o.num_vertices <= 0 {
        lo = Vertex::default();
        hi = Vertex::default();
    }
    let p = o.modified_pos;
    // SAFETY: the game's VERTEXes.
    unsafe {
        if let Some(m) = min.as_mut() {
            *m = Vertex { x: p.x + lo.x, y: p.y + lo.y, z: p.z + lo.z };
        }
        if let Some(m) = max.as_mut() {
            *m = Vertex { x: p.x + hi.x, y: p.y + hi.y, z: p.z + hi.z };
        }
    }
}

// ------------------------------------------------------------ drawing

#[no_mangle]
pub extern "C" fn E_RenderToScreen(_rotate_on_off: i32, _balls_on_off: i32) {
    if view3d() {
        render::render_pass();
    }
}

#[no_mangle]
pub extern "C" fn E_RenderToBuffer(_buffer_address: *mut u8, _rotate_on_off: i32, _balls_on_off: i32) {
    if view3d() {
        render::render_pass();
    }
}

#[no_mangle]
pub extern "C" fn E_RenderPos(_y: i32) {}

#[no_mangle]
pub extern "C" fn E_DrawBall(x: i32, y: i32, radius: i32, _backscreen: *mut u8, colour: i32, shape: i32, hilite_on_off: i32) {
    if view3d() {
        balls::draw(x, y, radius, colour, shape, hilite_on_off);
    }
}

#[no_mangle]
pub extern "C" fn E_ModifyBall(
    _x: i32,
    _y: i32,
    _radius: i32,
    _colour: i32,
    _hilite_on_off: i32,
    _on_off: i32,
    _z: i32,
    _shape_scale: i32,
    _ball_number: i32,
) {
}

#[no_mangle]
pub extern "C" fn E_SortBalls() {}

#[no_mangle]
pub extern "C" fn E_Reset2DLines() {
    lines::reset();
}

#[no_mangle]
pub extern "C" fn E_Draw2DLine(x1: i32, y1: i32, x2: i32, y2: i32, colour: i32, sort_value: i32) {
    if view3d() {
        lines::add(x1, y1, x2, y2, colour, sort_value);
    }
}

// ------------------------------------------------- the PC's dirty zones

#[no_mangle]
pub extern "C" fn E_ClearZones() {}

#[no_mangle]
pub extern "C" fn E_AddZone(_x1: i32, _y1: i32, _x2: i32, _y2: i32) {}

#[no_mangle]
pub extern "C" fn E_MergeZones() {}

#[no_mangle]
pub extern "C" fn E_BuildChunks() {}

#[no_mangle]
pub extern "C" fn E_CopyRamToRam(_source_buffer: *mut u8, _dest_buffer: *mut u8) {}

#[no_mangle]
pub extern "C" fn E_CopyRamToVga(_ram_buffer: *mut u8) {}

#[no_mangle]
pub extern "C" fn E_CopyVgaToRam(_ram_buffer: *mut u8) {}

#[no_mangle]
pub extern "C" fn E_SetZoneBank(_bank_num: i32) {}
