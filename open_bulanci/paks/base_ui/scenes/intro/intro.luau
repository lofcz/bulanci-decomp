-- intro — the CAdvertising startup splash (declarative scene).
--
-- The white full-screen backdrop + the centered splash sprite are declared in
-- intro.scene.json; this code-behind centers the splash (its pixel size is
-- asset-dependent) AND owns the entire dismissal policy: the 4s timeout, a
-- click anywhere, or any key. When any of those fires the scene pops itself
-- off the surface stack (`scene.pop`); the client picks that up and runs the
-- intro->menu handoff (menu slots + the appearance sfx + day/night), which is
-- the only part that has to stay on the Rust side (it crosses the app-phase +
-- scheduler boundary the scene can't own). Retiming or re-triggering the
-- splash is now a pure mod edit here — no engine rebuild.

local b = require("@bulanci")

-- Center the splash over the 800x600 logical screen (mirrors the old hardcoded
-- `draw_intro` math). `node`/`image_size` are nil-safe: the splash only moves
-- once both its node and its measured size are known.
local splash = b.node("intro_splash")
local w, h = b.image_size("menu/intro/intro_splash")
if splash and w and h then
  b.set_pos(splash, math.floor((800 - w) * 0.5), math.floor((600 - h) * 0.5))
end

-- Dismiss at most once, no matter which trigger wins. `scene.pop` retracts
-- this overlay; the client then hands off to the menu beneath.
local dismissed = false
local function dismiss()
  if dismissed then return end
  dismissed = true
  b.scene.pop()
end

-- Trigger 1 — the 4000ms CAdvertising timeout (anchored at scene load).
b.after(4000, dismiss)

-- Trigger 2 — a click anywhere: a full-screen input region makes the whole
-- splash hot, and `press` fires on the mouse-down edge (matching the original
-- left-button-down dismissal).
local hit = b.input_region(b.ROOT, 800, 600)
b.on_input(hit, "press", dismiss)

-- Trigger 3 — any key. The wildcard code-behind `key` hook fires for every key
-- that goes down this frame.
return {
  key = function() dismiss() end,
}
