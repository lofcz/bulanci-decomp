//! The **HMR surface** abstraction — the reusable contract every
//! live-patchable thing in the game implements.
//!
//! A *surface* is any addressable, hot-reloadable unit the editor can edit
//! while the game runs: a declarative scene today (the menu), and, as the
//! system grows, intro/connecting screens, dialogs/HUD overlays, levels, and
//! eventually the gameplay sim. The client owns an ordered stack of surfaces
//! (see the client `surface` module); every editor edit funnels through the
//! same two channels:
//!
//!   * **byte overlays** (`AssetServer`'s dirty queue) — when a surface's
//!     dependency bytes change, [`HmrSurface::needs_rebuild`] decides whether
//!     a from-scratch rebuild is required (e.g. a `.luau` code edit);
//!   * **patches** (`AssetServer`'s scene-patch queue) — a structural/visual
//!     diff that [`HmrSurface::apply_raw_patch`] applies *in place*, preserving
//!     runtime state (the Luau VM, scheduler, timers, surviving node identity).
//!
//! Keeping this a tiny trait (matching the codebase's `AssetClass`/`FontProvider`
//! convention) lets the client hold a heterogeneous `Vec<Box<dyn HmrSurface>>`
//! router and dispatch a drained patch to whichever surface claims it — the
//! routing happens once per patch per frame, never on a hot path.

use std::collections::HashSet;

use crate::assets::hash_path;

/// What happened when a surface was offered a raw editor patch.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PatchOutcome {
    /// The patch targeted this surface and was applied live, in place.
    Applied,
    /// The patch is addressed to a different surface — try the next one.
    NotMine,
    /// The patch targeted this surface but couldn't be applied live (parse
    /// error, or a non-patchable surface). The caller may fall back to a full
    /// rebuild via the byte-overlay path.
    Failed,
}

/// A live-patchable game surface. Implementors decode their own patch
/// vocabulary from the raw wire bytes, so each surface kind (scene, sim, …)
/// can evolve an independent op set behind one router.
pub trait HmrSurface {
    /// The routing key the editor addresses this surface by (a scene name
    /// today). Patches whose envelope `surface` matches are this surface's.
    fn surface_name(&self) -> &str;

    /// VFS paths whose dirtiness should force a from-scratch rebuild of this
    /// surface (its doc/module + scripts + candidate libs). Used by the
    /// default [`needs_rebuild`](HmrSurface::needs_rebuild).
    fn deps(&self) -> &[String];

    /// Apply a raw editor patch (the JSON wire payload) in place. The
    /// implementor parses its own op type, checks the envelope's `surface`
    /// against [`surface_name`](HmrSurface::surface_name), and mutates live
    /// state without disturbing surviving node identity.
    fn apply_raw_patch(&self, raw: &[u8]) -> PatchOutcome;

    /// Whether any of this surface's [`deps`](HmrSurface::deps) is in `dirty`
    /// (hashed by the canonical path hash), i.e. a byte edit landed that this
    /// surface can only absorb by rebuilding. The default covers every
    /// path-addressed surface; specialised surfaces may override.
    fn needs_rebuild(&self, dirty: &HashSet<u64>) -> bool {
        self.deps().iter().any(|p| dirty.contains(&hash_path(p)))
    }
}
