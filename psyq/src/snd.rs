//! The sound library (libsnd), as 360X uses it, on psx-spu.
//!
//! VAB banks follow the public VAB format: a 32-byte header ("pBAV",
//! programs, tones, waveforms, master volume and pan), 128 program entries
//! of 16 bytes, then 16 tone entries of 32 bytes for each program that has
//! tones (in program order; each tone names its program), then 256
//! half-words of waveform sizes (in 8-byte units, entry 0 unused).
//! `SsVabOpenHead` reads what playing needs from the header into the
//! layer's own tables, and `SsSeqOpen` copies the song when it fits: 360X
//! loads both into its heap and resets that heap at every screen
//! (`Jalloc_Init`) while the bank stays open and the song plays on, so a
//! library that went on reading the game's memory (as the 1996 one did,
//! by its RAM) would read whatever the next screen loaded there.
//! `SsVabTransBody` copies the waveforms to sound RAM one after another
//! from 0x1010 up; `SsVabTransBodyPartly` takes the same body in pieces.
//!
//! `SsUtKeyOn` sets up one tone of one program on a free voice: start
//! address, the tone's ADSR words as they are, the pitch the 1996
//! library's note-to-pitch rule gives (below), and a volume (below). As in
//! the 1996 library these go into a copy of the voice registers and reach
//! the sound processor at the next tick: in SS_NOTICK mode when the game
//! calls `SsSeqCalledTbyT`. NMS's 1996 build of 360X shows this: at the
//! ship demo its libsnd has voice 0 set up (left 3FFFh, right 0, pitch
//! 7D4h, start 1010h, ADSR 80FFh/5FC0h) and the key-on pending, and as it
//! never calls SsSeqCalledTbyT (UpdateTheSound is commented out in
//! VblCallback) nothing is played. In the tick modes the layer writes at
//! once instead of at the next tick.
//!
//! Pitch (read from the 1996 build's note2pitch2): fine tune plus the
//! tone's shift, in 1/128 semitone, is cut to 1/16 semitone steps (a
//! carry into the next semitone at 16); the note against the tone's centre
//! note, plus 60, gives an octave and a semitone; a table of
//! floor(4096 * 2^((semitone + step/16) / 12)) gives the pitch at octave
//! 5, shifted left or right for the others.
//!
//! SEQ songs ("pQES", version 1: resolution in ticks per quarter note,
//! tempo in microseconds per quarter note, then MIDI events with running
//! status, ending with FF 2F) play at `SsSeqCalledTbyT`. Measured on NMS's
//! March 1996 build of 360X (`NMS.zip NMS/360WORK/MAIN.CPE`, the one build
//! whose music plays: OPTPA.SEQ on OPTIONS.VAB, ticked from its VBlank
//! callback), in the emulator:
//! - each call advances the song by resolution * 1000000 / (tempo * rate)
//!   ticks, with rate 50 on a PAL display and 60 otherwise (its PAL VBlank
//!   moved OPTPA 9.6 ticks per call; the layer keeps the fraction);
//! - the events due are played first, then time advances: the song's
//!   tick-0 events play at the first call after `SsSeqPlay`, and when the
//!   end comes due a song played with SSPLAY_INFINITY starts again from
//!   its first event within the same call (OPTPA's 9984 ticks took exactly
//!   1040 calls a play; for a song whose length is not a whole number of
//!   calls the layer carries the time past the end into the next play);
//! - a note-on keys every tone of the channel's program whose key range
//!   holds the note, each on the lowest-numbered voice not keyed on (a
//!   voice keyed off is free again at once, even in its release); a
//!   note-on of velocity 0 or a note-off keys the note's voices off;
//! - the voice volume is x * x * 3FFFh / 127 / 127 with
//!   x = velocity * channel volume (controller 7) / 127, at every velocity
//!   and channel volume OPTPA plays (16 voices, 600 notes watched in its
//!   voice registers); the same on both sides at the centre pan.
//! The other volumes (the song's `SsSeqSetVol`, the program's, the tone's
//! and the bank's) are 127 wherever 360X's data and calls reach them, so
//! the 1996 build never showed what they do below 127: here they multiply
//! into x like the channel volume. `SsUtKeyOn`'s volume is x the same way.
//! Pan: the tone's, program's, channel's (controller 10) and bank's pans
//! (64 the centre) are added as offsets; at the centre both sides are at
//! full volume, at 0 only the left (the ship demo's bank, program pan 0,
//! as the 1996 build's registers show).
//! Controllers other than 7 and 10, pitch bend and loop markers are not
//! played (360X's songs have none).

use psx_spu::{Adsr, Pitch, SpuAddr, Voice, Volume};

const MAX_VABS: usize = 4;
const MAX_TONES: usize = 128;
const MAX_SONGS: usize = 4;
const SONG_ARENA: usize = 4096;
const VOICES: usize = 24;
const BODY_START: u32 = 0x1010;
const SPU_RAM: u32 = 0x8_0000;
const SS_NOTICK: i32 = 0x1000;

/// One tone, as SsUtKeyOn and a note-on need it.
#[derive(Clone, Copy)]
struct Tone {
    vol: u8,
    pan: u8,
    centre: u8,
    shift: u8,
    min: u8,
    max: u8,
    adsr: [u16; 2],
    /// byte offset of the tone's waveform from the bank's body start
    wave: u32,
}

const NO_TONE: Tone = Tone { vol: 0, pan: 0, centre: 0, shift: 0, min: 0, max: 0, adsr: [0; 2], wave: 0 };

#[derive(Clone, Copy)]
struct Prog {
    tones: u8,
    vol: u8,
    pan: u8,
    /// index of the program's first tone in the tone pool
    first: u8,
}

const NO_PROG: Prog = Prog { tones: 0, vol: 0, pan: 0, first: 0 };

#[derive(Clone, Copy)]
struct Vab {
    open: bool,
    loaded: bool,
    vol: u8,
    pan: u8,
    /// sound RAM address of the body
    base: u32,
    /// body bytes, and bytes sent so far by SsVabTransBodyPartly
    size: u32,
    sent: u32,
    progs: [Prog; 128],
}

const NO_VAB: Vab =
    Vab { open: false, loaded: false, vol: 0, pan: 0, base: 0, size: 0, sent: 0, progs: [NO_PROG; 128] };

#[derive(Clone, Copy)]
struct Chan {
    prog: u8,
    vol: u8,
    pan: u8,
}

const CHAN0: Chan = Chan { prog: 0, vol: 127, pan: 64 };

const SONG_FREE: u8 = 0;
const SONG_STOPPED: u8 = 1;
const SONG_PLAYING: u8 = 2;

#[derive(Clone, Copy)]
#[repr(C)]
struct Song {
    state: u8,
    vab: i16,
    data: *const u8,
    /// byte after the end event (the copy's size), 0 when not copied
    len: usize,
    pos: usize,
    status: u8,
    res: u32,
    tempo: u32,
    /// time carried between calls, in tempo units (one tick = `tempo`)
    frac: u32,
    /// ticks until the next event
    wait: i32,
    /// time per call in tempo units, and the rate it was worked out for
    step: u32,
    rate: u32,
    /// plays left; 0 = for ever
    plays: i16,
    vol: [u8; 2],
    ch: [Chan; 16],
}

const NO_SONG: Song = Song {
    state: SONG_FREE,
    vab: 0,
    data: core::ptr::null(),
    len: 0,
    pos: 0,
    status: 0,
    res: 0,
    tempo: 0,
    frac: 0,
    wait: 0,
    step: 0,
    rate: 0,
    plays: 0,
    vol: [127, 127],
    ch: [CHAN0; 16],
};

/// One voice's registers as last set up: SsUtKeyOn's wait here for the
/// next tick. (`repr(C)`, as `Snd` and `Song`: a run can watch them.)
#[derive(Clone, Copy)]
#[repr(C)]
struct VoiceRegs {
    vol: [i16; 2],
    pitch: u16,
    start: u32,
    adsr: [u16; 2],
}

const NO_REGS: VoiceRegs = VoiceRegs { vol: [0; 2], pitch: 0, start: 0, adsr: [0; 2] };

/// Who holds a voice.
const OWNER_NONE: u8 = 0xFF;
const OWNER_UT: u8 = 0xFE;

#[repr(C)]
struct Snd {
    shadow: [VoiceRegs; VOICES],
    vabs: [Vab; MAX_VABS],
    tones: [Tone; MAX_TONES],
    ntones: usize,
    songs: [Song; MAX_SONGS],
    arena: [u8; SONG_ARENA],
    arena_used: usize,
    /// SsUtKeyOn voices to key on at the next tick
    pending_on: u32,
    tick_mode: i32,
    /// next free byte of sound RAM
    next: u32,
    /// voices held: a song's note not keyed off, or an SsUtKeyOn tone not
    /// yet seen ended
    held: u32,
    owner: [u8; VOICES],
    note: [(u8, u8); VOICES],
    master: [i16; 2],
    mono: bool,
    /// set while SsInit or a body transfer drives the sound processor;
    /// the tick then leaves it alone
    busy: bool,
}

static mut SND: Snd = Snd {
    shadow: [NO_REGS; VOICES],
    vabs: [NO_VAB; MAX_VABS],
    tones: [NO_TONE; MAX_TONES],
    ntones: 0,
    songs: [NO_SONG; MAX_SONGS],
    arena: [0; SONG_ARENA],
    arena_used: 0,
    pending_on: 0,
    tick_mode: SS_NOTICK,
    next: BODY_START,
    held: 0,
    owner: [OWNER_NONE; VOICES],
    note: [(0, 0); VOICES],
    master: [127, 127],
    mono: false,
    busy: false,
};

#[allow(static_mut_refs)]
fn snd() -> &'static mut Snd {
    // SAFETY: the foreground changes the shared tables only with interrupts
    // off (or with `busy` set, which keeps the tick out), and the tick runs
    // in the VBlank interrupt, so the two never overlap.
    unsafe { &mut SND }
}

fn critical<R>(f: impl FnOnce(&mut Snd) -> R) -> R {
    psx_io::irq::without_interrupts(|| f(snd()))
}

#[inline(always)]
unsafe fn rd16(p: *const u8, off: usize) -> u16 {
    // SAFETY: the caller's data.
    unsafe { core::ptr::read_unaligned(p.add(off) as *const u16) }
}

/// Turn the SPU on and forget every bank and song.
#[no_mangle]
pub extern "C" fn SsInit() {
    critical(|s| {
        s.busy = true;
        s.vabs = [NO_VAB; MAX_VABS];
        s.ntones = 0;
        s.songs = [NO_SONG; MAX_SONGS];
        s.arena_used = 0;
        s.next = BODY_START;
        s.held = 0;
        s.owner = [OWNER_NONE; VOICES];
        s.pending_on = 0;
    });
    #[allow(deprecated)]
    psx_spu::init();
    snd().busy = false;
}

#[no_mangle]
pub extern "C" fn SsSetTableSize(_table: *mut u8, _s_max: i16, _t_max: i16) {}

#[no_mangle]
pub extern "C" fn SsSetTickMode(mode: i32) {
    snd().tick_mode = mode;
}

#[no_mangle]
pub extern "C" fn SsStart() {}

#[no_mangle]
pub extern "C" fn SsEnd() {}

/// Main volume, 0-127 per side.
#[no_mangle]
pub extern "C" fn SsSetMVol(l: i16, r: i16) {
    let s = snd();
    s.master = [l.clamp(0, 127), r.clamp(0, 127)];
    let v = |x: i16| Volume((x as i32 * 0x3FFF / 127) as i16);
    psx_spu::set_main_volume(v(s.master[0]), v(s.master[1]));
}

#[no_mangle]
pub extern "C" fn SsSetMono() {
    snd().mono = true;
}

#[no_mangle]
pub extern "C" fn SsSetStereo() {
    snd().mono = false;
}

// ------------------------------------------------------------- banks

/// Open a bank from its header in RAM: the given id, or the first free one
/// for -1. Returns the id, or -1 (not a VAB header, no free id, or more
/// tones than the layer keeps).
#[no_mangle]
pub unsafe extern "C" fn SsVabOpenHead(addr: *const u8, vabid: i16) -> i16 {
    if addr.is_null() {
        return -1;
    }
    // SAFETY: the game's header; the 32-byte header is read before the
    // form is checked.
    if unsafe { core::ptr::read_unaligned(addr as *const u32) } != 0x5641_4270 {
        crate::console::puts("psyq: SsVabOpenHead: not a VAB header\n");
        return -1;
    }
    critical(|s| {
        let id = if vabid < 0 {
            match s.vabs.iter().position(|v| !v.open) {
                Some(i) => i,
                None => return -1,
            }
        } else if (vabid as usize) < MAX_VABS && !s.vabs[vabid as usize].open {
            vabid as usize
        } else {
            return -1;
        };
        // SAFETY: the game's header, read within the sizes it gives.
        unsafe {
            let programs = rd16(addr, 18) as usize;
            let waveforms = rd16(addr, 22) as usize;
            let sizes = 32 + 128 * 16 + programs * 16 * 32;
            let wave_off = |n: usize| -> u32 { (1..n).map(|i| rd16(addr, sizes + 2 * i) as u32 * 8).sum() };
            let size = wave_off(waveforms + 1);
            let mut v = NO_VAB;
            v.vol = *addr.add(24);
            v.pan = *addr.add(25);
            v.size = size;
            let mut block = 0;
            for p in 0..128 {
                let pe = addr.add(32 + p * 16);
                let tones = *pe;
                if tones == 0 {
                    continue;
                }
                if block >= programs || s.ntones + tones as usize > MAX_TONES || tones > 16 {
                    crate::console::puts("psyq: SsVabOpenHead: bank too large\n");
                    return -1;
                }
                v.progs[p] = Prog { tones, vol: *pe.add(1), pan: *pe.add(4), first: s.ntones as u8 };
                for k in 0..tones as usize {
                    let t = addr.add(32 + 128 * 16 + (block * 16 + k) * 32);
                    let wave = rd16(t, 22) as usize;
                    s.tones[s.ntones + k] = Tone {
                        vol: *t.add(2),
                        pan: *t.add(3),
                        centre: *t.add(4),
                        shift: *t.add(5),
                        min: *t.add(6),
                        max: *t.add(7),
                        adsr: [rd16(t, 16), rd16(t, 18)],
                        // waveform 0 or past the bank's count: no sound
                        wave: if wave == 0 || wave > waveforms { u32::MAX } else { wave_off(wave) },
                    };
                }
                s.ntones += tones as usize;
                block += 1;
            }
            v.open = true;
            s.vabs[id] = v;
        }
        id as i16
    })
}

/// Sound RAM for an open bank's body, from the next free byte up.
fn place_body(s: &mut Snd, vabid: i16) -> Option<usize> {
    if vabid < 0 || vabid as usize >= MAX_VABS || !s.vabs[vabid as usize].open {
        return None;
    }
    let id = vabid as usize;
    if s.vabs[id].base == 0 {
        if s.next + s.vabs[id].size > SPU_RAM {
            crate::console::puts("psyq: SsVabTransBody: sound RAM full\n");
            return None;
        }
        s.vabs[id].base = s.next;
        s.next = (s.next + s.vabs[id].size + 7) & !7;
    }
    Some(id)
}

fn upload(dest: u32, bytes: &[u8]) {
    snd().busy = true;
    #[allow(deprecated)]
    if psx_spu::upload_adpcm(SpuAddr::new(dest), bytes).is_err() {
        // The SDK's upload reports a transfer that timed out (and latches UPLOAD_FAULT); Psy-Q's SpuWrite had no
        // result, so the layer says so on the console and goes on, the sample missing.
        crate::console::puts("psyq: SpuWrite: transfer timed out\n");
    }
    snd().busy = false;
}

/// Copy a bank's waveforms to sound RAM. Returns the id, or -1.
#[no_mangle]
pub unsafe extern "C" fn SsVabTransBody(addr: *const u8, vabid: i16) -> i16 {
    if addr.is_null() {
        return -1;
    }
    let Some(id) = critical(|s| place_body(s, vabid)) else { return -1 };
    let s = snd();
    // SAFETY: the game's body of the size its header gives.
    let body = unsafe { core::slice::from_raw_parts(addr, s.vabs[id].size as usize) };
    upload(s.vabs[id].base, body);
    s.vabs[id].sent = s.vabs[id].size;
    s.vabs[id].loaded = true;
    vabid
}

/// Copy the next `bufsize` bytes of a bank's waveforms to sound RAM, for a
/// body read in pieces (each a whole number of 8-byte units but the last).
/// Returns the id once the whole body is there, -2 before, -1 on error.
#[no_mangle]
pub unsafe extern "C" fn SsVabTransBodyPartly(addr: *const u8, bufsize: u32, vabid: i16) -> i16 {
    if addr.is_null() {
        return -1;
    }
    let Some(id) = critical(|s| place_body(s, vabid)) else { return -1 };
    let s = snd();
    let v = s.vabs[id];
    let n = bufsize.min(v.size - v.sent) & !1;
    // SAFETY: the game's piece of `bufsize` bytes.
    let piece = unsafe { core::slice::from_raw_parts(addr, n as usize) };
    upload(v.base + v.sent, piece);
    s.vabs[id].sent += n;
    if s.vabs[id].sent < v.size {
        return -2;
    }
    s.vabs[id].loaded = true;
    vabid
}

#[no_mangle]
pub extern "C" fn SsVabClose(vabid: i16) {
    if (0..MAX_VABS as i16).contains(&vabid) {
        critical(|s| s.vabs[vabid as usize] = NO_VAB);
    }
}

// ------------------------------------------------------------- notes

/// floor(4096 * 2^((semitone + step / 16) / 12)) for 12 semitones of 16
/// steps: the 1996 library's pitch table (equal to it entry for entry).
static PITCH_TABLE: [u16; 192] = {
    // 2^(x/192) for x in 0..192, in Q32 by repeated multiplication with
    // 2^(1/192) in Q62 (integer only, exact to well under 1/4096 here)
    let mut t = [0u16; 192];
    const STEP_Q62: u128 = 4_628_364_946_339_752_272; // 2^(1/192) * 2^62, rounded down
    let mut v: u128 = 1u128 << 62;
    let mut i = 0;
    while i < 192 {
        t[i] = ((v * 4096) >> 62) as u16;
        v = v * STEP_Q62 >> 62;
        i += 1;
    }
    t
};

/// The voice pitch for a note, as the 1996 library's note2pitch2 works it
/// out from the tone's centre note and shift.
fn note_pitch(note: i32, fine: i32, centre: i32, shift: i32) -> u16 {
    let f = (fine + shift) / 8; // toward zero, as the 1996 code
    let (carry, step) = if f >= 16 { (1, f - 16) } else { (0, f) };
    let semis = (note + 60 - centre + carry) as i16 as i32;
    let oct = semis / 12;
    let rem = semis - oct * 12;
    let index = ((rem << 4) + step) as usize;
    let Some(&base) = PITCH_TABLE.get(index) else { return 0 };
    let sh = oct - 5;
    let v = if sh > 0 { (base as u32) << sh } else { (base as u32) >> (-sh) };
    v as u16
}

/// The two voice volumes for a note: `x` (per side, 0-127) squared, then
/// the pan, as described at the top.
fn voice_volume(s: &Snd, x: [i32; 2], pan: i32) -> [i16; 2] {
    let reg = |x: i32| x * x * 0x3FFF / (127 * 127);
    let (l, r) = (reg(x[0]), reg(x[1]));
    if s.mono {
        return [l as i16, r as i16];
    }
    let p = pan.clamp(0, 127);
    [(l * (128 - p).min(64) / 64) as i16, (r * p.min(64) / 64) as i16]
}

/// A voice's current envelope level (its ADSR volume register).
fn envelope(n: usize) -> u16 {
    // SAFETY: a read of the SPU's per-voice envelope register, which has no
    // side effect.
    unsafe { psx_io::read_u16(psx_hw::spu::voice::base(n as u32) + psx_hw::spu::voice::ADSR_LEVEL) }
}

/// A voice for a new note: the lowest-numbered voice that is not held and
/// silent (its envelope at 0), else the lowest not held (still in its
/// release), else the last voice (the caller keys it off first). SsUtKeyOn
/// tones the sound processor has played to their end are let go first.
fn free_voice(s: &mut Snd) -> usize {
    let ended = Voice::ended_voices() & !s.pending_on;
    for n in 0..VOICES {
        if s.owner[n] == OWNER_UT && ended & (1 << n) != 0 {
            s.held &= !(1 << n);
            s.owner[n] = OWNER_NONE;
        }
    }
    let unheld = |n: &usize| s.held & (1 << *n) == 0;
    (0..VOICES)
        .filter(unheld)
        .find(|&n| envelope(n) == 0)
        .or_else(|| (0..VOICES).find(unheld))
        .unwrap_or(VOICES - 1)
}

/// Write voice `n`'s registers (the key-on follows).
fn key_voice(n: usize, r: VoiceRegs) {
    snd().shadow[n] = r;
    let voice = Voice::new(n as u8);
    voice.set_volume(Volume(r.vol[0]), Volume(r.vol[1]));
    voice.set_pitch(Pitch::raw(r.pitch));
    voice.set_start_addr(SpuAddr::new(r.start));
    voice.set_adsr(Adsr { lower: r.adsr[0], upper: r.adsr[1] });
}

/// Key one tone; the voice used, or -1.
#[no_mangle]
pub unsafe extern "C" fn SsUtKeyOn(vabid: i16, prog: i16, tone: i16, note: i16, fine: i16, voll: i16, volr: i16) -> i16 {
    if !(0..MAX_VABS as i16).contains(&vabid) || !(0..128).contains(&prog) || !(0..16).contains(&tone) {
        return -1;
    }
    let keyed = critical(|s| {
        let v = &s.vabs[vabid as usize];
        if !v.open || !v.loaded {
            return None;
        }
        let p = v.progs[prog as usize];
        if tone as u8 >= p.tones {
            return None;
        }
        let t = s.tones[p.first as usize + tone as usize];
        if t.wave == u32::MAX {
            return None;
        }
        let (bvol, bpan, base) = (v.vol as i32, v.pan as i32, v.base);
        let n = free_voice(s);
        let f = |x: i32| x.clamp(0, 127);
        let x = |c: i32| f(c) * f(t.vol as i32) / 127 * f(p.vol as i32) / 127 * f(bvol) / 127;
        let pan = t.pan as i32 - 64 + p.pan as i32 - 64 + bpan - 64 + 64;
        s.shadow[n] = VoiceRegs {
            vol: voice_volume(s, [x(voll as i32), x(volr as i32)], pan),
            pitch: note_pitch(note as i32, fine as i32, t.centre as i32, t.shift as i32),
            start: base + t.wave,
            adsr: t.adsr,
        };
        s.pending_on |= 1 << n;
        s.held |= 1 << n;
        s.owner[n] = OWNER_UT;
        Some((n, s.tick_mode == SS_NOTICK))
    });
    let Some((n, notick)) = keyed else { return -1 };
    if !notick {
        critical(|_| {
            let on = flush_ut();
            Voice::clear_ended(on);
            Voice::start(on);
        });
    }
    n as i16
}

/// Write the SsUtKeyOn voices set up since the last tick; the voices to
/// key on.
fn flush_ut() -> u32 {
    let s = snd();
    let on = core::mem::take(&mut s.pending_on);
    for n in 0..VOICES {
        if on & (1 << n) != 0 {
            key_voice(n, s.shadow[n]);
        }
    }
    on
}

fn key_off(s: &mut Snd, mask: u32) {
    if mask != 0 {
        Voice::release(mask);
        s.held &= !mask;
        for n in 0..VOICES {
            if mask & (1 << n) != 0 {
                s.owner[n] = OWNER_NONE;
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn SsUtKeyOff(voice: i16, _vabid: i16, _prog: i16, _tone: i16, _note: i16) -> i16 {
    if !(0..VOICES as i16).contains(&voice) {
        return -1;
    }
    critical(|s| key_off(s, 1 << voice));
    0
}

#[no_mangle]
pub extern "C" fn SsUtAllKeyOff(_mode: i16) -> i16 {
    critical(|s| key_off(s, (1 << VOICES) - 1));
    0
}

// ------------------------------------------------------------- songs

/// A variable-length number at `p`; the value and the bytes it took.
unsafe fn read_vlq(p: *const u8) -> (u32, usize) {
    let mut v = 0u32;
    let mut n = 0;
    loop {
        // SAFETY: the caller's song, which ends with an end event.
        let b = unsafe { *p.add(n) };
        n += 1;
        v = (v << 7) | (b & 0x7F) as u32;
        if b & 0x80 == 0 || n == 4 {
            return (v, n);
        }
    }
}

/// Bytes of data after a status byte (not counting meta events).
fn data_len(status: u8) -> usize {
    match status & 0xF0 {
        0xC0 | 0xD0 => 1,
        _ => 2,
    }
}

const SEQ_HEADER: usize = 15;
/// Longest song the open scans for its end (a song with none is played from
/// the game's memory).
const SEQ_SCAN: usize = 0x1_0000;

/// The song's length up to and including its end event, if it has one
/// within SEQ_SCAN bytes.
unsafe fn song_len(p: *const u8) -> Option<usize> {
    let mut i = SEQ_HEADER;
    let mut status = 0u8;
    while i < SEQ_SCAN {
        // SAFETY: the game's song, read until its end event.
        unsafe {
            i += read_vlq(p.add(i)).1;
            let b = *p.add(i);
            if b & 0x80 != 0 {
                status = b;
                i += 1;
            }
            if status == 0xFF {
                let kind = *p.add(i);
                i += 1;
                match kind {
                    0x2F => return Some(i + 1),
                    0x51 => i += 3,
                    _ => return None,
                }
            } else if status < 0x80 {
                return None;
            } else {
                i += data_len(status);
            }
        }
    }
    None
}

/// A SEQ song: "pQES", version 1. Returns its id, or -1.
#[no_mangle]
pub unsafe extern "C" fn SsSeqOpen(addr: *const u32, vabid: i16) -> i16 {
    let p = addr as *const u8;
    if p.is_null() {
        crate::console::puts("psyq: SsSeqOpen: no SEQ data\n");
        return -1;
    }
    // SAFETY: the game's data, at least the header.
    unsafe {
        if core::ptr::read_unaligned(p as *const u32) != 0x5345_5170 {
            crate::console::puts("psyq: SsSeqOpen: not SEQ data\n");
            return -1;
        }
        let len = song_len(p);
        let res = (*p.add(8) as u32) << 8 | *p.add(9) as u32;
        let tempo = (*p.add(10) as u32) << 16 | (*p.add(11) as u32) << 8 | *p.add(12) as u32;
        critical(|s| {
            let Some(id) = s.songs.iter().position(|x| x.state == SONG_FREE) else { return -1 };
            let mut song = NO_SONG;
            song.vab = vabid;
            song.data = p;
            if let Some(n) = len.filter(|&n| s.arena_used + n <= SONG_ARENA) {
                let dst = s.arena.as_mut_ptr().add(s.arena_used);
                core::ptr::copy_nonoverlapping(p, dst, n);
                song.data = dst;
                song.len = n;
                s.arena_used += n;
            }
            song.res = res.max(1);
            song.tempo = tempo.max(1);
            rewind(&mut song);
            song.state = SONG_STOPPED;
            s.songs[id] = song;
            id as i16
        })
    }
}

/// Back to the first event: its delta read, as the 1996 library leaves a
/// song when it opens it.
fn rewind(song: &mut Song) {
    // SAFETY: the song's data, past its header.
    let (d, n) = unsafe { read_vlq(song.data.add(SEQ_HEADER)) };
    song.pos = SEQ_HEADER + n;
    song.wait = d as i32;
    song.frac = 0;
    song.status = 0;
}

fn song_id(seq: i16) -> Option<usize> {
    ((0..MAX_SONGS as i16).contains(&seq)).then_some(seq as usize)
}

/// Key off the voices a song holds.
fn song_off(s: &mut Snd, id: usize) {
    let mask = (0..VOICES).filter(|&n| s.owner[n] == id as u8).fold(0, |m, n| m | 1 << n);
    key_off(s, mask);
}

#[no_mangle]
pub extern "C" fn SsSeqClose(seq: i16) {
    if let Some(id) = song_id(seq) {
        critical(|s| {
            song_off(s, id);
            s.songs[id].state = SONG_FREE;
            if s.songs.iter().all(|x| x.state == SONG_FREE) {
                s.arena_used = 0;
            }
        });
    }
}

/// Play a song (mode 1) `count` times, 0 for ever; mode 0 leaves it paused.
#[no_mangle]
pub extern "C" fn SsSeqPlay(seq: i16, mode: i8, count: i16) {
    if let Some(id) = song_id(seq) {
        critical(|s| {
            let song = &mut s.songs[id];
            if song.state != SONG_FREE {
                song.plays = count.max(0);
                if mode != 0 {
                    song.state = SONG_PLAYING;
                }
            }
        });
    }
}

#[no_mangle]
pub extern "C" fn SsSeqStop(seq: i16) {
    if let Some(id) = song_id(seq) {
        critical(|s| {
            song_off(s, id);
            let song = &mut s.songs[id];
            if song.state != SONG_FREE {
                song.state = SONG_STOPPED;
                rewind(song);
            }
        });
    }
}

/// The song's volume, 0-127 per side, for the notes it plays from now on.
#[no_mangle]
pub extern "C" fn SsSeqSetVol(seq: i16, l: i16, r: i16) {
    if let Some(id) = song_id(seq) {
        critical(|s| s.songs[id].vol = [l.clamp(0, 127) as u8, r.clamp(0, 127) as u8]);
    }
}

/// A song's note-on: every tone of the channel's program whose range holds
/// the note, each on a free voice. The voices to key on.
fn note_on(s: &mut Snd, id: usize, ch: usize, note: u8, vel: u8) -> u32 {
    // Read what is needed, not a copy of the whole song and bank: this runs from the VBlank hook,
    // on the dispatcher's 400 bytes, and a copy of the bank alone is 532.
    let (song_vab, song_vol, c) = {
        let song = &s.songs[id];
        (song.vab, song.vol, song.ch[ch])
    };
    if !(0..MAX_VABS as i16).contains(&song_vab) {
        return 0;
    }
    let v = &s.vabs[song_vab as usize];
    if !v.open || !v.loaded {
        return 0;
    }
    let (v_vol, v_pan, v_base) = (v.vol, v.pan, v.base);
    let p = v.progs[c.prog as usize & 127];
    let mut on = 0;
    for k in 0..p.tones as usize {
        let t = s.tones[p.first as usize + k];
        if note < t.min || note > t.max || t.wave == u32::MAX {
            continue;
        }
        let n = free_voice(s);
        if s.held & (1 << n) != 0 {
            key_off(s, 1 << n);
        }
        let x = |side: usize| {
            vel as i32 * c.vol as i32 / 127 * song_vol[side] as i32 / 127 * p.vol as i32 / 127 * t.vol as i32 / 127
                * v_vol as i32
                / 127
        };
        let pan = t.pan as i32 - 64 + p.pan as i32 - 64 + c.pan as i32 - 64 + v_pan as i32 - 64 + 64;
        key_voice(
            n,
            VoiceRegs {
                vol: voice_volume(s, [x(0), x(1)], pan),
                pitch: note_pitch(note as i32, 0, t.centre as i32, t.shift as i32),
                start: v_base + t.wave,
                adsr: t.adsr,
            },
        );
        s.held |= 1 << n;
        s.owner[n] = id as u8;
        s.note[n] = (ch as u8, note);
        on |= 1 << n;
    }
    on
}

fn note_off(s: &mut Snd, id: usize, ch: usize, note: u8) {
    let mask =
        (0..VOICES).filter(|&n| s.owner[n] == id as u8 && s.note[n] == (ch as u8, note)).fold(0, |m, n| m | 1 << n);
    key_off(s, mask);
}

/// Play a song's events that are due, then move its time on by one call.
/// The voices to key on.
fn tick_song(s: &mut Snd, id: usize, rate: u32) -> u32 {
    let mut on = 0;
    let mut guard = 0;
    while s.songs[id].wait <= 0 {
        guard += 1;
        if guard > 4096 {
            break;
        }
        let song = &mut s.songs[id];
        let d = song.data;
        // SAFETY: the song's data; `pos` stays before its end event.
        let (status, a, b) = unsafe {
            let mut i = song.pos;
            if *d.add(i) & 0x80 != 0 {
                song.status = *d.add(i);
                i += 1;
            }
            let st = song.status;
            if st == 0xFF {
                let kind = *d.add(i);
                if kind == 0x51 {
                    song.tempo = ((*d.add(i + 1) as u32) << 16 | (*d.add(i + 2) as u32) << 8 | *d.add(i + 3) as u32)
                        .max(1);
                    song.step = 0;
                    i += 4;
                } else {
                    i += 2;
                }
                song.pos = i;
                (st, kind, 0)
            } else {
                let a = *d.add(i);
                let b = if data_len(st) == 2 { *d.add(i + 1) } else { 0 };
                song.pos = i + data_len(st);
                (st, a, b)
            }
        };
        let ch = (status & 15) as usize;
        match status & 0xF0 {
            0x90 if b > 0 => on |= note_on(s, id, ch, a, b),
            0x90 | 0x80 => note_off(s, id, ch, a),
            0xB0 if a == 7 => s.songs[id].ch[ch].vol = b,
            0xB0 if a == 10 => s.songs[id].ch[ch].pan = b,
            0xC0 => s.songs[id].ch[ch].prog = a,
            0xF0 if status == 0xFF && a == 0x2F => {
                let song = &mut s.songs[id];
                if song.plays == 1 {
                    song.state = SONG_STOPPED;
                    rewind(song);
                    song_off(s, id);
                    return on;
                }
                if song.plays > 1 {
                    song.plays -= 1;
                }
                // again from the first event, keeping the time past the end,
                // so a play lasts exactly the song's length
                let wait = song.wait;
                let frac = song.frac;
                rewind(song);
                song.wait += wait;
                song.frac = frac;
                continue;
            }
            _ => {}
        }
        let song = &mut s.songs[id];
        // SAFETY: the next event's delta.
        let (delta, n) = unsafe { read_vlq(song.data.add(song.pos)) };
        song.pos += n;
        song.wait += delta as i32;
    }
    let song = &mut s.songs[id];
    if song.step == 0 || song.rate != rate {
        song.rate = rate;
        // resolution * 10^6 / rate in 32 bits (exact for 50 and 60)
        song.step = song.res.min(40_000) * 100_000 / (rate / 10);
    }
    // time per call in tempo units, whole ticks off the wait
    let t = song.frac.saturating_add(song.step);
    song.wait -= (t / song.tempo) as i32;
    song.frac = t % song.tempo;
    on
}

/// The tick in SS_NOTICK mode: the SsUtKeyOn voices set up since the last
/// one reach the sound processor, and every playing song moves on by one
/// call's time.
#[no_mangle]
pub extern "C" fn SsSeqCalledTbyT() {
    critical(|s| {
        if s.busy {
            return;
        }
        let mut on = flush_ut();
        let rate = if crate::gpu::video_mode() == 1 { 50 } else { 60 };
        for id in 0..MAX_SONGS {
            if s.songs[id].state == SONG_PLAYING {
                on |= tick_song(s, id, rate);
            }
        }
        if on != 0 {
            Voice::clear_ended(on);
            Voice::start(on);
        }
    });
}
