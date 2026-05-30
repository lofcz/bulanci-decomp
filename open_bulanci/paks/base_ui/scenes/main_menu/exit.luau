-- main_menu / exit — the Y/N quit confirmation modal (code-behind #4). The
-- backdrop, border, title, and Yes/No lines are declared in the scene under
-- the state-gated "exit" group; here we center the title (its width is
-- font-dependent) and handle the Enter/Y (quit) and Esc/N (cancel) keys.

local b = require("@bulanci")
local M = _G.MENU

-- Center the title in the 394-wide dialog (203 + (394 - tw) / 2).
local title = b.node("exit_title")
local tw = b.measure("Opravdu chcete skončit? TEST", 20, 0)
b.set_pos(title, 203 + (394 - tw) * 0.5, 240)

b.subscribe("update", function()
  if not b.state.active("exit") then return end
  if b.key_pressed("enter") or b.key_pressed("y") then
    b.app.exit()
  elseif b.key_pressed("escape") or b.key_pressed("n") then
    M.close_sub()
  end
end)
