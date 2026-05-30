//! Minimal-footprint scene engine: a primitive node graph + a Luau host.
//!
//! The Rust side owns only **primitives** — a transform/arena node graph
//! ([`graph`]) and the VM wiring + scheduler ([`host`]). Higher-level
//! widgets (button, frame, list) and all behavior are composed in Luau on
//! top of the versioned `bulanci.*` API. The engine never renders or
//! plays audio itself: the client walks [`host::SceneHost::draw_list`] and
//! drains queued audio/scene commands, so core stays free of raylib.

pub mod control;
pub mod document;
pub mod graph;
pub mod host;
pub mod manager;
pub mod patch;
pub mod registry;
pub mod surface;

pub use control::{build_into, Binding, BuiltScene, ControlNode, Layout, NodeRef, DEFAULT_VIEWPORT};
pub use document::{SceneDoc, StateInit};
pub use graph::{DrawItem, InputEvent, Node, NodeId, NodeKind, Scene, TextAnchor, Transform, ROOT};
pub use host::{
    AtlasInfo, FrameInput, HostCommand, HostServices, Phase, SceneHost, API_VERSION,
};
pub use manager::{SceneManager, SceneReader};
pub use patch::{PatchOp, ScenePatch};
pub use surface::{HmrSurface, PatchOutcome};
