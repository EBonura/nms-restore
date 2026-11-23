//! Kernel services: events, critical sections, the vertical blank, the
//! controller buffers.
//!
//! On the console these were the BIOS's: its interrupt handler filled the
//! controller buffers and ran the handlers of enabled events at every
//! vertical blank. Here psx-rt's dispatcher (`sdk/docs/INTERRUPTS.md`) owns the
//! exception vector and counts vertical blanks, and the layer is one of its
//! VBlank hooks, which does the BIOS's part on every VBlank, in interrupt
//! context on the dispatcher's stack:
//!
//! 1. poll the controller in port 1 into the buffer `InitPAD` was given;
//! 2. call the handler of every enabled `RCntCNT3` event opened with
//!    `EvMdINTR`, then the `VSyncCallback` function.
//!
//! The DMA interrupt at the end of an ordering-table walk, for
//! `DrawSyncCallback` (gpu.rs), is the dispatcher's DMA source, attached there.
//!
//! So a game's VBlank handler runs in interrupt context once per VBlank, as
//! on the console (QD Up's counts the `frames` its physics steps by).

use crate::console;

// ------------------------------------------------------------------ events

const EVENT_CLASS_VBLANK: u32 = 0xF200_0003; // RCntCNT3
const EV_MD_INTR: u32 = 0x1000;
const MAX_EVENTS: usize = 16;
const EVENT_HANDLE: u32 = 0xF100_0000;

#[derive(Clone, Copy)]
#[allow(dead_code, reason = "spec is kept as opened; only VBlank events are delivered")]
struct Event {
    class: u32,
    spec: u32,
    mode: u32,
    func: Option<extern "C" fn()>,
    open: bool,
    enabled: bool,
    /// delivered and not yet tested (EvMdNOINTR events)
    ready: bool,
}

const CLOSED: Event = Event { class: 0, spec: 0, mode: 0, func: None, open: false, enabled: false, ready: false };
static mut EVENTS: [Event; MAX_EVENTS] = [CLOSED; MAX_EVENTS];
static mut VSYNC_CALLBACK: Option<extern "C" fn()> = None;

fn slot(event: i32) -> Option<usize> {
    let e = event as u32;
    if e & 0xFFFF_0000 != EVENT_HANDLE {
        return None;
    }
    let i = (e & 0xFFFF) as usize;
    (i < MAX_EVENTS).then_some(i)
}

/// With interrupts off, so the VBlank handler never sees a half-written entry.
fn with_events<R>(f: impl FnOnce(&mut [Event; MAX_EVENTS]) -> R) -> R {
    // SAFETY: CPU interrupts are off for the closure, so the VBlank hook (the
    // only other reader) cannot run; one thread of control.
    psx_io::irq::without_interrupts(|| f(unsafe { &mut *(&raw mut EVENTS) }))
}

#[no_mangle]
pub extern "C" fn OpenEvent(class: u32, spec: i32, mode: i32, func: Option<extern "C" fn()>) -> i32 {
    with_events(|events| {
        for (i, e) in events.iter_mut().enumerate() {
            if !e.open {
                *e = Event { class, spec: spec as u32, mode: mode as u32, func, open: true, enabled: false, ready: false };
                return (EVENT_HANDLE | i as u32) as i32;
            }
        }
        console::puts("psyq: OpenEvent: no free event\n");
        -1
    })
}

#[no_mangle]
pub extern "C" fn CloseEvent(event: i32) -> i32 {
    match slot(event) {
        Some(i) => with_events(|events| {
            events[i] = CLOSED;
            1
        }),
        None => 0,
    }
}

#[no_mangle]
pub extern "C" fn EnableEvent(event: i32) -> i32 {
    match slot(event) {
        Some(i) => with_events(|events| {
            events[i].enabled = events[i].open;
            1
        }),
        None => 0,
    }
}

#[no_mangle]
pub extern "C" fn DisableEvent(event: i32) -> i32 {
    match slot(event) {
        Some(i) => with_events(|events| {
            events[i].enabled = false;
            1
        }),
        None => 0,
    }
}

/// 1 once an event has been delivered (and clears it), else 0. Besides the
/// VBlank, which calls its handlers, only the memory card calls deliver
/// events (card.rs).
#[no_mangle]
pub extern "C" fn TestEvent(event: i32) -> i32 {
    match slot(event) {
        Some(i) => with_events(|events| i32::from(core::mem::take(&mut events[i].ready))),
        None => 0,
    }
}

/// Mark every open, enabled event of this class and spec as happened, as
/// the BIOS's DeliverEvent did for an event without a handler.
pub fn deliver_event(class: u32, spec: u32) {
    with_events(|events| {
        for e in events.iter_mut() {
            if e.open && e.enabled && e.class == class && e.spec == spec {
                e.ready = true;
            }
        }
    });
}

#[no_mangle]
pub extern "C" fn WaitEvent(_event: i32) -> i32 {
    stub!("WaitEvent");
    0
}

#[no_mangle]
pub extern "C" fn VSyncCallback(func: Option<extern "C" fn()>) -> i32 {
    psx_io::irq::without_interrupts(|| {
        // SAFETY: interrupts are off; the VBlank hook is the only other reader.
        unsafe { core::ptr::write_volatile(&raw mut VSYNC_CALLBACK, func) };
    });
    0
}

#[no_mangle]
pub extern "C" fn EnterCriticalSection() {
    // SAFETY: masking CPU interrupts is always sound; ExitCriticalSection
    // turns them back on, as the BIOS calls did.
    unsafe {
        psx_io::irq::disable_cpu_interrupts();
    }
}

#[no_mangle]
pub extern "C" fn ExitCriticalSection() {
    // SAFETY: the vector holds psx-rt's handler (or the BIOS's, if the game
    // has not called ResetCallback yet, with no source unmasked).
    unsafe { psx_io::irq::restore_cpu_interrupts(true) }
}

/// The root counters QD Up starts are only there to drive its VBlank event,
/// which the VBlank interrupt delivers by itself.
#[no_mangle]
pub extern "C" fn StartRCnt(_spec: u32) -> i32 {
    1
}

#[no_mangle]
pub extern "C" fn StopRCnt(_spec: u32) -> i32 {
    1
}

// ------------------------------------------------------- controller buffers

static mut PAD_BUF: [*mut u8; 2] = [core::ptr::null_mut(); 2];
static mut PAD_LEN: [usize; 2] = [0; 2];
static mut PAD_ON: bool = false;

#[no_mangle]
pub unsafe extern "C" fn InitPAD(buf1: *mut u8, len1: i32, buf2: *mut u8, len2: i32) {
    psx_io::irq::without_interrupts(|| {
        // SAFETY: interrupts are off; the VBlank hook is the only other user, and
        // the game keeps the buffers alive for the program's lifetime.
        unsafe {
            PAD_BUF = [buf1, buf2];
            PAD_LEN = [len1.max(0) as usize, len2.max(0) as usize];
            for (b, n) in [(buf1, len1), (buf2, len2)] {
                if !b.is_null() && n > 0 {
                    *b = 0xFF; // nothing read yet
                }
            }
        }
    });
}

#[no_mangle]
pub extern "C" fn StartPAD() -> i32 {
    ensure_interrupts();
    // SAFETY: a flag the VBlank hook reads; an aligned byte store cannot tear.
    unsafe { core::ptr::write_volatile(&raw mut PAD_ON, true) };
    1
}

#[no_mangle]
pub extern "C" fn StopPAD() {
    // SAFETY: as StartPAD.
    unsafe { core::ptr::write_volatile(&raw mut PAD_ON, false) };
}

/// Stop the VBlank controller poll while a memory card transaction has the
/// port (card.rs); says whether it was on, for `resume_pad`.
pub(crate) fn pause_pad() -> bool {
    // SAFETY: as StartPAD; the hook only reads the flag.
    unsafe {
        let was = core::ptr::read_volatile(&raw const PAD_ON);
        core::ptr::write_volatile(&raw mut PAD_ON, false);
        was
    }
}

pub(crate) fn resume_pad(was: bool) {
    // SAFETY: as StartPAD.
    unsafe { core::ptr::write_volatile(&raw mut PAD_ON, was) };
}

/// The BIOS let a game ask it not to acknowledge the VBlank interrupt;
/// psx-rt's handler always does, after the game's handlers have run.
#[no_mangle]
pub extern "C" fn ChangeClearPAD(_val: i32) {}

/// How far the left stick has to be pushed, in counts from centre (of the 127 it reaches), before
/// the game reads a direction, per axis, through the SDK's one stick rule
/// (`psx_pad::stick_bits_of`: sticks count when the pad reports them, `PadMode::has_sticks`). The
/// radius is the one 360X and QD Up have had since the analog pass; the games are tuned against it.
const STICK_THRESHOLD: i16 = 28;

/// Fill one controller buffer from a poll: status, id, then the data bytes
/// as the controller sent them (buttons active low).
///
/// RESTORE: PLATFORM (owner's decision, 2026-11-13). Both games were written in 1996 for the
/// digital pad: they read the buttons only when the id byte says a digital pad (`GetType` is
/// `STD_PAD`, `GENERAL.C` `Read_Pads`, `PAD.C` `get_pad`) and ignore the packet otherwise, so a
/// DualShock in analog mode (id 0x73) was dead to them, and a mode switch in play left the last
/// direction held. A pad in analog mode is now handed over as the digital pad the games know:
/// id 0x41, the two button bytes, no stick bytes; L3 and R3 (which a digital pad has no switch
/// for) read released; the left stick pushed past `STICK_DEADZONE` presses the direction it points
/// to; the right stick is not read. The direction pad itself still works in analog mode.
unsafe fn fill_pad(buf: *mut u8, len: usize, state: psx_pad::PadState) {
    if buf.is_null() || len < 2 {
        return;
    }
    // SAFETY: the caller passes the buffer and length the game gave InitPAD.
    unsafe {
        if !state.is_connected() {
            // RESTORE: PLATFORM. No controller answered: status 0xFF as a real port gives, and
            // the id and both button bytes released (0xFF, active low) instead of the last
            // poll's, so a reader that ignores the status byte sees no button pressed.
            for i in 1..4 {
                if i < len {
                    *buf.add(i) = 0xFF;
                }
            }
            *buf = 0xFF;
            return;
        }
        let mut buttons = !state.buttons.bits();
        let mut id = state.id_low;
        if state.mode.has_sticks() {
            id = 0x41;
            buttons |= psx_pad::button::L3 | psx_pad::button::R3;
            buttons &= !psx_pad::stick_bits_of::<STICK_THRESHOLD>(&state);
        }
        let data = [id, buttons as u8, (buttons >> 8) as u8];
        for (i, &b) in data.iter().enumerate() {
            if 1 + i < len {
                *buf.add(1 + i) = b;
            }
        }
        *buf = 0x00;
    }
}

/// A buffer of the layer's own for a pad poll before the game has called `InitPAD` (the boot splash of a
/// game that shows it before its system set-up, splash.rs).
static mut EARLY_PAD: [u8; 4] = [0xFF; 4];

/// Poll port 1 into the layer's own buffer when the game has registered none; says whether it did, for
/// `early_pad_end`. A game's later `InitPAD` and `StartPAD` take over as usual.
pub(crate) fn early_pad_begin() -> bool {
    ensure_interrupts();
    psx_io::irq::without_interrupts(|| {
        // SAFETY: interrupts are off; the VBlank poll is the only other user.
        unsafe {
            if !(*(&raw const PAD_BUF))[0].is_null() {
                return false;
            }
            PAD_BUF[0] = (&raw mut EARLY_PAD).cast::<u8>();
            PAD_LEN[0] = 4;
            core::ptr::write_volatile(&raw mut PAD_ON, true);
            true
        }
    })
}

/// Undo `early_pad_begin`.
pub(crate) fn early_pad_end() {
    psx_io::irq::without_interrupts(|| {
        // SAFETY: as early_pad_begin.
        unsafe {
            core::ptr::write_volatile(&raw mut PAD_ON, false);
            PAD_BUF[0] = core::ptr::null_mut();
            PAD_LEN[0] = 0;
        }
    });
}

/// The buttons of the controller in port 1 as the VBlank poll last filled the buffer the game gave
/// `InitPAD`, for the boot splash's skip (splash.rs): nothing held while no pad is started or none
/// answers. The buffer holds the buttons active low, as `fill_pad` writes them.
pub(crate) fn pad1_buttons() -> psx_pad::ButtonState {
    // SAFETY: the buffer is the game's (alive for the program's lifetime, as InitPAD's contract
    // says) and the VBlank poll rewrites it, so the bytes are read volatile.
    unsafe {
        let buf = PAD_BUF[0];
        if buf.is_null() || PAD_LEN[0] < 4 || core::ptr::read_volatile(buf) != 0 {
            return psx_pad::ButtonState::NONE;
        }
        let lo = u16::from(core::ptr::read_volatile(buf.add(2)));
        let hi = u16::from(core::ptr::read_volatile(buf.add(3)));
        psx_pad::ButtonState::from_bits(!(lo | hi << 8))
    }
}

// ---------------------------------------------------- the VBlank interrupt

/// The controller driver the VBlank hook polls port 1 with (`psx_pad::Controllers`, as every game
/// reads its pads); the hook is its only user.
///
/// RESTORE: PLATFORM. `digital()`: the poll reads the pad in whatever mode it is in and has no
/// analog handshake, which would hold every interrupt off for several video frames inside the hook
/// at boot and at each plug-in. The layer never forced analog mode (the player's Analog button
/// decides, as on the 1996 `InitPAD` path); a pad in analog mode reports its sticks through the
/// same poll, and `fill_pad` hands them to the games as the direction pad.
///
/// Pad row, closed (wave 4): this IS the SDK's driver, so the pad-transport fix of the pin wave is what
/// these games run (`Controllers<false>::step` and the transport's `cs_high_gap` are linked in both
/// executables; with the pad model at 600 cycles the Mission 1 frame equals the no-model run). What stays
/// in the layer is only the Psy-Q buffer API on top: `InitPAD`'s buffers, `fill_pad` (the 1996 digital
/// packet the games read) and the early-poll buffer of the boot splash.
static mut PADS: psx_pad::Controllers<false> = psx_pad::Controllers::digital();

/// Run by psx-rt's dispatcher once per VBlank, after its own VBlank work, in interrupt context on
/// the dispatcher's stack (`sdk/docs/INTERRUPTS.md`): the BIOS's part of the VBlank.
fn vblank_hook() {
    // SAFETY: CPU interrupts are off in a handler; every foreground writer of these statics also
    // turns them off, and the hook is the only user of `PADS` and of the port token.
    unsafe {
        if core::ptr::read_volatile(&raw const PAD_ON) {
            // Port 1 only: QD Up never reads port 2, and an empty port costs about 120k cycles to
            // find empty on every poll.
            let state = *(*(&raw mut PADS)).poll(&mut psx_io::periph::ControllerPort::steal(), psx_pad::Port::One);
            fill_pad(PAD_BUF[0], PAD_LEN[0], state);
            if !PAD_BUF[1].is_null() && PAD_LEN[1] > 0 {
                *PAD_BUF[1] = 0xFF;
            }
        }
        let events = &*(&raw const EVENTS);
        for e in events.iter() {
            if e.open && e.enabled && e.class == EVENT_CLASS_VBLANK && e.mode & EV_MD_INTR != 0 {
                if let Some(f) = e.func {
                    f();
                }
            }
        }
        if let Some(f) = core::ptr::read_volatile(&raw const VSYNC_CALLBACK) {
            f();
        }
    }
}

/// The DMA interrupt's handler, attached by `DrawSyncCallback` (gpu.rs).
pub(crate) fn dma_handler() {
    crate::gpu::dma_interrupt();
}

static mut INSTALLED: bool = false;

/// psx-rt's VBlank counter, and the BIOS's VBlank work as a hook of its dispatcher.
fn install() {
    psx_rt::interrupts::install_vblank_counter();
    if psx_rt::dispatch::on_vblank(vblank_hook).is_err() {
        console::puts("psyq: no VBlank hook slot; VBlank handlers off\n");
        return;
    }
    // SAFETY: a flag only the foreground writes.
    unsafe { core::ptr::write_volatile(&raw mut INSTALLED, true) };
}

pub fn ensure_interrupts() {
    // SAFETY: a flag only the foreground writes.
    if !unsafe { core::ptr::read_volatile(&raw const INSTALLED) } {
        install();
    }
}

/// Psy-Q's callback system initialisation: here, the interrupt set-up.
#[no_mangle]
pub extern "C" fn ResetCallback() -> i32 {
    ensure_interrupts();
    0
}

// ------------------------------------------------------------------ VSync

static mut LAST_VSYNC: u32 = 0;

fn vblanks() -> u32 {
    psx_rt::interrupts::vblank_count()
}

/// Horizontal lines per vertical blank, for VSync's return value (Psy-Q
/// reports time in horizontal blanks).
fn lines_per_field() -> u32 {
    if crate::gpu::video_mode() == 1 { 313 } else { 263 }
}

#[no_mangle]
pub extern "C" fn VSync(mode: i32) -> i32 {
    ensure_interrupts();
    let now = vblanks();
    if mode < 0 {
        return now as i32;
    }
    // SAFETY: foreground-only static.
    let last = unsafe { core::ptr::read_volatile(&raw const LAST_VSYNC) };
    if mode == 1 {
        return (now.wrapping_sub(last) * lines_per_field()) as i32;
    }
    let mut target = now.wrapping_add(1);
    if mode > 1 {
        let wanted = last.wrapping_add(mode as u32);
        if (wanted.wrapping_sub(target) as i32) > 0 {
            target = wanted;
        }
    }
    while (vblanks().wrapping_sub(target) as i32) < 0 {
        core::hint::spin_loop();
    }
    let t = vblanks();
    // SAFETY: as above.
    unsafe { core::ptr::write_volatile(&raw mut LAST_VSYNC, t) };
    (t.wrapping_sub(last) * lines_per_field()) as i32
}

/// Lines from the VBlank interrupt to the end of the vertical blank, with 12 to spare: the blank
/// is 58 lines in PAL (line 256 to the end of the 314-line field) and 20 in NTSC (243 to the end
/// of 263), as the emulator models the console (emulator-core timers.rs vblank_lines).
fn start_line() -> u16 {
    if crate::gpu::video_mode() == 1 { 58 + 12 } else { 20 + 12 }
}

/// The last line from the VBlank interrupt at which a primitive may still be sent: 20 lines (1.3
/// ms) before the next blank, more than the largest primitive of QD Up's option screen takes on
/// the console (a quarter of the backdrop, 19,200 pixels a field: about 0.9 ms at the 7.2 ms the
/// console took for 153,600 textured pixels, hardware tests v2.24).
fn stop_line() -> u16 {
    if crate::gpu::video_mode() == 1 { 314 - 20 } else { 263 - 20 }
}

/// Lines since the last VBlank interrupt: root counter 1 counting horizontal blanks, reset at
/// every vertical blank (set up by `VSyncField`).
fn line() -> u16 {
    psx_io::timers::counter(psx_io::timers::Timer::Timer1)
}

/// RESTORE: PLATFORM, not a Psy-Q call (QD Up's 480-line option screen, GFX.C display_screen).
/// Waits for the next vertical blank whose count has the parity `parity` (0 or 1), then for the
/// blank to be over, and returns the count. In 480-line interlaced mode the fields take turns, one
/// a vertical blank, so passes started after blanks of opposite count parity start in opposite
/// fields, an odd number of fields apart however long a pass takes; and in the picture (not in the
/// blank) the GPU, drawing with dfe 0, leaves the lines of the field on show alone and draws the
/// other field's (hardware tests v2.24, rows B: it follows the field on show every time; in the
/// blank it skips the even lines every time). GPUSTAT is never read.
#[no_mangle]
pub extern "C" fn VSyncField(parity: i32) -> i32 {
    use psx_hw::timers::mode::{clock_source, sync_mode, SYNC_ENABLE};
    use psx_io::timers::{set_mode, Timer};

    VSync(0);
    if (vblanks() ^ parity as u32) & 1 != 0 {
        VSync(0);
    }
    // Root counter 1 counts horizontal blanks from 0 (the write) and from 0 again at every vertical blank.
    set_mode(Timer::Timer1, SYNC_ENABLE | sync_mode(1) | clock_source(1));
    let start = start_line();
    while line() < start {
        core::hint::spin_loop();
    }
    vblanks() as i32
}

/// Called before each primitive of `DrawOTagField` (gpu.rs), with the GPU idle: when the next
/// vertical blank is near, wait for it and for it to be over, so no primitive is drawn in a blank
/// (where the GPU would skip the even lines whatever the field). The primitives after the wait
/// draw the next field's lines; the pass after this one, started an odd number of fields later,
/// waits at the same primitive and so draws each primitive in the other field.
pub fn hold_out_of_blank() {
    if line() < stop_line() {
        return;
    }
    VSync(0);
    let start = start_line();
    while line() < start {
        core::hint::spin_loop();
    }
}
