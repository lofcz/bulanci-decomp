-- main_menu / boot — scene bootstrap + shared navigation (code-behind #1).
--
-- The declarative tree (main_menu.scene.json) defines the static visuals, the
-- fully-declared `sg1` sub-screen control group, and the empty layer groups
-- (poem_layer, ruch_layer, cursor_layer) in exact paint order. This script
-- fills the empty layers from the libraries, attaches behavior to the declared
-- sg1 controls, and publishes the shared nav state (_G.MENU) the
-- dials/history/exit code-behind scripts drive. Runs first.

local b = require("@bulanci")
local fx = require("@fx")
local cursor = require("@cursor")
local poem = require("@poem")
local startgame1 = require("@startgame1")

-- Sub-screen conditional states (one active at a time, or none).
b.state.set("sg1", false)
b.state.set("history", false)
b.state.set("exit", false)

-- ---- shared navigation, used by dials/history/exit ----
local SUB = { [0] = "sg1", [1] = "history", [2] = "exit" }
local CLICK_SFX = { [0] = "menu/sfx/sfx_start", [1] = "menu/sfx/sfx_history", [2] = "menu/sfx/sfx_quit" }

local M = { SUB = SUB, CLICK_SFX = CLICK_SFX, current_sub = nil, sg1 = nil }

function M.close_sub()
  if M.current_sub then b.state.set(M.current_sub, false) end
  M.current_sub = nil
end

function M.click(idx)
  local sub = SUB[idx]
  if M.current_sub == sub then return end -- already active: no-op (CMenu)
  b.audio.play(CLICK_SFX[idx])
  if M.current_sub then b.state.set(M.current_sub, false) end
  b.state.set(sub, true)
  M.current_sub = sub
  if idx == 0 and M.sg1 then M.sg1.reset() end
end

_G.MENU = M

-- Layer 4: poem scroller.
poem.mount{ parent = b.node("poem_layer") }

-- Layer 6: StartGame1 sub-screen. Its controls are declared in the scene doc
-- under the `sg1` state group (editable in the editor); this attaches the
-- behavior to those named nodes (gated by state "sg1").
M.sg1 = startgame1.attach{ state = "sg1", on_close = M.close_sub }

-- Boot state: the retail CMenu ctor immediately "clicks" Start, which opens
-- StartGame1 and lights the Start dial. The matching SFX cue is played by the
-- Rust intro->menu transition (so it lands on the exact frame the menu
-- appears), so we only set the visual/sub-screen state here — no audio.
b.state.set("sg1", true)
M.current_sub = "sg1"
if M.sg1 then M.sg1.reset() end

-- Layer 7: CRuch phosphor scanlines (over the sub-screens).
fx.ruch{ parent = b.node("ruch_layer"), count = 4, width = 800, height = 600 }

-- Layer 8 (topmost): the CGunMouse software cursor.
cursor.mount{ parent = b.node("cursor_layer") }
