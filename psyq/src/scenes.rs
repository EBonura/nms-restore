//! RESTORE: PLATFORM. Scene images: the game's front end and its game proper load, one at a time, into
//! one region of RAM (tools/scenes.py, docs/DECISIONS.md "Scene images").
//!
//! The base calls each image only through its entry functions, and the link sends those calls
//! (`ld --wrap`) through `__wrap_<entry>` (tools/scenes.py makes them), which calls
//! [`PsyqSceneEnter`] with the image's scene id and then the real function with the arguments it
//! was given. NMS's C is not changed for it.

use crate::core_shim::{
    core_current_scene, core_load_image, STAGE_BOUNDS, STAGE_CHECKSUM, STAGE_CLIENT, STAGE_NO_ROW, STAGE_READ_FAILED,
};

/// Make scene image `scene` the one in the region: nothing to do when it is already there, else
/// the seam loads it. A load that fails after its tries is the LOAD FAILED screen (`loadfail.rs`),
/// as a game file's is: nothing could run from the region.
#[no_mangle]
pub extern "C" fn PsyqSceneEnter(scene: u32) {
    if core_current_scene() == scene {
        return;
    }
    let step: &[u8] = match core_load_image(scene) {
        0 => return,
        STAGE_READ_FAILED => b"SCENE IMAGE READ\0",
        STAGE_CHECKSUM => b"SCENE IMAGE CHECKSUM\0",
        STAGE_NO_ROW | STAGE_BOUNDS => b"SCENE IMAGE ROW\0",
        STAGE_CLIENT => b"SCENE IMAGE DRIVE HELD\0",
        _ => b"SCENE TABLE\0",
    };
    // SAFETY: two NUL-terminated strings.
    unsafe { crate::loadfail::PsyqLoadFailed(b"SCENES.IMG\0".as_ptr(), step.as_ptr()) }
}
