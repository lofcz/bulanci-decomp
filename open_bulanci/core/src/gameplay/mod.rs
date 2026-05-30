//! **Gameplay** as an HMR surface — the live-match half of the content model.
//!
//! This is the Phase 5 split made concrete. The deterministic
//! [`EngineSimulation`](crate::engine::EngineSimulation) stays the Rust
//! *primitive* core (movement, AABB collision, hitscan, bullets, netcode), but
//! its magic numbers move into a data-driven
//! [`SimTunables`](crate::engine::SimTunables); the gamemode *rules* and the
//! spawn table live in a [`GameplayDoc`] that a Luau gamemode (named by
//! `GameplayDoc::gamemode`) and the editor can tune. A [`GameplaySurface`]
//! binds the two and implements the shared
//! [`HmrSurface`](crate::scene::HmrSurface) contract, so a live match routes
//! editor patches through the very same stack as scenes and levels — and its
//! world renders through the same [`DrawItem`](crate::scene::DrawItem) list as
//! scenes (via the [`draw`] builders and the new shape primitives).
//!
//! The one thing gameplay adds over the other surfaces is the **determinism
//! gate** ([`HmrMode`]): a networked lockstep match may only absorb edits at a
//! tick boundary, broadcast to every peer, or it desyncs. See
//! [`surface`] for the rule and its enforcement.

pub mod doc;
pub mod draw;
pub mod patch;
pub mod surface;

pub use doc::GameplayDoc;
pub use patch::{set_tunable_field, GameplayOp, GameplayPatch};
pub use surface::{GameplaySurface, HmrMode};
