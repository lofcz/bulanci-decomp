//! Declarative **levels** as HMR surfaces.
//!
//! A level is the sim-adjacent half of the content model: collision obstacles,
//! spawn points, and gamemode tunables, authored as a `*.level.json`
//! [`LevelDoc`] and addressed by the path `levels/<name>.level.json` (the same
//! path-hash mounting [`scenes`](crate::scene) use). Unlike a scene — a UI
//! control tree handed to the renderer — a level feeds the deterministic
//! gameplay simulation, so it carries its own patch vocabulary ([`LevelOp`])
//! while still implementing the shared
//! [`HmrSurface`](crate::scene::HmrSurface) contract.
//!
//! That shared contract is the point: the client's surface router offers a
//! level surface and a scene surface the *same* raw editor patch, and each
//! claims only the patches whose envelope `surface` names it (peeked before any
//! op-vocabulary parse). So a level becomes hot-reloadable "for free" the
//! moment it is an `HmrSurface` — the transport, dirty queue, and router are
//! all already generic.

pub mod doc;
pub mod patch;
pub mod surface;

pub use doc::{LevelDoc, LevelObstacle, LevelSpawn};
pub use patch::{LevelOp, LevelPatch};
pub use surface::LevelSurface;
