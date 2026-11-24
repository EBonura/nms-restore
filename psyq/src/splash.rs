//! The boot splash: the SDK's studio splash with the PSX Archive logo beside the mark, the game's
//! line and the credit line under them.
//!
//! RESTORE: PLATFORM (owner's decisions: 2026-11-13, "Standard splash, new line"; 2026-11-22,
//! "rebuilt with PSoXide in collaboration with PSX Archive", the two "on the same screen"; 2026-11-23,
//! one splash, the SDK's). Not a Psy-Q function. The game calls `PsxSplash` once at boot, after its
//! pad is started and its video mode is set. Declared in `psyq/include/psxsplash.h`.
//!
//! The screen is the SDK's `psx_splash::show_with_partner`: its mark, timing, skip rule (a fresh press
//! of Cross, Circle or Start after the first 8 frames), brightness and animation. The layer gives it
//! the partner: the PSX Archive logo, packed from `psyq/art/psxarchive-logo.png` by
//! `tools/psxarchive_logo.py` at build time, and the line "in collaboration with PSX Archive".
//!
//! It is boot-only code (PLAN.md "Splash: one mechanism"): its code, the SDK's splash and the logo are the
//! boot image the EXE carries (scenes.txt `boot`, tools/scenes.py), loaded in the same read as the EXE,
//! moved to the start of the heap by `__core_boot` and run once there; the heap then takes its memory, so
//! nothing of it stays resident. Only a disc's first program plays it: a program the scene core switched to
//! (`core_shim::entered_by_core`) does not.

use psx_display::Brightness;
use psx_gpu::display::{DisplayConfig, DoubleBuffer, Resolution, VideoMode};
use psx_gpu::Gpu;
use psx_io::periph::GpuDma;

#[repr(C, align(4))]
struct Packed<T: ?Sized>(T);

/// The PSX Archive logo: width and height, a 256-colour palette, then one byte a pixel
/// (`tools/psxarchive_logo.py`).
const LOGO_LEN: usize = include_bytes!("../../build/psxarchive.bin").len();
#[link_section = ".boot_only.splash_logo"]
static LOGO: Packed<[u8; LOGO_LEN]> = Packed(*include_bytes!("../../build/psxarchive.bin"));

#[link_section = ".boot_only.splash_text"]
static CREDIT: [u8; 33] = *b"in collaboration with PSX Archive";

/// Play the splash with the NUL-terminated `line` (the game's "Rebuilt with PSoXide") and return
/// when it has run or a press of Cross, Circle or Start has ended it. The pad is the one the game
/// gave `InitPAD` (port 1), as the VBlank poll fills it; a game that shows the splash before
/// `InitPAD` gets a buffer of the layer's own, polled the same way.
///
/// Takes over the GPU (reset, 320 by 240) and leaves it showing a black frame; the caller sets its
/// own display up afterwards, as it does after `FmvPlay`. VRAM is left as the SDK's splash used it
/// (its doc lists the columns), which a game's own uploads overwrite.
#[no_mangle]
pub unsafe extern "C" fn PsxSplash(line: *const u8) {
    // Only a disc's first program plays the splash: one the scene core switched to does not play it again.
    // The display on such an entry is the SDK's (the loading frame stays up until the first frame, the
    // film turns the display on itself).
    if crate::core_shim::entered_by_core() {
        return;
    }
    let mut len = 0;
    // SAFETY: the game passes a NUL-terminated string.
    while unsafe { *line.add(len) } != 0 {
        len += 1;
    }
    // SAFETY: as above, `len` bytes of it.
    let Ok(line) = core::str::from_utf8(unsafe { core::slice::from_raw_parts(line, len) }) else {
        return;
    };
    // The splash waits on psx-rt's VBlank counter.
    crate::kernel::ensure_interrupts();
    crate::gpu::wait_idle();
    let mode = if crate::gpu::video_mode() == 1 { VideoMode::Pal } else { VideoMode::Ntsc };
    // SAFETY: this layer is the only driver of GP0 and DMA channel 2 (gpu.rs).
    let mut gpu = Gpu::new(unsafe { GpuDma::steal() }, DisplayConfig::new(mode, Resolution::R320X240));
    let mut buffers = DoubleBuffer::new(Resolution::R320X240);
    buffers.apply_draw_target(&mut gpu);
    // SAFETY: LOGO is word aligned and every part of it a whole number of 16-bit words.
    let logo: &[u16] = unsafe { core::slice::from_raw_parts(LOGO.0.as_ptr().cast::<u16>(), LOGO_LEN / 2) };
    let palette: &[u16; 256] = logo[2..258].try_into().unwrap_or(&[0; 256]);
    // SAFETY: ASCII.
    let credit = unsafe { core::str::from_utf8_unchecked(&CREDIT) };
    let partner = psx_splash::Partner::new(credit, logo[0], logo[1], palette, &logo[258..]);
    let early = crate::kernel::early_pad_begin();
    psx_splash::show_with_partner(&mut gpu, &mut buffers, line, &partner, Brightness::DEFAULT, crate::kernel::pad1_buttons);
    if early {
        crate::kernel::early_pad_end();
    }
}
