use serde::{Serialize, Deserialize};

#[derive(Debug, Clone, Default, Serialize, Deserialize, PartialEq, Eq)]
pub struct Position {
    pub x: i32, // Fixed-point: 1000 = 1.0 pixel (or just integers)
    pub y: i32,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Player {
    pub id: u8,
    pub pos: Position,
    pub velocity_x: i32,
    pub velocity_y: i32,
    pub facing_dir: u8, // 0 = UP, 1 = RIGHT, 2 = DOWN, 3 = LEFT
    pub team: u8,
    pub is_alive: bool,
    pub score: i32,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Bullet {
    pub id: u32,
    pub owner_id: u8,
    pub pos: Position,
    pub velocity_x: i32,
    pub velocity_y: i32,
    pub is_active: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct MapObstacle {
    pub x1: i32,
    pub y1: i32,
    pub x2: i32,
    pub y2: i32,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Timer {
    pub slot_id: i32,
    pub remaining_ticks: i32,
    pub flags: i32,
}

#[derive(Debug, Clone, Default, Serialize, Deserialize, PartialEq, Eq)]
pub struct WorldState {
    pub tick: u64,
    pub players: Vec<Player>,
    pub bullets: Vec<Bullet>,
    pub obstacles: Vec<MapObstacle>,
    pub timers: Vec<Timer>,
    pub game_started: bool,
}

#[derive(Debug, Clone, Default, Serialize, Deserialize, PartialEq, Eq)]
pub struct FrameInputs {
    pub up: bool,
    pub down: bool,
    pub left: bool,
    pub right: bool,
    pub shoot: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum ClientMessage {
    Connect { name: String },
    Input { tick: u64, inputs: FrameInputs },
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum ServerMessage {
    Welcome { assigned_id: u8, initial_state: WorldState },
    StateUpdate { state: WorldState },
    PlayerJoined { id: u8, name: String },
    PlayerLeft { id: u8 },
}

pub mod clock;
pub mod scheduler;
pub mod engine;
pub mod luau_vm;
pub mod assets;
pub mod scene;
pub mod level;
pub mod gameplay;

pub use clock::EngineClock;
pub use scheduler::Scheduler;
