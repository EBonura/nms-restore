//! The GPU: environments, ordering tables, primitives, VRAM transfers.
//!
//! Primitives are the GPU's command words behind an ordering-table tag (next
//! address in bits 0..23, payload words in 24..31), so a chain is exactly
//! what DMA channel 2 walks in linked-list mode. `DrawOTag` hands the chain
//! to psx-gpu's walker and returns; everything that writes GP0 directly
//! (`PutDrawEnv`, `DrawPrim`, `LoadImage`, the font) first waits for the
//! walk and the GPU to finish, which is where Psy-Q queued them instead.

use psx_hw::gpu::{gp0, gp1};
use psx_io::gpu::{wait_command_ready, wait_dma_ready, write_command, write_command_unguarded, write_display_control};
use psx_io::periph::{GpuDma, OrderingTableClearDma};

#[repr(C)]
#[derive(Clone, Copy)]
pub struct Rect {
    pub x: i16,
    pub y: i16,
    pub w: i16,
    pub h: i16,
}

#[repr(C)]
pub struct DrawEnv {
    pub clip: Rect,
    pub ofs: [i16; 2],
    pub tw: Rect,
    pub tpage: u16,
    pub dtd: u8,
    pub dfe: u8,
    pub isbg: u8,
    pub r0: u8,
    pub g0: u8,
    pub b0: u8,
    pub dr_env: [u32; 16],
}

#[repr(C)]
pub struct DispEnv {
    pub disp: Rect,
    pub screen: Rect,
    pub isinter: u8,
    pub isrgb24: u8,
    pub pad0: u8,
    pub pad1: u8,
}

static mut VIDEO_MODE: i32 = 0; // MODE_NTSC

pub fn video_mode() -> i32 {
    // SAFETY: foreground-only static, aligned word.
    unsafe { core::ptr::read_volatile(&raw const VIDEO_MODE) }
}

#[no_mangle]
pub extern "C" fn SetVideoMode(mode: i32) -> i32 {
    let old = video_mode();
    // SAFETY: as video_mode.
    unsafe { core::ptr::write_volatile(&raw mut VIDEO_MODE, mode & 1) };
    old
}

#[no_mangle]
pub extern "C" fn GetVideoMode() -> i32 {
    video_mode()
}

fn gpu_dma() -> GpuDma {
    // SAFETY: this layer is the only driver of DMA channel 2 and GP0; the
    // token is a logic guard (psx_io::periph), not a memory-safety one.
    unsafe { GpuDma::steal() }
}

// ------------------------------------------------------- DrawSyncCallback

static mut DRAW_SYNC_CALLBACK: Option<extern "C" fn()> = None;
/// An ordering table handed to DrawOTag whose walk has not been seen to end.
static mut WALK_IN_FLIGHT: bool = false;
/// Foreground code writing to the GPU right now (nesting count).
static mut GPU_LOCK: u32 = 0;
/// The walk ended while the foreground held the GPU: call the function when
/// it lets go.
static mut CALLBACK_DEFERRED: bool = false;

/// Held while this layer writes to the GPU from the foreground, so the
/// DrawSync callback (which may load images) never lands in the middle.
struct GpuLock;

impl GpuLock {
    fn take() -> Self {
        // SAFETY: foreground-only writer; the interrupt only reads it.
        unsafe { core::ptr::write_volatile(&raw mut GPU_LOCK, core::ptr::read_volatile(&raw const GPU_LOCK) + 1) };
        GpuLock
    }
}

impl Drop for GpuLock {
    fn drop(&mut self) {
        // SAFETY: as take; the deferred flag is read and cleared with
        // interrupts off.
        unsafe {
            let n = core::ptr::read_volatile(&raw const GPU_LOCK) - 1;
            core::ptr::write_volatile(&raw mut GPU_LOCK, n);
            if n == 0 && psx_io::irq::without_interrupts(|| core::mem::take(&mut *(&raw mut CALLBACK_DEFERRED))) {
                if let Some(f) = core::ptr::read_volatile(&raw const DRAW_SYNC_CALLBACK) {
                    f();
                }
            }
        }
    }
}

const DICR_GPU_ENABLE: u32 = (1 << 23) | (1 << 18);
const DICR_GPU_FLAG: u32 = 1 << 26;
/// `I_STAT` and `I_MASK`'s DMA bit.
const IRQ_DMA: u32 = 1 << 3;

/// Name the function to call when drawing ends, and return the previous
/// one. As on the console it is called from the interrupt (the DMA
/// interrupt at the end of a DrawOTag walk, once the GPU has drawn what it
/// read); if the foreground is writing to the GPU at that moment, it is
/// called when that write is done instead.
#[no_mangle]
pub extern "C" fn DrawSyncCallback(func: Option<extern "C" fn()>) -> Option<extern "C" fn()> {
    crate::kernel::ensure_interrupts();
    psx_io::irq::without_interrupts(|| {
        // SAFETY: interrupts are off; the DMA interrupt is the only other
        // user of these, and of DICR's GPU bits.
        unsafe {
            let old = core::ptr::read_volatile(&raw const DRAW_SYNC_CALLBACK);
            core::ptr::write_volatile(&raw mut DRAW_SYNC_CALLBACK, func);
            let d = psx_io::read_u32(psx_hw::dma::DICR);
            psx_io::write_u32(psx_hw::dma::DICR, (d & 0x00FF_FFFF) | DICR_GPU_ENABLE);
            psx_rt::dispatch::attach(psx_rt::dispatch::Source::Dma, crate::kernel::dma_handler);
            psx_io::irq::set_mask(psx_io::irq::mask() | IRQ_DMA);
            old
        }
    })
}

/// The DMA interrupt (kernel.rs's `dma_handler`), CPU interrupts off.
pub fn dma_interrupt() {
    // SAFETY: DICR and I_STAT: acknowledge channel 2's flag (write 1) and
    // the controller's DMA bit (write 0); the statics are shared with the
    // foreground, which changes them with interrupts off.
    unsafe {
        let d = psx_io::read_u32(psx_hw::dma::DICR);
        psx_io::write_u32(psx_hw::dma::DICR, (d & 0x00FF_FFFF) | (d & DICR_GPU_FLAG));
        psx_io::irq::acknowledge(IRQ_DMA);
        if d & DICR_GPU_FLAG == 0 || !core::mem::take(&mut *(&raw mut WALK_IN_FLIGHT)) {
            return;
        }
        // the GPU has read the list; let it finish drawing it
        let mut spins = 0u32;
        while !psx_io::gpu::status().contains(psx_hw::gpu::GpuStat::READY_CMD) && spins < 1_000_000 {
            spins += 1;
        }
        if core::ptr::read_volatile(&raw const GPU_LOCK) != 0 {
            core::ptr::write_volatile(&raw mut CALLBACK_DEFERRED, true);
        } else if let Some(f) = core::ptr::read_volatile(&raw const DRAW_SYNC_CALLBACK) {
            f();
        }
    }
}

/// Wait until the end of the last DrawOTag walk has been handled (its
/// DrawSync callback called), with CPU interrupts on. Bounded: a walk the
/// interrupt never reports is given up on with one message.
pub fn wait_walk_handled() {
    let mut spins = 0u32;
    // SAFETY: a flag the DMA interrupt clears; read volatile.
    while unsafe { core::ptr::read_volatile(&raw const WALK_IN_FLIGHT) } {
        spins += 1;
        if spins > 20_000_000 {
            crate::console::puts("psyq: no DMA interrupt for the last DrawOTag\n");
            // SAFETY: as above.
            unsafe { core::ptr::write_volatile(&raw mut WALK_IN_FLIGHT, false) };
            break;
        }
        core::hint::spin_loop();
    }
}

/// Wait for the last chain to be walked and the GPU to take commands.
pub fn wait_idle() {
    psx_gpu::chain::wait(&mut gpu_dma());
    wait_dma_ready();
    wait_command_ready();
}

fn busy() -> bool {
    psx_io::dma::is_busy(psx_io::dma::Channel::Gpu)
        || !psx_io::gpu::status().contains(psx_hw::gpu::GpuStat::READY_CMD)
}

#[no_mangle]
pub extern "C" fn ResetGraph(mode: i32) -> i32 {
    psx_io::dma::abort(psx_io::dma::Channel::Gpu);
    match mode {
        // 0: the whole GPU; 3: the same but the display settings are kept,
        // which here means only the command side is reset.
        // GP1(00h)'s reset but the display: GP1(00h) would turn it off and blank the loading frame a
        // launcher put up. The game sets its own display mode and ranges and turns it on after this, as
        // it did; the first frame's display write clears the loading frame (psx-io).
        0 => psx_io::gpu::reset_keeping_display(),
        _ => write_display_control(gp1::RESET_CMD_BUFFER),
    }
    psx_io::dma::enable_channel(psx_io::dma::Channel::Gpu);
    crate::kernel::ensure_interrupts();
    0
}

#[no_mangle]
pub extern "C" fn SetGraphDebug(level: i32) -> i32 {
    let _ = level;
    0
}

/// The field on show in 480-line interlaced mode: GPUSTAT bit 31, 1 while
/// the odd lines are on show, 0 while the even ones are and all through the
/// vertical blank (the GPU then leaves the lines of that field alone when
/// drawing to the display area is off, dfe 0). Psy-Q's libgpu GetODE.
#[no_mangle]
pub extern "C" fn GetODE() -> i32 {
    i32::from(psx_io::gpu::status().contains(psx_hw::gpu::GpuStat::INTERLACE_ODD))
}

#[no_mangle]
pub extern "C" fn SetDispMask(mask: i32) {
    write_display_control(gp1::display_enable(mask != 0));
}

#[no_mangle]
pub extern "C" fn DrawSync(mode: i32) -> i32 {
    if mode == 0 {
        wait_idle();
        0
    } else {
        i32::from(busy())
    }
}

// ------------------------------------------------------- ordering tables

#[no_mangle]
pub unsafe extern "C" fn ClearOTagR(ot: *mut u32, n: i32) -> *mut u32 {
    if ot.is_null() || n <= 0 {
        return ot;
    }
    // SAFETY: the game passes a table of n words it owns.
    let table = unsafe { core::slice::from_raw_parts_mut(ot, n as usize) };
    // SAFETY: as gpu_dma: the only driver of channel 6.
    let mut otc = unsafe { OrderingTableClearDma::steal() };
    if !otc.clear_table(table) {
        // The channel wedged (or n is past 65535): link it on the CPU.
        table[0] = 0x00FF_FFFF;
        for i in 1..table.len() {
            table[i] = (ot as u32).wrapping_add(4 * (i as u32 - 1)) & 0x00FF_FFFF;
        }
    }
    ot
}

#[no_mangle]
pub unsafe extern "C" fn ClearOTag(ot: *mut u32, n: i32) -> *mut u32 {
    if ot.is_null() || n <= 0 {
        return ot;
    }
    // SAFETY: the game passes a table of n words it owns.
    let table = unsafe { core::slice::from_raw_parts_mut(ot, n as usize) };
    let last = table.len() - 1;
    for i in 0..last {
        table[i] = (ot as u32).wrapping_add(4 * (i as u32 + 1)) & 0x00FF_FFFF;
    }
    table[last] = 0x00FF_FFFF;
    ot
}

#[no_mangle]
pub unsafe extern "C" fn DrawOTag(p: *const u32) {
    if p.is_null() {
        return;
    }
    let _gpu = GpuLock::take();
    // SAFETY: the game's chain (its ordering table and the primitives on it)
    // lives in its own RAM and is not touched until the next DrawSync,
    // ClearOTagR of the same table or a call here that waits for this walk.
    unsafe {
        psx_gpu::chain::wait(&mut gpu_dma());
        core::ptr::write_volatile(&raw mut WALK_IN_FLIGHT, true);
        psx_gpu::chain::submit_async_raw(&mut gpu_dma(), p)
    }
}

/// RESTORE: PLATFORM, not a Psy-Q call (QD Up's 480-line option screen, GFX.C display_screen,
/// after `VSyncField`). DrawOTag's walk done by the CPU, one primitive at a time with the GPU idle
/// between them, so that no primitive is drawn in a vertical blank: before each one, when the next
/// blank is near, it waits until that blank is over (`kernel::hold_out_of_blank`). A pass then
/// draws every primitive in the picture of some field, whatever its length on the console (the
/// console draws textured pixels about 4.7 times slower than the emulator, hardware tests v2.24).
#[no_mangle]
pub unsafe extern "C" fn DrawOTagField(p: *const u32) {
    if p.is_null() {
        return;
    }
    let _gpu = GpuLock::take();
    let mut at = p;
    loop {
        // SAFETY: the game's ordering table and the primitives it links, as DrawOTag walks them.
        let tag = unsafe { *at };
        let len = tag >> 24;
        if len != 0 {
            wait_idle();
            crate::kernel::hold_out_of_blank();
            for i in 1..=len as usize {
                // SAFETY: as above: `len` command words follow the tag.
                write_command(unsafe { *at.add(i) });
            }
        }
        let next = tag & 0x00FF_FFFF;
        if next == 0x00FF_FFFF {
            break;
        }
        at = (0x8000_0000 | next) as *const u32;
    }
    wait_idle();
}

/// Send one primitive (or environment packet) now.
#[no_mangle]
pub unsafe extern "C" fn DrawPrim(p: *const u32) {
    if p.is_null() {
        return;
    }
    let _gpu = GpuLock::take();
    wait_idle();
    // SAFETY: a primitive: tag word, then `len` command words.
    unsafe {
        let len = *p >> 24;
        for i in 1..=len as usize {
            write_command(*p.add(i));
        }
    }
}

// ------------------------------------------------------------- primitives

#[inline]
unsafe fn set_len_code(p: *mut u8, len: u32, code: u8) {
    // SAFETY: the caller passes a primitive with a tag and a code byte.
    unsafe {
        let tag = p as *mut u32;
        *tag = (*tag & 0x00FF_FFFF) | (len << 24);
        *p.add(7) = code;
    }
}

#[no_mangle]
pub unsafe extern "C" fn SetSprt(p: *mut u8) {
    unsafe { set_len_code(p, 4, 0x64) }
}
#[no_mangle]
pub unsafe extern "C" fn SetSprt8(p: *mut u8) {
    unsafe { set_len_code(p, 3, 0x74) }
}
#[no_mangle]
pub unsafe extern "C" fn SetSprt16(p: *mut u8) {
    unsafe { set_len_code(p, 3, 0x7C) }
}
#[no_mangle]
pub unsafe extern "C" fn SetTile(p: *mut u8) {
    unsafe { set_len_code(p, 3, 0x60) }
}
#[no_mangle]
pub unsafe extern "C" fn SetLineF2(p: *mut u8) {
    unsafe { set_len_code(p, 3, 0x40) }
}
#[no_mangle]
pub unsafe extern "C" fn SetPolyF4(p: *mut u8) {
    unsafe { set_len_code(p, 5, 0x28) }
}
#[no_mangle]
pub unsafe extern "C" fn SetPolyFT4(p: *mut u8) {
    unsafe { set_len_code(p, 9, 0x2C) }
}

/// Bit 1 of the code: semi-transparent.
#[no_mangle]
pub unsafe extern "C" fn SetSemiTrans(p: *mut u8, abe: i32) {
    // SAFETY: a primitive's code byte.
    unsafe {
        let c = p.add(7);
        *c = if abe != 0 { *c | 2 } else { *c & !2 };
    }
}

/// Bit 0 of the code: texture used as it is, not modulated by the colour.
#[no_mangle]
pub unsafe extern "C" fn SetShadeTex(p: *mut u8, tge: i32) {
    // SAFETY: a primitive's code byte.
    unsafe {
        let c = p.add(7);
        *c = if tge != 0 { *c | 1 } else { *c & !1 };
    }
}

fn draw_mode_word(dfe: bool, dtd: bool, tpage: u32) -> u32 {
    gp0::draw_mode_from_texpage((tpage & 0x09FF) | (u32::from(dtd) << 9) | (u32::from(dfe) << 10))
}

/// GP0(E2h): the texture window, in 8-texel steps; a zero or missing
/// rectangle is no window.
fn texture_window_word(tw: *const Rect) -> u32 {
    // SAFETY: null or the game's RECT.
    let Some(r) = (unsafe { tw.as_ref() }) else { return gp0::tex_window(0, 0, 0, 0) };
    if r.w == 0 || r.h == 0 {
        return gp0::tex_window(0, 0, 0, 0);
    }
    let mask_x = ((256 - (r.w as u32 & 0xFF)) >> 3) & 0x1F;
    let mask_y = ((256 - (r.h as u32 & 0xFF)) >> 3) & 0x1F;
    let off_x = (r.x as u32 >> 3) & 0x1F;
    let off_y = (r.y as u32 >> 3) & 0x1F;
    gp0::tex_window(mask_x, mask_y, off_x, off_y)
}

#[no_mangle]
pub unsafe extern "C" fn SetDrawMode(p: *mut u32, dfe: i32, dtd: i32, tpage: i32, tw: *const Rect) {
    // SAFETY: a DR_MODE: tag and two command words.
    unsafe {
        *p = (*p & 0x00FF_FFFF) | (2 << 24);
        *p.add(1) = draw_mode_word(dfe != 0, dtd != 0, tpage as u32);
        *p.add(2) = texture_window_word(tw);
    }
}

#[no_mangle]
pub unsafe extern "C" fn AddPrim(ot: *mut u32, p: *mut u32) {
    // SAFETY: an ordering-table entry and a primitive's tag.
    unsafe {
        *p = (*p & 0xFF00_0000) | (*ot & 0x00FF_FFFF);
        *ot = (*ot & 0xFF00_0000) | (p as u32 & 0x00FF_FFFF);
    }
}

#[no_mangle]
pub unsafe extern "C" fn AddPrims(ot: *mut u32, p0: *mut u32, p1: *mut u32) {
    // SAFETY: an ordering-table entry and two primitives' tags (p0 .. p1 linked).
    unsafe {
        *p1 = (*p1 & 0xFF00_0000) | (*ot & 0x00FF_FFFF);
        *ot = (*ot & 0xFF00_0000) | (p0 as u32 & 0x00FF_FFFF);
    }
}

#[no_mangle]
pub unsafe extern "C" fn CatPrim(p0: *mut u32, p1: *mut u32) {
    // SAFETY: a primitive's tag.
    unsafe { *p0 = (*p0 & 0xFF00_0000) | (p1 as u32 & 0x00FF_FFFF) }
}

#[no_mangle]
pub unsafe extern "C" fn TermPrim(p: *mut u32) {
    // SAFETY: a primitive's tag.
    unsafe { *p |= 0x00FF_FFFF }
}

// ----------------------------------------------------------- environments

#[no_mangle]
pub unsafe extern "C" fn SetDefDrawEnv(env: *mut DrawEnv, x: i32, y: i32, w: i32, h: i32) -> *mut DrawEnv {
    // SAFETY: the game's DRAWENV.
    let e = unsafe { &mut *env };
    e.clip = Rect { x: x as i16, y: y as i16, w: w as i16, h: h as i16 };
    e.ofs = [x as i16, y as i16];
    e.tw = Rect { x: 0, y: 0, w: 0, h: 0 };
    e.tpage = 10; // page at (640, 0), 4-bit, no blending
    e.dtd = 1;
    // Not drawn on the field on show, also for a 480-line buffer: the 1996
    // build of QD Up leaves dfe 0 in its 320x480 option screen's DRAWENVs
    // (read from its RAM). The fill and the backdrop (DR_MODE dfe 0) then
    // skip the same lines and the two fields are drawn on alternate frames.
    let _ = h;
    e.dfe = 0;
    e.isbg = 0;
    e.r0 = 0;
    e.g0 = 0;
    e.b0 = 0;
    env
}

#[no_mangle]
pub unsafe extern "C" fn SetDefDispEnv(env: *mut DispEnv, x: i32, y: i32, w: i32, h: i32) -> *mut DispEnv {
    // SAFETY: the game's DISPENV.
    let e = unsafe { &mut *env };
    e.disp = Rect { x: x as i16, y: y as i16, w: w as i16, h: h as i16 };
    e.screen = Rect { x: 0, y: 0, w: 0, h: 0 };
    e.isinter = 0;
    e.isrgb24 = 0;
    env
}

#[no_mangle]
pub unsafe extern "C" fn PutDrawEnv(env: *mut DrawEnv) -> *mut DrawEnv {
    let _gpu = GpuLock::take();
    // SAFETY: the game's DRAWENV.
    let e = unsafe { &*env };
    let c = e.clip;
    let x1 = (c.x as i32 + c.w as i32 - 1).max(0) as u32;
    let y1 = (c.y as i32 + c.h as i32 - 1).max(0) as u32;
    wait_idle();
    write_command(draw_mode_word(e.dfe != 0, e.dtd != 0, e.tpage as u32));
    write_command(texture_window_word(&e.tw));
    write_command(gp0::draw_area_top_left(c.x as u32, c.y as u32));
    write_command(gp0::draw_area_bottom_right(x1, y1));
    write_command(gp0::draw_offset(e.ofs[0] as i32, e.ofs[1] as i32));
    if e.isbg != 0 && c.w > 0 && c.h > 0 {
        write_command(gp0::fill_rect(e.r0, e.g0, e.b0));
        write_command((c.x as u32 & 0xFFFF) | ((c.y as u32 & 0xFFFF) << 16));
        write_command((c.w as u32 & 0xFFFF) | ((c.h as u32 & 0xFFFF) << 16));
    }
    env
}

/// The display: start in VRAM, then the TV area and the mode, as NMS's 1996
/// builds' PutDispEnv sets them (read from 360X's MAIN.CPE). Horizontally
/// from GPU clock 608 + 10 per screen.x unit, 2560 clocks or 10 per
/// screen.w unit wide; vertically from line 16 (NTSC) or 19 (PAL) plus
/// screen.y, 240 lines or screen.h; both clamped to what the TV shows.
/// The video mode is written back into the environment's pad0, as there.
#[no_mangle]
pub unsafe extern "C" fn PutDispEnv(env: *mut DispEnv) -> *mut DispEnv {
    // SAFETY: the game's DISPENV.
    let e = unsafe { &mut *env };
    let d = e.disp;
    write_display_control(gp1::display_start(d.x as u32, d.y as u32));
    let pal = video_mode() == 1;
    e.pad0 = u8::from(pal);
    let s = e.screen;
    let x0 = (608 + 10 * s.x as i32).clamp(500, 3290);
    let x1 = if s.w != 0 { 608 + 10 * s.x as i32 + 10 * s.w as i32 } else { 608 + 10 * s.x as i32 + 2560 };
    let x1 = if x1 < x0 + 80 || x1 > 3290 { 3290 } else { x1 };
    let y0 = s.y as i32 + if pal { 19 } else { 16 };
    let y1 = y0 + if s.h != 0 { s.h as i32 } else { 240 };
    let y0 = if y0 < 0 { 0 } else { y0.min(if pal { 310 } else { 254 }) };
    let y1 = if y1 < y0 + 1 { y0 + 1 } else { y1.min(if pal { 312 } else { 256 }) };
    write_display_control(gp1::h_display_range(x0 as u32, x1 as u32));
    write_display_control(gp1::v_display_range(y0 as u32, y1 as u32));
    // The mode word's fixed part and the three field values this function composes at run time, each the
    // SDK's constructor folded at compile time: the 368-wide bit, and the two bits of a tall picture (the
    // 480-line mode, always interlaced).
    const PLAIN: u32 = gp1::display_mode_in_range(0, 0, false, false, false);
    const WIDE_368: u32 = gp1::display_mode_hres2_in_range(0, 0, false, false, false) ^ PLAIN;
    const TALL: u32 = gp1::display_mode_in_range(0, 1, false, false, true) ^ PLAIN;
    let hres = match d.w {
        w if w <= 280 => 0,
        w if w <= 352 => 1,
        w if w <= 400 => WIDE_368,
        w if w <= 560 => 2,
        _ => 3,
    };
    let tall = d.h > if pal { 288 } else { 256 };
    let mode = hres
        | (u32::from(pal) << 3)
        | (u32::from(e.isrgb24 != 0) << 4)
        | (u32::from(e.isinter != 0) << 5)
        | if tall { TALL } else { 0 };
    write_display_control(PLAIN | mode);
    env
}

// ---------------------------------------------------------------- VRAM

/// Copy `rect` to VRAM from `p` (16-bit pixels, rows packed), through the
/// command port. An odd pixel count is sent as whole words; the GPU drops
/// the spare half-word.
#[no_mangle]
pub unsafe extern "C" fn LoadImage(rect: *const Rect, p: *const u8) -> i32 {
    let _gpu = GpuLock::take();
    // SAFETY: the game's RECT.
    let r = unsafe { *rect };
    if r.w <= 0 || r.h <= 0 {
        return 0;
    }
    let words = (r.w as usize * r.h as usize + 1) / 2;
    wait_idle();
    write_command(gp0::CLEAR_CACHE); // flush the texture cache
    write_command(gp0::COPY_CPU_TO_VRAM);
    write_command((r.x as u32 & 0xFFFF) | ((r.y as u32 & 0xFFFF) << 16));
    write_command((r.w as u32 & 0xFFFF) | ((r.h as u32 & 0xFFFF) << 16));
    let src = p as *const u32;
    for i in 0..words {
        // SAFETY: the game's pixel data, w*h half-words (the last word may
        // reach one half-word past an odd count, as the hardware reads it).
        write_command_unguarded(unsafe { core::ptr::read_unaligned(src.add(i)) });
    }
    0
}

#[no_mangle]
pub unsafe extern "C" fn StoreImage(rect: *const Rect, p: *mut u8) -> i32 {
    let _gpu = GpuLock::take();
    // SAFETY: the game's RECT.
    let r = unsafe { *rect };
    if r.w <= 0 || r.h <= 0 {
        return 0;
    }
    let words = (r.w as usize * r.h as usize + 1) / 2;
    wait_idle();
    write_command(gp0::CLEAR_CACHE);
    write_command(gp0::COPY_VRAM_TO_CPU);
    write_command((r.x as u32 & 0xFFFF) | ((r.y as u32 & 0xFFFF) << 16));
    write_command((r.w as u32 & 0xFFFF) | ((r.h as u32 & 0xFFFF) << 16));
    while !psx_io::gpu::status().contains(psx_hw::gpu::GpuStat::READY_VRAM_SEND) {}
    let dst = p as *mut u32;
    for i in 0..words {
        // SAFETY: the game's buffer for w*h half-words.
        unsafe { core::ptr::write_unaligned(dst.add(i), psx_io::gpu::read_data()) };
    }
    0
}

#[no_mangle]
pub unsafe extern "C" fn MoveImage(rect: *const Rect, x: i32, y: i32) -> i32 {
    let _gpu = GpuLock::take();
    // SAFETY: the game's RECT.
    let r = unsafe { *rect };
    if r.w <= 0 || r.h <= 0 {
        return 0;
    }
    wait_idle();
    write_command(gp0::COPY_VRAM_TO_VRAM);
    write_command((r.x as u32 & 0xFFFF) | ((r.y as u32 & 0xFFFF) << 16));
    write_command((x as u32 & 0xFFFF) | ((y as u32 & 0xFFFF) << 16));
    write_command((r.w as u32 & 0xFFFF) | ((r.h as u32 & 0xFFFF) << 16));
    0
}

#[no_mangle]
pub unsafe extern "C" fn ClearImage(rect: *const Rect, r: u8, g: u8, b: u8) -> i32 {
    let _gpu = GpuLock::take();
    // SAFETY: the game's RECT.
    let rc = unsafe { *rect };
    wait_idle();
    write_command(gp0::fill_rect(r, g, b));
    write_command((rc.x as u32 & 0xFFFF) | ((rc.y as u32 & 0xFFFF) << 16));
    write_command((rc.w as u32 & 0xFFFF) | ((rc.h as u32 & 0xFFFF) << 16));
    0
}
