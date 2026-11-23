//! The names of a Psy-Q path, as `psx_io::iso::find_in_directory` takes them.

/// A path has at most this many directory levels.
pub const MAX_DEPTH: usize = 8;
/// Longest path (bytes) `CdSearchFile` takes.
pub const MAX_PATH: usize = 96;

/// The names of `path` (`\DIR\FILE.EXT;1`, `/` too), upper case, each without its `;version`,
/// in `text`'s bytes; and how many there are. `None` when the path is too long or too deep.
/// ISO 9660 writes upper case; the C callers' names may not be.
pub fn split<'t>(
    path: &[u8],
    text: &'t mut [u8; MAX_PATH],
) -> Option<([&'t [u8]; MAX_DEPTH], usize)> {
    text.get_mut(..path.len())?.copy_from_slice(path);
    text.make_ascii_uppercase();
    let mut names: [&[u8]; MAX_DEPTH] = [&[]; MAX_DEPTH];
    let mut count = 0;
    for part in text[..path.len()].split(|&c| c == b'\\' || c == b'/').filter(|p| !p.is_empty()) {
        let name = part.split(|&c| c == b';').next()?;
        *names.get_mut(count)? = name;
        count += 1;
    }
    Some((names, count))
}

#[cfg(test)]
mod tests {
    use super::*;

    fn names(path: &str) -> Option<Vec<String>> {
        let mut text = [0; MAX_PATH];
        let (names, count) = split(path.as_bytes(), &mut text)?;
        Some(names[..count].iter().map(|n| String::from_utf8_lossy(n).into_owned()).collect())
    }

    #[test]
    fn the_paths_the_restored_games_ask_for() {
        assert_eq!(names("\\360X.DAT;1").unwrap(), ["360X.DAT"]);
        assert_eq!(names("\\QDUP.DAT;1").unwrap(), ["QDUP.DAT"]);
    }

    #[test]
    fn other_forms_of_the_same_names() {
        assert_eq!(names("360x.dat").unwrap(), ["360X.DAT"]);
        assert_eq!(names("/DIR//a.bin;1").unwrap(), ["DIR", "A.BIN"]);
        assert!(names("").unwrap().is_empty());
        assert!(names(&"A".repeat(97)).is_none());
        assert!(names("\\A\\B\\C\\D\\E\\F\\G\\H\\I").is_none());
    }
}
