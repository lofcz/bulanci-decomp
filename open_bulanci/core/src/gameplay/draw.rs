//! Gameplay world → [`DrawItem`] builders.
//!
//! These turn the deterministic sim's world state into the same flat
//! [`DrawItem`] list scenes produce, so a live match composes in the client's
//! [`SurfaceStack`] alongside a HUD scene overlay instead of being a separate
//! immediate-mode draw path. They lean on the shape primitives added to
//! [`NodeKind`](crate::scene::NodeKind) (`Quad`/`Circle`/`Line`).
//!
//! The builders are pure functions of world state (no client, no raylib), so
//! they're unit-testable and reusable by both the composable
//! [`GameplaySurface`](super::surface::GameplaySurface) draw list and the
//! client's predicted-world render (which overrides colors to distinguish the
//! local player).

use crate::engine::SimTunables;
use crate::scene::{DrawItem, NodeKind, Transform};
use crate::{Bullet, MapObstacle, Player};

/// Dark fill for static obstacles (RGBA8 `0xRRGGBBAA`).
pub const OBSTACLE_COLOR: u32 = 0x4B_4B_4B_FF;
/// Outline accent for obstacles.
pub const OBSTACLE_EDGE_COLOR: u32 = 0x80_80_80_FF;
/// Facing-indicator line color (yellow).
pub const FACING_COLOR: u32 = 0xFF_FF_00_FF;
/// Active bullet color (orange).
pub const BULLET_COLOR: u32 = 0xFF_A5_00_FF;
/// Default team colors when the caller doesn't override (e.g. the local
/// player is highlighted by the client). Team 0 green, team 1 red, else gray.
pub const TEAM0_COLOR: u32 = 0x00_C8_00_FF;
pub const TEAM1_COLOR: u32 = 0xC8_00_00_FF;
pub const NEUTRAL_COLOR: u32 = 0xC8_C8_C8_FF;
/// Bullet radius in world units (kept constant; not a balance tunable).
pub const BULLET_RADIUS: f32 = 4.0;

/// A pixel offset for the facing indicator line. 0 = UP, 1 = RIGHT, 2 = DOWN,
/// 3 = LEFT. (Shared with the client so the predicted and composed renders
/// match exactly.)
pub fn facing_offset(facing_dir: u8) -> (f32, f32) {
    match facing_dir {
        0 => (0.0, -25.0),
        1 => (25.0, 0.0),
        2 => (0.0, 25.0),
        3 => (-25.0, 0.0),
        _ => (0.0, 0.0),
    }
}

/// Default body color for a player by team.
pub fn team_color(team: u8) -> u32 {
    match team {
        0 => TEAM0_COLOR,
        1 => TEAM1_COLOR,
        _ => NEUTRAL_COLOR,
    }
}

fn item(kind: NodeKind, x: f32, y: f32) -> DrawItem {
    DrawItem {
        id: 0,
        world: Transform::at(x, y),
        kind,
        tint: 0xFFFF_FFFF,
    }
}

/// A filled rect for one obstacle (top-left anchored, matching the sim's
/// `(x1,y1)-(x2,y2)` AABB).
pub fn obstacle_item(obs: &MapObstacle) -> DrawItem {
    item(
        NodeKind::Quad {
            w: (obs.x2 - obs.x1) as f32,
            h: (obs.y2 - obs.y1) as f32,
            color: OBSTACLE_COLOR,
        },
        obs.x1 as f32,
        obs.y1 as f32,
    )
}

/// The body circle + facing line for one player, in `color`. The radius tracks
/// the tunable player width so a balance edit visibly resizes players.
pub fn player_items(player: &Player, color: u32, tunables: SimTunables) -> Vec<DrawItem> {
    let (px, py) = (player.pos.x as f32, player.pos.y as f32);
    let radius = (tunables.player_w as f32 / 2.0).max(1.0);
    let (dx, dy) = facing_offset(player.facing_dir);
    vec![
        item(NodeKind::Circle { radius, color }, px, py),
        item(
            NodeKind::Line { dx, dy, thickness: 3.0, color: FACING_COLOR },
            px,
            py,
        ),
    ]
}

/// One active bullet as a small circle.
pub fn bullet_item(bullet: &Bullet) -> DrawItem {
    item(
        NodeKind::Circle { radius: BULLET_RADIUS, color: BULLET_COLOR },
        bullet.pos.x as f32,
        bullet.pos.y as f32,
    )
}

/// Build the full painter-ordered world draw list (obstacles → alive players →
/// active bullets) with default team colors. The client may instead compose
/// the per-primitive builders to recolor the local player; this is the generic
/// path used by [`GameplaySurface::draw_list`](super::surface::GameplaySurface).
pub fn world_draw_items(
    obstacles: &[MapObstacle],
    players: &[Player],
    bullets: &[Bullet],
    tunables: SimTunables,
) -> Vec<DrawItem> {
    let mut out = Vec::new();
    for obs in obstacles {
        out.push(obstacle_item(obs));
    }
    for p in players {
        if p.is_alive {
            out.extend(player_items(p, team_color(p.team), tunables));
        }
    }
    for b in bullets {
        if b.is_active {
            out.push(bullet_item(b));
        }
    }
    out
}
