use crate::{WorldState, Player, Position, MapObstacle, Timer, Bullet};

pub const TICK_RATE: u64 = 60;
pub const PLAYER_WIDTH: i32 = 40;
pub const PLAYER_HEIGHT: i32 = 40;
pub const BULLET_SPEED: i32 = 8;
pub const PLAYER_SPEED: i32 = 4;

#[derive(Debug, Clone, Default)]
pub struct EngineSimulation {
    pub state: WorldState,
    pub pending_spawn_points: Vec<Position>,
}

impl EngineSimulation {
    pub fn new() -> Self {
        EngineSimulation {
            state: WorldState::default(),
            pending_spawn_points: Vec::new(),
        }
    }

    /// Reset simulation state.
    pub fn reset(&mut self) {
        self.state = WorldState::default();
        self.pending_spawn_points.clear();
    }

    /// Add player to the simulation.
    pub fn add_player(&mut self, id: u8, team: u8, x: i32, y: i32) {
        if !self.state.players.iter().any(|p| p.id == id) {
            self.state.players.push(Player {
                id,
                pos: Position { x, y },
                velocity_x: 0,
                velocity_y: 0,
                facing_dir: 1, // Default facing right
                team,
                is_alive: true,
                score: 0,
            });
        }
    }

    /// Add an obstacle dynamically.
    pub fn add_obstacle(&mut self, x1: i32, y1: i32, x2: i32, y2: i32) {
        self.state.obstacles.push(MapObstacle { x1, y1, x2, y2 });
    }

    /// Set an obstacle's bounds.
    pub fn set_obstacle_bounds(&mut self, index: usize, x1: i32, y1: i32, x2: i32, y2: i32) {
        if index < self.state.obstacles.len() {
            self.state.obstacles[index] = MapObstacle { x1, y1, x2, y2 };
        }
    }

    /// Add a timer dynamically.
    pub fn add_timer(&mut self, slot_id: i32, delay_ticks: i32, flags: i32) {
        if let Some(timer) = self.state.timers.iter_mut().find(|t| t.slot_id == slot_id) {
            timer.remaining_ticks = delay_ticks;
            timer.flags = flags;
        } else {
            self.state.timers.push(Timer {
                slot_id,
                remaining_ticks: delay_ticks,
                flags,
            });
        }
    }

    /// Stop or remove a timer.
    pub fn stop_timer(&mut self, slot_id: i32) {
        self.state.timers.retain(|t| t.slot_id != slot_id);
    }

    /// Fire bullet.
    pub fn fire_bullet(&mut self, owner_id: u8) {
        let player = match self.state.players.iter().find(|p| p.id == owner_id && p.is_alive) {
            Some(p) => p.clone(),
            None => return,
        };

        // Spawning position based on direction
        let mut spawn_pos = player.pos.clone();
        let (vel_x, vel_y) = match player.facing_dir {
            0 => { // UP
                spawn_pos.y -= PLAYER_HEIGHT / 2 + 5;
                (0, -BULLET_SPEED)
            }
            1 => { // RIGHT
                spawn_pos.x += PLAYER_WIDTH / 2 + 5;
                (BULLET_SPEED, 0)
            }
            2 => { // DOWN
                spawn_pos.y += PLAYER_HEIGHT / 2 + 5;
                (0, BULLET_SPEED)
            }
            3 => { // LEFT
                spawn_pos.x -= PLAYER_WIDTH / 2 + 5;
                (-BULLET_SPEED, 0)
            }
            _ => (0, 0),
        };

        let bullet_id = self.state.bullets.len() as u32 + 1;
        self.state.bullets.push(Bullet {
            id: bullet_id,
            owner_id,
            pos: spawn_pos,
            velocity_x: vel_x,
            velocity_y: vel_y,
            is_active: true,
        });
    }

    /// Tick simulation forward by 1 frame.
    /// Returns a list of expired timer slot IDs that should trigger OnTimer events.
    pub fn tick(&mut self, inputs: &[Option<crate::FrameInputs>]) -> Vec<i32> {
        self.state.tick += 1;

        // 1. Process player movement & input
        for player_idx in 0..self.state.players.len() {
            let player_id = self.state.players[player_idx].id;
            if !self.state.players[player_idx].is_alive {
                continue;
            }

            if let Some(Some(input)) = inputs.get(player_id as usize) {
                let mut move_x = 0;
                let mut move_y = 0;

                if input.up {
                    move_y -= PLAYER_SPEED;
                    self.state.players[player_idx].facing_dir = 0;
                } else if input.down {
                    move_y += PLAYER_SPEED;
                    self.state.players[player_idx].facing_dir = 2;
                }

                if input.left {
                    move_x -= PLAYER_SPEED;
                    self.state.players[player_idx].facing_dir = 3;
                } else if input.right {
                    move_x += PLAYER_SPEED;
                    self.state.players[player_idx].facing_dir = 1;
                }

                self.state.players[player_idx].velocity_x = move_x;
                self.state.players[player_idx].velocity_y = move_y;

                if input.shoot {
                    // Handled separately or as discrete events; for simple tick, we trigger fire
                    // In true rollback we only fire on state change (just_pressed) to avoid firing every tick
                }
            } else {
                self.state.players[player_idx].velocity_x = 0;
                self.state.players[player_idx].velocity_y = 0;
            }

            // Move X axis & resolve collision
            let orig_x = self.state.players[player_idx].pos.x;
            self.state.players[player_idx].pos.x += self.state.players[player_idx].velocity_x;
            if self.check_player_collision(player_idx) {
                self.state.players[player_idx].pos.x = orig_x;
            }

            // Move Y axis & resolve collision
            let orig_y = self.state.players[player_idx].pos.y;
            self.state.players[player_idx].pos.y += self.state.players[player_idx].velocity_y;
            if self.check_player_collision(player_idx) {
                self.state.players[player_idx].pos.y = orig_y;
            }
        }

        // 2. Process bullets
        for b_idx in 0..self.state.bullets.len() {
            if !self.state.bullets[b_idx].is_active {
                continue;
            }

            self.state.bullets[b_idx].pos.x += self.state.bullets[b_idx].velocity_x;
            self.state.bullets[b_idx].pos.y += self.state.bullets[b_idx].velocity_y;

            // Check collision with obstacles
            if self.check_bullet_obstacle_collision(b_idx) {
                self.state.bullets[b_idx].is_active = false;
                continue;
            }

            // Check collision with players
            if let Some(hit_player_id) = self.check_bullet_player_collision(b_idx) {
                self.state.bullets[b_idx].is_active = false;
                self.kill_player(hit_player_id);
                
                // Award score to owner
                let owner_id = self.state.bullets[b_idx].owner_id;
                if let Some(owner) = self.state.players.iter_mut().find(|p| p.id == owner_id) {
                    owner.score += 1;
                }
            }
        }

        // Remove inactive bullets
        self.state.bullets.retain(|b| b.is_active);

        // 3. Process timers
        let mut expired_timers = Vec::new();
        for timer in self.state.timers.iter_mut() {
            if timer.remaining_ticks > 0 {
                timer.remaining_ticks -= 1;
                if timer.remaining_ticks == 0 {
                    expired_timers.push(timer.slot_id);
                }
            }
        }

        expired_timers
    }

    fn kill_player(&mut self, player_id: u8) {
        if let Some(player) = self.state.players.iter_mut().find(|p| p.id == player_id) {
            player.is_alive = false;
            // Spawning a timer to respawn player or handling respawns externally
        }
    }

    /// Check if a player intersects any map obstacles or other players
    fn check_player_collision(&self, player_idx: usize) -> bool {
        let player = &self.state.players[player_idx];
        let p_left = player.pos.x - PLAYER_WIDTH / 2;
        let p_right = player.pos.x + PLAYER_WIDTH / 2;
        let p_top = player.pos.y - PLAYER_HEIGHT / 2;
        let p_bottom = player.pos.y + PLAYER_HEIGHT / 2;

        // Screen boundary collision checking
        if p_left < 0 || p_right > 800 || p_top < 0 || p_bottom > 600 {
            return true;
        }

        // Collision with map obstacles (AABB)
        for obs in &self.state.obstacles {
            if p_right > obs.x1 && p_left < obs.x2 && p_bottom > obs.y1 && p_top < obs.y2 {
                return true;
            }
        }

        // Collision with other players (AABB)
        for (i, other) in self.state.players.iter().enumerate() {
            if i == player_idx || !other.is_alive {
                continue;
            }
            let o_left = other.pos.x - PLAYER_WIDTH / 2;
            let o_right = other.pos.x + PLAYER_WIDTH / 2;
            let o_top = other.pos.y - PLAYER_HEIGHT / 2;
            let o_bottom = other.pos.y + PLAYER_HEIGHT / 2;

            if p_right > o_left && p_left < o_right && p_bottom > o_top && p_top < o_bottom {
                return true;
            }
        }

        false
    }

    /// Check if a bullet intersects any obstacles
    fn check_bullet_obstacle_collision(&self, b_idx: usize) -> bool {
        let bullet = &self.state.bullets[b_idx];
        
        // Out of bounds
        if bullet.pos.x < 0 || bullet.pos.x > 800 || bullet.pos.y < 0 || bullet.pos.y > 600 {
            return true;
        }

        // Check map obstacles
        for obs in &self.state.obstacles {
            if bullet.pos.x >= obs.x1 && bullet.pos.x <= obs.x2 && bullet.pos.y >= obs.y1 && bullet.pos.y <= obs.y2 {
                return true;
            }
        }

        false
    }

    /// Check if a bullet intersects any alive players (returns hit player id)
    fn check_bullet_player_collision(&self, b_idx: usize) -> Option<u8> {
        let bullet = &self.state.bullets[b_idx];

        for player in &self.state.players {
            if player.id == bullet.owner_id || !player.is_alive {
                continue;
            }

            let p_left = player.pos.x - PLAYER_WIDTH / 2;
            let p_right = player.pos.x + PLAYER_WIDTH / 2;
            let p_top = player.pos.y - PLAYER_HEIGHT / 2;
            let p_bottom = player.pos.y + PLAYER_HEIGHT / 2;

            if bullet.pos.x >= p_left && bullet.pos.x <= p_right && bullet.pos.y >= p_top && bullet.pos.y <= p_bottom {
                return Some(player.id);
            }
        }

        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::FrameInputs;

    #[test]
    fn test_player_movement_and_bounds() {
        let mut sim = EngineSimulation::new();
        // Add player 1 at center
        sim.add_player(1, 0, 100, 100);

        // Inputs to move right
        let mut inputs = vec![None; 2];
        inputs[1] = Some(FrameInputs {
            up: false,
            down: false,
            left: false,
            right: true,
            shoot: false,
        });

        // Tick simulation
        sim.tick(&inputs);

        // Player should have moved PLAYER_SPEED to the right (x = 100 + 4 = 104)
        assert_eq!(sim.state.players[0].pos.x, 104);
        assert_eq!(sim.state.players[0].pos.y, 100);
    }

    #[test]
    fn test_player_collision_with_obstacle() {
        let mut sim = EngineSimulation::new();
        // Add player 1 at 100, 100
        sim.add_player(1, 0, 100, 100);
        // Add obstacle to the right of the player: player edge is 100 + 20 = 120.
        // Let's place obstacle at x1=122, y1=80, x2=200, y2=120
        sim.add_obstacle(122, 80, 200, 120);

        // Inputs to move right (PLAYER_SPEED is 4, so would reach x=104, edge=124, which overlaps x1=122)
        let mut inputs = vec![None; 2];
        inputs[1] = Some(FrameInputs {
            up: false,
            down: false,
            left: false,
            right: true,
            shoot: false,
        });

        sim.tick(&inputs);

        // Collision should prevent player from moving, restoring original position
        assert_eq!(sim.state.players[0].pos.x, 100);
    }

    #[test]
    fn test_bullet_collision_and_hit() {
        let mut sim = EngineSimulation::new();
        // Add player 1 (shooter) at 100, 100
        sim.add_player(1, 0, 100, 100);
        // Add player 2 (target) at 200, 100 (facing left)
        sim.add_player(2, 1, 200, 100);

        // Set player 1 to face Right (1)
        sim.state.players[0].facing_dir = 1;

        // Fire bullet! Spawns bullet at 100 + 20 + 5 = 125, flying right with speed 8
        sim.fire_bullet(1);
        assert_eq!(sim.state.bullets.len(), 1);
        assert_eq!(sim.state.bullets[0].pos.x, 125);
        assert_eq!(sim.state.bullets[0].pos.y, 100);

        // Tick simulation several times to fly bullet towards player 2
        // Player 2 width is 40, so left edge is 200 - 20 = 180.
        // Ticks:
        // Tick 1: 125 + 8 = 133
        // Tick 2: 133 + 8 = 141
        // ...
        // Tick 7: 125 + 7*8 = 181 (inside player 2 bounds [180, 220])
        let inputs = vec![None; 3];
        for _ in 0..7 {
            sim.tick(&inputs);
        }

        // Bullet should have hit player 2, making player 2 dead, bullet inactive/deleted, and awarding score
        assert!(!sim.state.players[1].is_alive);
        assert_eq!(sim.state.bullets.len(), 0);
        assert_eq!(sim.state.players[0].score, 1);
    }
}

