//! Movie playback for the games: a C entry over the SDK's `psx-fmv` player.
//!
//! This is not a Psy-Q function (NMS's 1996 builds played their films with
//! their own player); it is the restoration's way to show a film NMS made,
//! converted to the SDK's STR format by `tools/x360_film.py`. Declared in
//! `psyq/include/psxfmv.h`.

use psx_fmv::player::{self, Config, Depth, Movie, Stop, DEFAULT_BUFFERS};

fn movie(frames: u32, width: u32, height: u32, max_chunks: u32, rle_words: u32, lba: u32) -> Movie {
    Movie {
        lba,
        frames,
        width: width as u16,
        height: height as u16,
        max_chunks: max_chunks as u16,
        rle_words: rle_words as usize,
        xa: None,
    }
}

/// Bytes of memory `FmvPlay` needs for this movie (15-bit picture).
#[no_mangle]
pub extern "C" fn FmvMemoryBytes(width: u32, height: u32, max_chunks: u32, rle_words: u32) -> u32 {
    4 * player::memory_words(&movie(0, width, height, max_chunks, rle_words, 0), Depth::Rgb15) as u32
}

/// Play the movie in the root file `name` (an 8.3 name, no version suffix) from
/// the disc until it ends or the pad word `pad` (the `data.pad` of a
/// `ControllerPacket`, active low, refreshed every VBlank) shows a button of
/// `skip_mask`. `mem` is `FmvMemoryBytes` bytes, word aligned, free for the
/// duration. Returns 0 when it ended, 1 when skipped, 2 when it could not
/// play (file not found, setup, drive or decoder failure).
#[no_mangle]
pub unsafe extern "C" fn FmvPlay(
    name: *const u8,
    frames: u32,
    width: u32,
    height: u32,
    max_chunks: u32,
    rle_words: u32,
    mem: *mut u32,
    pad: *const u8,
    skip_mask: u32,
) -> i32 {
    let mut len = 0;
    // SAFETY: the game passes a NUL-terminated name.
    while unsafe { *name.add(len) } != 0 {
        len += 1;
    }
    // SAFETY: as above, `len` bytes of it.
    let name = match core::str::from_utf8(unsafe { core::slice::from_raw_parts(name, len) }) {
        Ok(n) => n,
        Err(_) => return 2,
    };
    crate::console::puts("psyq: FmvPlay start\n");
    let Some((lba, _bytes)) = player::find_root_file(name) else {
        crate::console::puts("psyq: FmvPlay: file not found\n");
        return 2;
    };
    // The movie is 320 pixels wide: a 320 by 240 picture on the TV area
    // 360X's own screens use (the PAL letterbox of GENERAL.H when PAL).
    let pal = crate::gpu::video_mode() == 1;
    let y0 = if pal { 19 + 28 } else { 16 };
    psx_io::gpu::write_display_control(0x0600_0000 | 608 | ((608 + 2560) << 12));
    psx_io::gpu::write_display_control(0x0700_0000 | y0 | ((y0 + 240) << 10));
    psx_io::gpu::write_display_control(0x0800_0000 | 1 | (u32::from(pal) << 3));
    let m = movie(frames, width, height, max_chunks, rle_words, lba);
    let words = player::memory_words(&m, Depth::Rgb15);
    crate::console::puts("psyq: FmvPlay memory bytes=");
    num(words as u32 * 4);
    crate::console::puts("\n");
    // SAFETY: the game lends FmvMemoryBytes bytes at `mem`.
    let memory = unsafe { core::slice::from_raw_parts_mut(mem, words) };
    let config = Config {
        depth: Depth::Rgb15,
        buffers: DEFAULT_BUFFERS,
        screen_width: 320,
        screen_height: 240,
        latency_vblanks: 3,
        stall_vblanks: 120,
    };
    let mut poll = || {
        // SAFETY: the pad buffer is the game's `ControllerPacket`; the
        // VBlank handler rewrites it, so read it volatile. Byte 0 is 0 when
        // a pad answers, bytes 2 and 3 the buttons, active low.
        unsafe {
            if core::ptr::read_volatile(pad) != 0 {
                return false;
            }
            let lo = core::ptr::read_volatile(pad.add(2)) as u32;
            let hi = core::ptr::read_volatile(pad.add(3)) as u32;
            (!(lo | hi << 8)) & skip_mask & 0xffff != 0
        }
    };
    // RESTORE: PLATFORM. The player streams with a drive of its own: it holds the drive as the scene
    // core's stream client while it plays (core_shim.rs), so no scene image loads meanwhile.
    if crate::core_shim::core_client_attach() != 0 {
        return 2;
    }
    let out = player::play(&m, &config, memory, &mut poll);
    crate::core_shim::core_client_detach();
    // One line on the debug console, as the layer does for what it stubs.
    crate::console::puts("psyq: FmvPlay stop=");
    num(out.stop as u32);
    for (label, v) in [
        (" shown=", out.shown),
        (" skipped=", out.skipped),
        (" late=", out.late),
        (" dropped=", out.dropped),
        (" decode_errors=", out.decode_errors),
        (" last_frame=", out.last_frame),
        (" vblanks=", out.vblanks),
    ] {
        crate::console::puts(label);
        num(v);
    }
    crate::console::puts("\n");
    match out.stop {
        Stop::End => 0,
        Stop::Skipped => 1,
        _ => 2,
    }
}

/// A decimal number on the debug console.
fn num(mut v: u32) {
    let mut digits = [0u8; 10];
    let mut n = 0;
    loop {
        digits[n] = b'0' + (v % 10) as u8;
        n += 1;
        v /= 10;
        if v == 0 {
            break;
        }
    }
    while n > 0 {
        n -= 1;
        crate::console::putc(digits[n]);
    }
}
