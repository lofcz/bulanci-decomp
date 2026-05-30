-- @fx — background effects composed from the scene-engine primitives.
--
-- This is the Luau counterpart of the bespoke Rust effect modules
-- (client/src/ruch.rs, …). The point of porting them here: an effect is
-- "just a scene" — a few nodes plus a behavior closure. Rust only provides
-- the primitives (quad / set_visible / set_pos / after / rand); all of the
-- state machine lives in Luau, so it is moddable and a higher pak can ship
-- its own "@fx".

local b = require("@bulanci")

local M = {}

-- CRuch — N independent vertical phosphor scanlines that flash briefly at a
-- random column. Faithful to the retail state machine documented in
-- ruch.rs (CRuch_ctor / CRuch_OnScheduledTick @ 0x00423c20 / 0x00423de0):
--
--   * each line starts HIDDEN with a random initial delay of [0, 4000] ms,
--   * the visible window is 100 ms,
--   * the hidden window is a fresh random [0, 4000] ms,
--   * the column X re-rolls on the visible -> hidden edge (not the reverse),
--   * lines are independent — no spacing constraint, can overlap.
--
-- The 0x3f3f3f solid-gray column is a 1px-wide quad spanning the screen
-- height. Per-line state ("visible") lives in the closure, not in Rust.
--
-- opts: { parent, count, width, height, color, state }
function M.ruch(opts)
  opts = opts or {}
  local parent = opts.parent or b.ROOT
  local count  = opts.count or 4
  local width  = opts.width or 800
  local height = opts.height or 600
  local color  = opts.color or 0x3f3f3fff
  local state  = opts.state

  -- A 0ms delay is "inactive" to the scheduler, so floor the random delays
  -- at 1ms — visually identical to the original's [0,4000] roll.
  local function hidden_delay() return math.max(1, b.rand(0, 4000)) end

  for _ = 1, count do
    local line = b.quad(parent, 1, height, color)
    if state then b.set_state_mask(line, b.state_bit(state)) end
    b.set_pos(line, b.rand(0, width - 1), 0)
    b.set_visible(line, false)

    local visible = false
    local function tick()
      if visible then
        visible = false
        b.set_visible(line, false)
        b.set_pos(line, b.rand(0, width - 1), 0)
        b.after(hidden_delay(), tick)
      else
        visible = true
        b.set_visible(line, true)
        b.after(100, tick)
      end
    end

    b.after(hidden_delay(), tick)
  end
end

return M
