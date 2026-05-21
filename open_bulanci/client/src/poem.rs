//! `CPoemScroller` poem loading + parsing + alignment metadata.
//!
//! Poems live as `poems/poem_<N>.txt` in the VFS (extracted from the
//! original `.dat` resource at offline-asset-pack time). Each line may
//! carry an optional `[CENTER]` / `[RIGHT]` prefix; otherwise it left-aligns.
//! See `main_menu.md` §9.3 for the scroller geometry and timing.

use bulanci_core::assets::AssetFileSystem;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Align {
    Left,
    Center,
    Right,
}

#[derive(Debug, Clone)]
pub struct PoemLine {
    pub text: String,
    pub align: Align,
}

/// Loads up to 13 poems (matches the original asset count). Returns an
/// empty `Vec` if the VFS doesn't have any.
pub fn load_poems(vfs: &AssetFileSystem) -> Vec<Vec<PoemLine>> {
    let mut poems = Vec::new();
    for i in 0..13 {
        let key = format!("poems/poem_{}.txt", i);
        if let Some(Ok(content)) = vfs.read_to_string(&key) {
            let parsed = content
                .lines()
                .map(parse_poem_line)
                .collect::<Vec<_>>();
            poems.push(parsed);
        }
    }
    poems
}

pub fn parse_poem_line(line: &str) -> PoemLine {
    let t = line.trim();
    if let Some(rest) = t.strip_prefix("[CENTER]") {
        PoemLine { text: rest.trim().to_string(), align: Align::Center }
    } else if let Some(rest) = t.strip_prefix("[RIGHT]") {
        PoemLine { text: rest.trim().to_string(), align: Align::Right }
    } else {
        PoemLine { text: t.to_string(), align: Align::Left }
    }
}

#[cfg(test)]
#[path = "poem_tests.rs"]
mod tests;
