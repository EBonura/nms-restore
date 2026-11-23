//! Host comparison: the layer's card.rs `read_dir`, `find` and `chain` (the OLD section, verbatim apart from the
//! device type) against psx-mc's borrowed-directory BIOS forms (the NEW section: `dir::read_into`, `find_in` and
//! `chain_in`, the calls card.rs makes), on random directory images: a frame with a
//! bad checksum, `?` and `*` patterns, broken links, loops, names with no NUL, an unformatted header. The layer
//! used the OLD functions until it adopted the SDK's forms (psyq/src/card.rs); this test holds the two equal.
use psx_mc::dir::{checksum, E_CHK, E_LINK, E_NAME, E_STATE, LINK_NONE, ST_FIRST, ST_FREE, ST_LAST, ST_MIDDLE};
use psx_mc::{Block, DirOptions, Error, FRAME_SIZE};

type Frame = [u8; FRAME_SIZE];
const BLOCKS: usize = 15;
const NAME_LEN: usize = 20;

struct Img {
    frames: Vec<Frame>,
    reads: Vec<u16>,
}
impl Block for Img {
    fn read_frame(&mut self, f: u16, out: &mut Frame) -> Result<(), Error> {
        self.reads.push(f);
        *out = *self.frames.get(f as usize).ok_or(Error::OutOfRange)?;
        Ok(())
    }
    fn write_frame(&mut self, _: u16, _: &Frame) -> Result<(), Error> {
        Err(Error::OutOfRange)
    }
}
// ---- OLD: the layer's psyq/src/card.rs, verbatim apart from the device type
fn old_name_matches(pattern: &[u8], entry: &Frame) -> bool {
    let name = &entry[E_NAME..E_NAME + NAME_LEN + 1];
    let len = name.iter().position(|&b| b == 0).unwrap_or(NAME_LEN);
    let name = &name[..len];
    let mut i = 0;
    for &p in pattern {
        match p {
            b'*' => return true,
            b'?' if i < name.len() => {}
            c if i < name.len() && name[i] == c => {}
            _ => return false,
        }
        i += 1;
    }
    i == name.len()
}
fn old_read_dir(card: &mut Img) -> Result<[Frame; BLOCKS], Error> {
    let mut f = [0; FRAME_SIZE];
    card.read_frame(0, &mut f)?;
    if f[0] != b'M' || f[1] != b'C' {
        return Err(Error::NotFormatted);
    }
    let mut dir = [[0; FRAME_SIZE]; BLOCKS];
    for (i, e) in dir.iter_mut().enumerate() {
        card.read_frame(1 + i as u16, e)?;
    }
    Ok(dir)
}
fn old_find(dir: &[Frame; BLOCKS], name: &[u8]) -> Option<usize> {
    (0..BLOCKS).find(|&i| dir[i][E_STATE] == ST_FIRST && old_name_matches(name, &dir[i]))
}
fn old_chain(dir: &[Frame; BLOCKS], first: usize) -> ([u8; BLOCKS], usize) {
    let (mut blocks, mut n, mut i) = ([0; BLOCKS], 0, first);
    while n < BLOCKS {
        blocks[n] = i as u8 + 1;
        n += 1;
        let link = u16::from_le_bytes([dir[i][E_LINK], dir[i][E_LINK + 1]]) as usize;
        if dir[i][E_STATE] == ST_LAST || link >= BLOCKS {
            break;
        }
        i = link;
    }
    (blocks, n)
}

// ---- NEW: the SDK's borrowed-directory BIOS forms, called as the layer calls them (card.rs)
fn read_dir(card: &mut Img) -> Result<[Frame; BLOCKS], Error> {
    let mut dir = [[0; FRAME_SIZE]; BLOCKS];
    psx_mc::dir::read_into(card, DirOptions::BIOS, &mut dir)?;
    Ok(dir)
}
fn find(dir: &[Frame; BLOCKS], name: &[u8]) -> Option<usize> {
    psx_mc::dir::find_in(dir, name, DirOptions::BIOS)
}
fn chain(dir: &[Frame; BLOCKS], first: usize) -> ([u8; BLOCKS], usize) {
    let c = psx_mc::dir::chain_in(dir, first);
    (c.blocks, c.len)
}

struct Rng(u64);
impl Rng {
    fn n(&mut self) -> u64 {
        self.0 ^= self.0 << 13;
        self.0 ^= self.0 >> 7;
        self.0 ^= self.0 << 17;
        self.0
    }
    fn r(&mut self, m: u64) -> u64 {
        self.n() % m
    }
}

fn seal(e: &mut Frame) {
    e[E_CHK] = checksum(e);
}

fn image(rng: &mut Rng, mode: u32) -> Img {
    let mut frames = vec![[0u8; FRAME_SIZE]; 1024];
    frames[0][0] = b'M';
    frames[0][1] = b'C';
    if mode % 11 == 3 {
        frames[0][1] = b'X'; // not formatted
    }
    if mode % 13 == 4 {
        frames[0][0] = 0;
    }
    const NAMES: [&[u8]; 6] = [
        b"BASLUS-00000SAVE0",
        b"BASLUS-00000SAVE1",
        b"BASLUS-12345HIGH",
        b"A",
        b"01234567890123456789",
        b"BASLUS-00000",
    ];
    for i in 0..BLOCKS {
        let e = &mut frames[1 + i];
        let st = [ST_FIRST, ST_MIDDLE, ST_LAST, ST_FREE, 0xA1, 0x00, 0x50][rng.r(7) as usize];
        e[E_STATE] = st;
        let link = match rng.r(6) {
            0 => LINK_NONE,
            1 => rng.r(15) as u16,
            2 => rng.r(30) as u16,
            3 => i as u16,
            4 => ((i + 1) % 15) as u16,
            _ => 0xFFFE,
        };
        e[E_LINK..E_LINK + 2].copy_from_slice(&link.to_le_bytes());
        let nm = NAMES[rng.r(6) as usize];
        e[E_NAME..E_NAME + nm.len()].copy_from_slice(nm);
        if rng.r(5) == 0 {
            e[E_NAME + 20] = b'Z'; // no NUL inside the 20
        }
        if rng.r(9) == 0 {
            for k in 0..10 {
                e[E_NAME + k] = 0; // empty or short name
            }
        }
        for k in 8..12 {
            e[k] = rng.r(256) as u8; // size field noise
        }
        seal(e);
        if rng.r(4) == 0 {
            e[E_CHK] ^= 1 + rng.r(255) as u8; // bad checksum frame
        }
    }
    Img { frames, reads: vec![] }
}

fn patterns(rng: &mut Rng, dir: &[Frame; BLOCKS]) -> Vec<Vec<u8>> {
    let mut v: Vec<Vec<u8>> = [
        &b""[..], b"*", b"?", b"BAS*", b"BASLUS-0000?SAVE?", b"BASLUS-00000SAVE0", b"BASLUS-00000SAVE0X", b"BASLUS-00000SAV",
        b"BASLUS-12345HIGH", b"?ASLUS-12345HIGH*", b"A", b"A?", b"01234567890123456789", b"0123456789012345678?",
        b"012345678901234567890", b"*X", b"??",
    ]
    .iter()
    .map(|s| s.to_vec())
    .collect();
    for e in dir {
        let len = e[E_NAME..E_NAME + 21].iter().position(|&b| b == 0).unwrap_or(20);
        let mut p = e[E_NAME..E_NAME + len].to_vec();
        v.push(p.clone());
        if !p.is_empty() {
            let k = rng.r(p.len() as u64) as usize;
            p[k] = b'?';
            v.push(p.clone());
            let k = rng.r(p.len() as u64) as usize;
            p.truncate(k);
            p.push(b'*');
            v.push(p.clone());
            let k = rng.r(p.len() as u64) as usize;
            p[k] = b'#';
            v.push(p);
        }
    }
    v
}

fn main() {
    let mut rng = Rng(0x9E3779B97F4A7C15);
    let (mut imgs, mut reads_ok, mut finds, mut chains, mut hits, mut loops, mut badck, mut nf) = (0u64, 0u64, 0u64, 0u64, 0u64, 0u64, 0u64, 0u64);
    let n: u32 = std::env::args().nth(1).and_then(|s| s.parse().ok()).unwrap_or(300_000);
    for mode in 0..n {
        let mut a = image(&mut rng, mode);
        let mut b = Img { frames: a.frames.clone(), reads: vec![] };
        let (ra, rb) = (old_read_dir(&mut a), read_dir(&mut b));
        assert_eq!(a.reads, b.reads, "device read sequence differs (image {mode})");
        match (&ra, &rb) {
            (Ok(x), Ok(y)) => assert!(x == y, "read_dir frames differ (image {mode})"),
            (Err(x), Err(y)) => assert_eq!(x, y, "read_dir error differs (image {mode})"),
            _ => panic!("read_dir result differs (image {mode})"),
        }
        imgs += 1;
        let Ok(dir) = ra else {
            nf += 1;
            continue;
        };
        reads_ok += 1;
        for e in &dir {
            if checksum(e) != e[E_CHK] {
                badck += 1;
            }
        }
        for p in patterns(&mut rng, &dir) {
            finds += 1;
            let (x, y) = (old_find(&dir, &p), find(&dir, &p));
            assert_eq!(x, y, "find differs for {:?} (image {mode})", String::from_utf8_lossy(&p));
            if x.is_some() {
                hits += 1;
            }
        }
        for first in 0..BLOCKS {
            chains += 1;
            let (x, y) = (old_chain(&dir, first), chain(&dir, first));
            assert!(x == y, "chain differs from {first} (image {mode}): {:?} vs {:?}", x, y);
            if x.1 == BLOCKS {
                loops += 1;
            }
        }
    }
    println!(
        "images {imgs} (unformatted {nf}, read {reads_ok}), directory entries with a bad checksum {badck}, finds {finds} ({hits} hits), chains {chains} ({loops} ran the full 15 blocks), 0 differences; the device read sequence is identical in every image"
    );
}
