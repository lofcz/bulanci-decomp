//! Phantom-typed asset handle.
//!
//! An `AssetHandle<T>` is a `u64` plus a zero-sized type tag.  The
//! tag is one of the marker structs in [`super::types`] (`BitmapJpeg`,
//! `AudioBank`, …) so the loader can produce the right concrete type
//! without a runtime cast and miswiring fails at compile time:
//!
//! ```compile_fail
//! # use crate::asset::AssetHandle;
//! # use crate::asset::types::{BitmapSpecial, AudioBank};
//! fn play(_: AssetHandle<AudioBank>) {}
//! let img: AssetHandle<BitmapSpecial> = AssetHandle::new(0xdead_beef);
//! play(img); // ❌ type mismatch — caught by the compiler
//! ```
//!
//! The `u64` is `blake2b("<folder>/<slug>", digest_size = 8)` —
//! stable across runs, no salt.  See FOLDERS.md.

use std::fmt;
use std::hash::{Hash, Hasher};
use std::marker::PhantomData;

#[repr(transparent)]
pub struct AssetHandle<T> {
    /// The 64-bit hash of "<folder>/<slug>".  Keep public-but-named
    /// (`raw()`) so call sites can use it as a map key without having
    /// to touch the marker type.
    id: u64,
    _t: PhantomData<fn() -> T>,
}

impl<T> AssetHandle<T> {
    /// Construct a handle from a precomputed hash.  Used by the
    /// auto-generated `crate::generated::assets` consts; you should
    /// never need to call this by hand.
    pub const fn new(id: u64) -> Self {
        Self { id, _t: PhantomData }
    }

    /// Raw 64-bit identifier — the only thing the binary actually
    /// carries about this asset.  Suitable as a `HashMap` key.
    #[inline]
    pub const fn raw(self) -> u64 { self.id }
}

// ----------------------------------------------------------------------
// Manual derives — `#[derive(Clone, Copy, ...)]` would require `T: Clone`
// etc., which is wrong: the marker `T` is purely a phantom tag and need
// not satisfy any bounds for the handle itself to be `Copy`.
// ----------------------------------------------------------------------

impl<T> Clone for AssetHandle<T> {
    #[inline] fn clone(&self) -> Self { *self }
}
impl<T> Copy for AssetHandle<T> {}

impl<T> PartialEq for AssetHandle<T> {
    #[inline] fn eq(&self, other: &Self) -> bool { self.id == other.id }
}
impl<T> Eq for AssetHandle<T> {}

impl<T> Hash for AssetHandle<T> {
    #[inline] fn hash<H: Hasher>(&self, state: &mut H) { self.id.hash(state) }
}

impl<T> fmt::Debug for AssetHandle<T> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        // Print as the same 16-hex-digit shape the manifest uses, so
        // a debug log can be grepped directly against manifest.json.
        write!(f, "AssetHandle<{}>(0x{:016x})",
               std::any::type_name::<T>(), self.id)
    }
}

/// Erase the type tag so multiple kinds of handle can sit in the same
/// container (e.g. an `AssetCache` keyed by `RawHandle`).  Going the
/// other way (`RawHandle` → `AssetHandle<T>`) is intentionally not
/// provided — that's exactly the safety the type tag buys us.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash, PartialOrd, Ord)]
#[repr(transparent)]
pub struct RawHandle(pub u64);

impl<T> From<AssetHandle<T>> for RawHandle {
    #[inline]
    fn from(h: AssetHandle<T>) -> Self { RawHandle(h.id) }
}
