-- main_menu / dials — the three dial buttons' glow/dim state machine, click
-- handlers, and keyboard shortcuts (code-behind #2; menu.rs::update_menu_input).
--
-- The dial/icon sprites + hit regions are declared in the scene
-- (dial{0,1,2}_btn / _icon / _rg); here we resolve them by name and drive
-- them. The state machine is the bespoke one: uninterruptible 8-frame @ 71 ms
-- tracks gated by an armed flag; the hover voice cue fires only on a committed
-- glow-up while the gate is open (the "delayed SFX").

local b = require("@bulanci")
local M = _G.MENU

local MENU_BTN_OFF = "menu/widgets/menu_button/menu_button_off" -- track 0 (glow-up)
local MENU_BTN_ON = "menu/widgets/menu_button/menu_button_on"   -- track 1 (dim-down)
local ICON = {
  [0] = { "menu/icons/icon_start", "menu/icons/icon_start_hi" },
  [1] = { "menu/icons/icon_history", "menu/icons/icon_history_hi" },
  [2] = { "menu/icons/icon_quit", "menu/icons/icon_quit_hi" },
}

local dial_node, label_node, dial_region = {}, {}, {}
-- Boot-seeded rest state: fully dim (track 1, frame 7), gate armed.
local d_track = { [0] = 1, [1] = 1, [2] = 1 }
local d_frame = { [0] = 7, [1] = 7, [2] = 7 }
local d_armed = { [0] = true, [1] = true, [2] = true }

for idx = 0, 2 do
  dial_node[idx] = b.node("dial" .. idx .. "_btn")
  label_node[idx] = b.node("dial" .. idx .. "_icon")
  dial_region[idx] = b.node("dial" .. idx .. "_rg")
  local captured = idx
  b.on_input(dial_region[idx], "click", function() M.click(captured) end)
end

b.subscribe("update", function()
  local dt = b.dt()
  for idx = 0, 2 do
    local is_active = M.current_sub == M.SUB[idx]
    local hovered = b.hovered(dial_region[idx])
    local want_glow = is_active or hovered

    if d_armed[idx] then
      if d_track[idx] == 1 and want_glow then
        d_track[idx] = 0; d_frame[idx] = 0; d_armed[idx] = false
        if not is_active then b.audio.play("menu/sfx/sfx_hover") end
      elseif d_track[idx] == 0 and not want_glow then
        d_track[idx] = 1; d_frame[idx] = 0; d_armed[idx] = false
      end
    else
      d_frame[idx] = math.min(d_frame[idx] + dt / 71, 7)
      if d_frame[idx] >= 7 then
        d_armed[idx] = true
        if d_track[idx] == 0 and not want_glow then
          d_track[idx] = 1; d_frame[idx] = 0; d_armed[idx] = false
        elseif d_track[idx] == 1 and want_glow then
          d_track[idx] = 0; d_frame[idx] = 0; d_armed[idx] = false
          if not is_active then b.audio.play("menu/sfx/sfx_hover") end
        end
      end
    end

    local path = (d_track[idx] == 0) and MENU_BTN_OFF or MENU_BTN_ON
    b.set_sprite(dial_node[idx], path, math.floor(d_frame[idx] + 0.5))
    local highlight = is_active or hovered
    b.set_sprite(label_node[idx], ICON[idx][highlight and 2 or 1])
  end
end)

-- Keyboard shortcuts (CMenu::OnKeyDown): always live in the menu.
b.on_key("s", function() M.click(0) end)
b.on_key("h", function() M.click(1) end)
b.on_key("k", function() M.click(2) end)
b.on_key("x", function() M.click(2) end)
