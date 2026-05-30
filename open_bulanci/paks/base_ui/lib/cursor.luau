-- @cursor — the menu's red software cursor (CGunMouse), as a scene behavior.
--
-- Faithful Luau port of client/src/cursor.rs (CGunMouse_Draw @ 0x00424610,
-- CGunMouse_OnAnimTick @ 0x00423bd0). Rust now provides only the primitives:
--   * b.pointer()           — live OS mouse in 800x600 logical space,
--   * b.atlas(path)         — idle-twitch track frame count + per-frame motion,
--   * sprite / quad nodes    — the dot, the 4 crosshair arms, the reticle.
--
-- Everything else (the 30-sample / 5 ms follow ring giving ~150 ms lag, the
-- 85 ms idle-twitch stepping that drifts the dot along a closed motion path,
-- and the MSVCRT `(rand*48)>>19` 3-way track reroll) lives here in Luau.

local b = require("@bulanci")

local M = {}

local CAP = 30          -- coord ring capacity (== ~150 ms lag at 5 ms/push)
local RETICLE_W = 75
local RETICLE_H = 75
local RETICLE_C = 37    -- RETICLE_W / 2
local SCREEN_W = 800
local SCREEN_H = 600
local DOT_SEED_X = 2
local DOT_SEED_Y = 3
local DOT_TINT = 0xffffff80   -- white @ alpha 128 (LINE_ALPHA 0.5)
local LINE_TINT = 0xff000080  -- red @ alpha 128
local TRACKS = {
  "menu/cursor/cursor_idle_a",
  "menu/cursor/cursor_idle_b",
  "menu/cursor/cursor_idle_c",
}

-- opts: { parent }
function M.mount(opts)
  opts = opts or {}
  local parent = b.group(opts.parent or b.ROOT)

  -- ---- atlas metadata (frame count, origin, motion path per track) ----
  local atlases = {}
  for i, path in ipairs(TRACKS) do
    atlases[i] = b.atlas(path) or { count = 0, ox = 0, oy = 0, motion = {} }
  end

  -- ---- nodes: dot under the 4 crosshair arms under the reticle ----
  local dot = b.sprite(parent, TRACKS[1], 0)
  b.set_tint(dot, DOT_TINT)
  b.set_visible(dot, false)

  -- Each arm is a 1x1 quad scaled to its length each frame (draw_scene
  -- multiplies a quad's w/h by world scale), so we never rebuild geometry.
  local arm_l = b.quad(parent, 1, 1, LINE_TINT)
  local arm_r = b.quad(parent, 1, 1, LINE_TINT)
  local arm_t = b.quad(parent, 1, 1, LINE_TINT)
  local arm_b = b.quad(parent, 1, 1, LINE_TINT)

  local reticle = b.sprite(parent, "menu/cursor/cursor_reticle", 0)
  b.set_tint(reticle, DOT_TINT)

  -- ---- CGunMouse state ----
  local mouse_x, mouse_y = 400, 300
  local target_x, target_y = 400, 300
  local dot_offset_x, dot_offset_y = DOT_SEED_X, DOT_SEED_Y
  local dot_base_x, dot_base_y = DOT_SEED_X, DOT_SEED_Y
  local dot_needs_update = false
  local dot_ready = false
  local drift_track = 1     -- 1-based (Rust track 0 == A)
  local sprite_frame = 0    -- 0-based

  -- ---- coordinate ring (FIFO, fixed cap, silent drop when full) ----
  local ring = {}
  local head, tail, count = 1, 1, 0
  local function ring_push(x, y)
    if count < CAP then
      ring[tail] = { x, y }
      tail = (tail % CAP) + 1
      count = count + 1
    end
  end
  local function ring_pop()
    if count == 0 then return nil end
    local v = ring[head]
    head = (head % CAP) + 1
    count = count - 1
    return v
  end

  -- ---- MSVCRT rand() + the OnAnimTick (rand*48)>>19 3-way roll ----
  local rng = 1
  local function rand15()
    rng = (rng * 214013 + 2531011) % 4294967296
    return bit32.band(bit32.rshift(rng, 16), 0x7fff)
  end
  local function roll_three()
    return math.floor((rand15() * 48) / 524288)  -- == (r*48) >> 19, in {0,1,2}
  end

  -- ---- dot offset accumulator (CGunMouse::OnMouseMove) ----
  local function on_mouse_move(dx, dy)
    if not dot_needs_update then
      dot_base_x = dot_offset_x + dx
      dot_base_y = dot_offset_y + dy
      dot_needs_update = true
    else
      dot_base_x = dot_base_x + dx
      dot_base_y = dot_base_y + dy
    end
  end

  -- Frame start: promote last frame's accumulated base into the offset
  -- (CGunMouse::Erase), then snapshot the live mouse.
  b.subscribe("update", function()
    dot_offset_x = dot_base_x
    dot_offset_y = dot_base_y
    dot_needs_update = false
    local mx, my = b.pointer()
    mouse_x, mouse_y = mx, my
  end)

  -- 5 ms queue beat: push the live mouse, and once the ring is full pop the
  -- oldest sample into the dot target — the source of the ~150 ms follow lag.
  -- `on_fixed` hands us all the 5 ms steps that matured this frame in ONE
  -- call, so the hot loop stays Luau-side but costs a single FFI crossing.
  b.on_fixed(5, function(steps)
    for _ = 1, steps do
      ring_push(mouse_x, mouse_y)
      if count == CAP then
        local v = ring_pop()
        target_x, target_y = v[1], v[2]
        dot_ready = true
      end
    end
  end)

  -- 85 ms idle-twitch beat: apply the active track frame's (dx,dy) to the
  -- dot drift, advance the strip frame, reroll the track at end-of-loop.
  b.on_fixed(85, function(steps)
    local a = atlases[drift_track]
    if a.count <= 0 then return end
    for _ = 1, steps do
      a = atlases[drift_track]
      local n = a.count
      if n <= 0 then break end
      local i = sprite_frame % n
      local m = a.motion[i + 1]
      if m then on_mouse_move(m.dx, m.dy) end
      local nxt = sprite_frame + 1
      if nxt >= n then
        drift_track = roll_three() + 1
        sprite_frame = 0
      else
        sprite_frame = nxt
      end
    end
  end)

  -- Draw phase: lay out the dot, the 4 crosshair arms, and the reticle from
  -- the freshest target/offset/mouse the timers produced this frame.
  b.subscribe("draw", function()
    local mx = math.floor(mouse_x + 0.5)
    local my = math.floor(mouse_y + 0.5)
    local ret_left = mx - RETICLE_C
    local ret_top = my - RETICLE_C
    local ret_right = ret_left + RETICLE_W
    local ret_bottom = ret_top + RETICLE_H

    -- Dot (active idle-twitch frame) at target + drift offset - origin.
    if dot_ready then
      local a = atlases[drift_track]
      local n = a.count
      if n > 0 then
        local fi = sprite_frame % n
        b.set_sprite(dot, TRACKS[drift_track], fi)
        local cx = target_x + dot_offset_x
        local cy = target_y + dot_offset_y
        b.set_pos(dot, math.floor(cx - a.ox + 0.5), math.floor(cy - a.oy + 0.5))
        b.set_visible(dot, true)
      end
    end

    -- Crosshair arms: screen edge -> reticle edge, 1 px thick, hidden when
    -- the reticle is already against that edge.
    if ret_left > 0 then
      b.set_pos(arm_l, 0, my); b.set_scale(arm_l, ret_left, 1); b.set_visible(arm_l, true)
    else
      b.set_visible(arm_l, false)
    end
    if ret_right < SCREEN_W then
      b.set_pos(arm_r, ret_right, my); b.set_scale(arm_r, SCREEN_W - ret_right, 1); b.set_visible(arm_r, true)
    else
      b.set_visible(arm_r, false)
    end
    if ret_top > 0 then
      b.set_pos(arm_t, mx, 0); b.set_scale(arm_t, 1, ret_top); b.set_visible(arm_t, true)
    else
      b.set_visible(arm_t, false)
    end
    if ret_bottom < SCREEN_H then
      b.set_pos(arm_b, mx, ret_bottom); b.set_scale(arm_b, 1, SCREEN_H - ret_bottom); b.set_visible(arm_b, true)
    else
      b.set_visible(arm_b, false)
    end

    -- Reticle ring, top of the stack.
    b.set_pos(reticle, ret_left, ret_top)
  end)

  return parent
end

return M
