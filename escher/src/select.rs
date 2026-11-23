//! E_SelectObject's selection. The game selects by name pattern dozens of
//! times a frame (setup_detail_level_of_room, draw_3d), always the same
//! patterns over the same objects, so each pattern's result is kept as a
//! bit set (cleared when E_LoadMapData sees new meshes), looked up from where
//! the last one was found.

use crate::nms::{name_matches, object_list, Object};

// NMS's per-object `selected` byte is left alone: only Escher read it.

const WORDS: usize = 8; // 256 objects
const SLOTS: usize = 64;

#[derive(Clone, Copy)]
struct Entry {
    hash: u32,
    pattern: [u32; 4],
    bits: [u32; WORDS],
}

static mut CURRENT: [u32; WORDS] = [0; WORDS];
static mut CACHE: [Entry; SLOTS] = [Entry { hash: 0, pattern: [0; 4], bits: [0; WORDS] }; SLOTS];
static mut USED: usize = 0;
static mut NEXT: usize = 0;
/// the slot after the last hit: the game asks in the same order each frame
static mut GUESS: usize = 0;

pub fn forget() {
    // SAFETY: foreground-only statics.
    unsafe {
        USED = 0;
        NEXT = 0;
        CURRENT = [0; WORDS];
    }
}

pub fn select(name: *const u8) {
    let objs = object_list();
    let mut key = [0u32; 4];
    let mut cacheable = false;
    for i in 0..16 {
        // SAFETY: a NUL-terminated C string from the game.
        let c = unsafe { *name.add(i) };
        if c == 0 {
            cacheable = true;
            break;
        }
        key[i >> 2] |= (c as u32) << ((i & 3) * 8);
    }
    let hash = key[0] ^ key[1].rotate_left(7) ^ key[2].rotate_left(13) ^ key[3].rotate_left(19);
    // SAFETY: foreground-only statics.
    unsafe {
        let cache = &mut *(&raw mut CACHE);
        let cur = &mut *(&raw mut CURRENT);
        if cacheable && USED > 0 {
            let start = GUESS % USED;
            for k in 0..USED {
                let s = (start + k) % USED;
                let e = &cache[s];
                if e.hash == hash && e.pattern == key {
                    *cur = e.bits;
                    GUESS = s + 1;
                    return;
                }
            }
        }
        *cur = [0; WORDS];
        for (i, o) in objs.iter().enumerate().take(WORDS * 32) {
            if name_matches(name, &o.object_name) {
                cur[i >> 5] |= 1 << (i & 31);
            }
        }
        if cacheable {
            cache[NEXT] = Entry { hash, pattern: key, bits: *cur };
            NEXT = (NEXT + 1) % SLOTS;
            USED = (USED + 1).min(SLOTS);
        }
    }
}

/// Run `f` on every selected object.
pub fn each(f: impl Fn(&mut Object)) {
    let objs = object_list();
    // SAFETY: foreground-only static.
    let cur = unsafe { &*(&raw const CURRENT) };
    for (w, &word) in cur.iter().enumerate() {
        let mut bits = word;
        while bits != 0 {
            let b = bits.trailing_zeros() as usize;
            bits &= bits - 1;
            if let Some(o) = objs.get_mut(w * 32 + b) {
                f(o);
            }
        }
    }
}
